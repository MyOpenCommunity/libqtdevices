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
#include "frame_functions.h" // createWriteDimensionFrame, isCommandFrame, isDimensionFrame, isWriteDimensionFrame
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

int dimmer100LevelTo10(int level)
{
	if (level == 0)
		return 0;
	else if (level <= 4)
		return 2;
	else if (level <= 15)
		return 3;
	else if (level <= 25)
		return 4;
	else if (level <= 35)
		return 5;
	else if (level <= 45)
		return 6;
	else if (level <= 53)
		return 7;
	else if (level <= 67)
		return 8;
	else if (level <= 85)
		return 9;
	else
		return 10;
}



LightingDevice::LightingDevice(QString where, PullMode pull, int openserver_id, int pull_delay, AdvancedMode adv, PullStateManager::FrameChecker checker) :
	PullDevice(QString("1"), where, pull, openserver_id, pull_delay, adv, checker)
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

void LightingDevice::turnOff()
{
	sendCommand(LIGHT_OFF);
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

FrameHandled LightingDevice::isFrameHandled(OpenMsg &msg)
{
	int what = msg.what();

	// dimmer 100 on/off
	if (isCommandFrame(msg) && (what == 1 || what == 0) && msg.whatArgCnt() == 1)
		return FRAME_MAYBE_HANDLED;

	// timed light
	if ((isDimensionFrame(msg) || isWriteDimensionFrame(msg)) && what == DIM_VARIABLE_TIMING)
		return FRAME_MAYBE_HANDLED;

	// light commands
	if (isCommandFrame(msg) && (what == DIM_DEVICE_ON || what == DIM_DEVICE_OFF))
		return FRAME_HANDLED;

	if (what >= FIXED_TIMING_MIN && what <= FIXED_TIMING_MAX)
		return FRAME_HANDLED;

	return FRAME_NOT_HANDLED;
}

bool LightingDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	int what = msg.what();

	switch (what)
	{
	case DIM_DEVICE_ON:
	case DIM_DEVICE_OFF:
		if (isCommandFrame(msg) &&
		    (msg.whatArgCnt() == 0 || (msg.whatArgCnt() == 1 && isAdvanced())))
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


DimmerDevice::DimmerDevice(QString where, PullMode pull, int openserver_id, int pull_delay, AdvancedMode adv, PullStateManager::FrameChecker checker) :
	LightingDevice(where, pull, openserver_id, pull_delay, adv, checker)
{
	 level = 0;
	 status = false;

	 delayed_level_request.setSingleShot(true);
	 delayed_level_request.setInterval(pull_delay);
	 connect(&delayed_level_request, SIGNAL(timeout()), SLOT(delayedLevelRequest()));
}

void DimmerDevice::delayedLevelRequest()
{
	requestStatus();
}

void DimmerDevice::increaseLevel()
{
	sendCommand(DIMMER_INC);
}

void DimmerDevice::decreaseLevel()
{
	sendCommand(DIMMER_DEC);
}

FrameHandled DimmerDevice::isFrameHandled(OpenMsg &msg)
{
	if (LightingDevice::isFrameHandled(msg) == FRAME_HANDLED)
		return FRAME_HANDLED;

	int what = msg.what();

	// dimmer 100 on/off
	if (isCommandFrame(msg) && (what == 1 || what == 0) && msg.whatArgCnt() == 1)
		return FRAME_MAYBE_HANDLED;

	// dimmer 100 set level
	if ((isDimensionFrame(msg) || isWriteDimensionFrame(msg)) && what == DIMMER100_STATUS)
		return FRAME_MAYBE_HANDLED;

	// dimmer 100 increase/decrease level
	if (isCommandFrame(msg) && (what == DIMMER_INC || what == DIMMER_DEC) && msg.whatArgCnt() == 2)
		return FRAME_MAYBE_HANDLED;

	// timed light
	if ((isDimensionFrame(msg) || isWriteDimensionFrame(msg)) && what == DIM_VARIABLE_TIMING)
		return FRAME_MAYBE_HANDLED;

	// dimmer commands
	if (isCommandFrame(msg) && (what >= DIMMER10_LEVEL_MIN && what <= DIMMER10_LEVEL_MAX))
		return FRAME_HANDLED;

	if (isCommandFrame(msg) && (what == DIM_DIMMER_PROBLEM || what == DIMMER_INC || what == DIMMER_DEC))
		return FRAME_HANDLED;

	return FRAME_NOT_HANDLED;
}

bool DimmerDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	LightingDevice::parseFrame(msg, values_list);

	if (values_list.contains(DIM_DEVICE_ON))
	{
		 status = values_list[DIM_DEVICE_ON].toBool();
		 // when a dimmer is turned on by a global/environment command, and we do not know its level, we
		 // need to issue a status request in order to get the level
		 if (status && level == 0 && checkAddressIsForMe(QString::fromStdString(msg.whereFull()), where) != P2P)
			 delayed_level_request.start();
		 else
			 delayed_level_request.stop();

		if (status && level != 0)
			 values_list[DIM_DIMMER_LEVEL] = dimmer100LevelTo10(level);
	}

	int what = msg.what();

	if (isCommandFrame(msg))
	{
		if (what >= DIMMER10_LEVEL_MIN && what <= DIMMER10_LEVEL_MAX)
		{
			level = dimmerLevelTo100(what);
			values_list[DIM_DEVICE_ON] = true;
			values_list[DIM_DIMMER_LEVEL] = dimmer100LevelTo10(level);
		}
		else if (what == DIM_DIMMER_PROBLEM)
			values_list[what] = true;
	}

	// dimmer 10 increment/decrement
	if ((what == DIMMER_INC || what == DIMMER_DEC) && msg.whatArgCnt() == 0)
	{
		if (status)
		{
			int dimmer10level = dimmer100LevelTo10(level);

			if (what == DIMMER_INC)
				dimmer10level += 1;
			else
				dimmer10level -= 1;

			dimmer10level = qMin(qMax(dimmer10level, 2), 10);
			level = dimmerLevelTo100(dimmer10level);
			values_list[DIM_DIMMER_LEVEL] = dimmer10level;
		}
		else
		{
			status = true;
			values_list[DIM_DIMMER_LEVEL] = dimmer100LevelTo10(level);
		}
		values_list[DIM_DEVICE_ON] = true;
	}

	// dimmer 100 increment/decrement for advanced dimmers
	if ((what == DIMMER_INC || what == DIMMER_DEC) && msg.whatArgCnt() == 2 && isAdvanced())
	{
		if (status)
		{
			int delta = msg.whatArgN(0);

			if (what == DIMMER_INC)
				level += delta;
			else
				level -= delta;

			level = qMin(qMax(level, 1), 100);
			values_list[DIM_DIMMER_LEVEL] = dimmer100LevelTo10(level);
		}
		else
		{
			status = true;
			values_list[DIM_DIMMER_LEVEL] = dimmer100LevelTo10(level);
		}
		values_list[DIM_DEVICE_ON] = true;
	}

	// dimmer 100 set status, for advanced dimmers
	if (what == DIMMER100_STATUS && (isDimensionFrame(msg) || isWriteDimensionFrame(msg)) && isAdvanced())
	{
		Q_ASSERT_X(msg.whatArgCnt() == 2, "DimmerDevice::parseFrame",
			"Dimmer 100 status frame must have 2 what args");

		int new_level = msg.whatArgN(0) - 100;

		// if level == 0 device is off
		if (new_level == 0)
		{
			values_list[DIM_DEVICE_ON] = status = false;
		}
		else
		{
			level = new_level;

			values_list[DIM_DEVICE_ON] = status = true;
			values_list[DIM_DIMMER_LEVEL] = dimmer100LevelTo10(level);
		}
	}

	return !values_list.isEmpty();
}


Dimmer100Device::Dimmer100Device(QString where, PullMode pull, int openserver_id, int pull_delay, PullStateManager::FrameChecker checker) :
	DimmerDevice(where, pull, openserver_id, pull_delay, PULL_ADVANCED, checker)
{
}

void Dimmer100Device::init()
{
	DimmerDevice::init();
	requestDimmer100Status();
}

void Dimmer100Device::delayedLevelRequest()
{
	requestDimmer100Status();
}

void Dimmer100Device::turnOn(int speed)
{
	sendCommand(QString("%1#%2").arg(LIGHT_ON).arg(speed));
}

void Dimmer100Device::turnOff(int speed)
{
	sendCommand(QString("%1#%2").arg(LIGHT_OFF).arg(speed));
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

	// DimmerDevice::parseFrame already performed the operation for us, and updated level
	// with the dimmer 100 level, we only need to set the dimmer 100 dimension
	if (values_list.contains(DIM_DIMMER_LEVEL))
		values_list[DIM_DIMMER100_LEVEL] = level;

	int what = msg.what();

	if (isCommandFrame(msg) && (what == DIM_DEVICE_ON || what == DIM_DEVICE_OFF) && msg.whatArgCnt() == 1)
		values_list[DIM_DEVICE_ON] = (what == DIM_DEVICE_ON);

	if (what == DIMMER100_STATUS && isDimensionFrame(msg))
	{
		Q_ASSERT_X(msg.whatArgCnt() == 2, "Dimmer100Device::parseFrame",
			"Dimmer 100 status frame must have 2 what args");

		// status/level handling performed in DimmerDevice
		if (status)
			values_list[DIM_DIMMER100_SPEED] = msg.whatArgN(1);
	}

	// the level adjustment is already performed in DimmerDevice::parseFrame, we only
	// need to send the status update
	if ((what == DIMMER_INC || what == DIMMER_DEC) && msg.whatArgCnt() == 2)
		values_list[DIM_DIMMER100_LEVEL] = level;

	return !values_list.isEmpty();
}
