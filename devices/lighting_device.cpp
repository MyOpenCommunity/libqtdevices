/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "lighting_device.h"
#include "openmsg.h"
#include "generic_functions.h"
#include "bttime.h" // BtTime

#include <QDebug>


enum
{
	LIGHT_ON = 1,
	LIGHT_OFF = 0,
	FIXED_TIMING_MIN = 11,
	FIXED_TIMING_MAX = 18,
	DIMMER10_LEVEL_MIN = 2,
	DIMMER10_LEVEL_MAX = 10,
	DIMMER_INC = 30,
	DIMMER_DEC = 31,
	DIMMER100_STATUS = 1,
};

enum
{
	DIM_DEVICE_OFF = 0,
};


int dimmerLevelTo100(int level)
{
	switch (level)
	{
	case 2:
		return 1;
	case 9:
		return 75;
	case 10:
		return 100;
	default:
		return (level - 2) * 10;
	}
}



LightingDevice::LightingDevice(QString where, PullMode pull, int openserver_id, int pull_delay) :
	PullDevice(QString("1"), where, pull, openserver_id, pull_delay)
{
	timed_light = NOT_TIMED_LIGHT;
}

void LightingDevice::init()
{
	requestStatus();
	if (timed_light == TIMED_LIGHT)
		requestVariableTiming();
}

void LightingDevice::turnOn()
{
	sendCommand(LIGHT_ON);
}

void LightingDevice::turnOn(int speed)
{
	sendCommand(QString("%1#%2").arg(LIGHT_ON).arg(speed));
}

void LightingDevice::turnOff()
{
	sendCommand(LIGHT_OFF);
}

void LightingDevice::turnOff(int speed)
{
	sendCommand(QString("%1#%2").arg(LIGHT_OFF).arg(speed));
}

void LightingDevice::fixedTiming(int value)
{
	int v = FIXED_TIMING_MIN + value;
	if (v >= FIXED_TIMING_MIN && v <= FIXED_TIMING_MAX)
		sendCommand(v);
}

// Limitations:
// 0 <= h <= 255
// 0 <= m <= 59
// 0 <= s <= 59
void LightingDevice::variableTiming(int h, int m, int s)
{
	if ((h >= 0 && h <= 255) && (m >= 0 && m <= 59) && (s >= 0 && s <= 59))
		sendFrame(createWriteDimensionFrame(who, QString("%1*%2*%3*%4").arg(DIM_VARIABLE_TIMING)
			.arg(h).arg(m).arg(s), where));
}

void LightingDevice::requestStatus()
{
	sendRequest(QString());
}

void LightingDevice::requestVariableTiming()
{
	sendRequest(DIM_VARIABLE_TIMING);
}


void LightingDevice::requestPullStatus()
{
	requestStatus();
}

void LightingDevice::setTimingBehaviour(Timed t)
{
	timed_light = t;
}

bool LightingDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	int what = msg.what();

	switch (what)
	{
	case DIM_DEVICE_ON:
	case DIM_DEVICE_OFF:
		if (isCommandFrame(msg))
			values_list[DIM_DEVICE_ON] = what == DIM_DEVICE_ON;
		break;
	case DIM_VARIABLE_TIMING:
		if (isDimensionFrame(msg))
		{
			Q_ASSERT_X(msg.whatArgCnt() == 3, "LightingDevice::parseFrame",
				"Variable timing message has more than 3 what args");
			int hour = msg.whatArgN(0);
			int minute = msg.whatArgN(1);
			int second = msg.whatArgN(2);
			if (hour == 255 && minute == 255 && second == 255)
				return false;
			BtTime t(hour, minute, second);
			QVariant v;
			v.setValue(t);
			values_list[what] = v;
		}
		break;
	}
	// handle fixed timing status changing. This is really useful for environment frames only, since
	// for point-to-point frames we also get a ON/OFF status update directly from the device.
	// It won't hurt handling twice the state (for point-to-point frames).
	if (what >= FIXED_TIMING_MIN && what <= FIXED_TIMING_MAX)
		values_list[DIM_DEVICE_ON] = true;

	return !values_list.isEmpty();
}


DimmerDevice::DimmerDevice(QString where, PullMode pull, int openserver_id, int pull_delay) :
	LightingDevice(where, pull, openserver_id, pull_delay)
{
}

void DimmerDevice::increaseLevel()
{
	sendCommand(DIMMER_INC);
}

void DimmerDevice::decreaseLevel()
{
	sendCommand(DIMMER_DEC);
}

bool DimmerDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	LightingDevice::parseFrame(msg, values_list);

	if (isCommandFrame(msg))
	{
		int what = msg.what();

		if (what >= DIMMER10_LEVEL_MIN && what <= DIMMER10_LEVEL_MAX)
			values_list[DIM_DIMMER_LEVEL] = what;
		else if (what == DIM_DIMMER_PROBLEM)
			values_list[what] = true;
	}
	return !values_list.isEmpty();
}



Dimmer100Device::Dimmer100Device(QString where, PullMode pull, int openserver_id, int pull_delay) :
	DimmerDevice(where, pull, openserver_id, pull_delay)
{
}

void Dimmer100Device::init()
{
	DimmerDevice::init();
	requestDimmer100Status();
}

void Dimmer100Device::increaseLevel100(int delta, int speed)
{
	sendCommand(QString("%1#%2#%3").arg(DIMMER_INC).arg(delta).arg(speed));
}

void Dimmer100Device::decreaseLevel100(int delta, int speed)
{
	sendCommand(QString("%1#%2#%3").arg(DIMMER_DEC).arg(delta).arg(speed));
}

void Dimmer100Device::requestDimmer100Status()
{
	sendRequest(DIMMER100_STATUS);
}

void Dimmer100Device::requestPullStatus()
{
	requestDimmer100Status();
}

bool Dimmer100Device::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	DimmerDevice::parseFrame(msg, values_list);
	int what = msg.what();

	if (what == DIMMER100_STATUS && isDimensionFrame(msg))
	{
		Q_ASSERT_X(msg.whatArgCnt() == 2, "Dimmer100Device::parseFrame",
			"Dimmer 100 status frame must have 2 what args");
		// convert the value in 0-100 range
		int level = msg.whatArgN(0) - 100;

		// if level == 0 device is off
		if (level == 0)
			values_list[DIM_DEVICE_ON] = false;
		else
		{
			values_list[DIM_DIMMER100_LEVEL] = level;
			values_list[DIM_DIMMER100_SPEED] = msg.whatArgN(1);
		}
	}
	return !values_list.isEmpty();
}
