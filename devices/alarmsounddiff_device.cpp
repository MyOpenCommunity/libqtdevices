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


#include "alarmsounddiff_device.h"
#include "hardware_functions.h" // AMPLI_NUM
#include "media_device.h"

#include <openmsg.h>

#include <QStringList>


#define ARRAY_SIZE(x) int(sizeof(x)/sizeof(x[0]))

enum
{
	_WHERE_SOURCE    = 2,
	_WHERE_AMPLIFIER = 3,
	_WHERE_GENERAL   = 5,

	_DIM_SOURCE_TRACK = 6,
	_DIM_SOURCE_STATION = 11,
	_DIM_SOURCE_ON = 2,

	_DIM_AMPLIFIER_VOLUME = 1,
	_DIM_AMPLIFIER_STATE = 12,
};

AlarmSoundDiffDevice::AlarmSoundDiffDevice(bool _multichannel)
	: device("22", ""), receive_frames(false)
{
	is_multichannel = _multichannel;
}

void AlarmSoundDiffDevice::setReceiveFrames(bool receive)
{
	receive_frames = receive;
}

void AlarmSoundDiffDevice::startAlarm(int source, int radio_station, int *alarmVolumes)
{
	bool environments[AMPLI_NUM / 10 + 1];
	for (int i = 0; i < ARRAY_SIZE(environments); ++i)
		environments[i] = false;

	RadioSourceDevice source_device(QString::number(source));
	source_device.turnOn("0");
	if (radio_station)
		source_device.setStation(QString::number(radio_station));

	for (int amplifier = 0; amplifier < AMPLI_NUM; ++amplifier)
	{
		if (alarmVolumes[amplifier] < 0)
			continue;

		int environment = amplifier / 10;
		if (is_multichannel && environment > 0 && environment < 9)
		{
			if (!environments[environment])
			{
				source_device.turnOn(QString::number(environment));
				environments[environment] = true;
			}
		}
		QString address = QString("%1%2").arg(amplifier < 10 ? "0" : "").arg(amplifier);

		AmplifierDevice ampli_device(address);
		int vol = alarmVolumes[amplifier] < 10 ? alarmVolumes[amplifier] : 8;

		ampli_device.setVolume(vol);
		ampli_device.turnOn();
	}
}

void AlarmSoundDiffDevice::stopAlarm(int source, int *alarmVolumes)
{
	for (int amplifier = 0; amplifier < AMPLI_NUM; ++amplifier)
	{
		if (alarmVolumes[amplifier] < 0)
			continue;

		QString address = QString("%1%2").arg(amplifier < 10 ? "0" : "").arg(amplifier);
		AmplifierDevice ampli_device(address);
		ampli_device.turnOff();
	}
}

void AlarmSoundDiffDevice::setRadioStation(int source, int radio_station)
{
	QString f = QString("*#22*2#%1*#6*%2##").arg(source).arg(radio_station);
	sendFrame(f);
}

void AlarmSoundDiffDevice::activateSource(int source)
{
	QString f = QString("*22*35#4#0#%1*3#0#0##").arg(source);
	sendFrame(f);
}

void AlarmSoundDiffDevice::activateEnvironment(int environment, int source)
{
	QString f = QString("*22*35#4#%1#%2*3#%1#0##").arg(environment).arg(source);
	sendFrame(f);
}

void AlarmSoundDiffDevice::setVolume(int amplifier, int volume)
{
	QString f = QString("*#22*3#%1#%2*#1*%3##").arg(amplifier / 10).arg(amplifier % 10).arg(volume);
	sendFrame(f);
}

void AlarmSoundDiffDevice::amplifierOn(int amplifier)
{
	QString f = QString("*22*34#4#%1*3#%1#%2##").arg(amplifier / 10).arg(amplifier % 10);
	sendFrame(f);
}

void AlarmSoundDiffDevice::amplifierOff(int amplifier)
{
	QString f = QString("*22*0#4#%1*3#%1#%2##").arg(amplifier / 10).arg(amplifier % 10);
	sendFrame(f);
}

bool AlarmSoundDiffDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (!receive_frames)
		return false;

	int where = msg.where();
	int what = msg.what();

	// where can have the form:
	// 2#<source id>        source
	// 5#2#<source id>      general (source)
	// 3#<ambient>#<point>  amplifier

	int source = -1;
	if (where == _WHERE_GENERAL && msg.whereArg(0) == "2")
		source = QString::fromStdString(msg.whereArg(1)).toInt();
	else if (where == _WHERE_SOURCE)
		source = QString::fromStdString(msg.whereArg(0)).toInt();

	if (source != -1)
	{
		values_list[DIM_SOURCE] = source;

		if (what == _DIM_SOURCE_STATION)
		{
			// got radio station
			int station = msg.whatArgN(2);

			values_list[DIM_RADIO_STATION] = station;
		}
		else if (what == _DIM_SOURCE_ON)
		{
			// request the radio station to check if the source is a radio
			QString f = QString("*#22*2#%1*11##").arg(source);
			sendInit(f);
		}
		else if (what == _DIM_SOURCE_TRACK)
		{
			// got radio station/track
			int station = msg.whatArgN(0);

			values_list[DIM_RADIO_STATION] = station;
		}
	}
	else if (where == _WHERE_AMPLIFIER)
	{
		if (msg.whereArgCnt() != 2)
			return false;
		int environment = QString::fromStdString(msg.whereArg(0)).toInt();
		int amplifier = QString::fromStdString(msg.whereArg(1)).toInt();

		if (what == _DIM_AMPLIFIER_STATE)
		{
			// got on/off state
			int state = msg.whatArgN(0);

			// if we got an "on" state, wait for the volume to emit the
			// valueReceived notification
			if (state == 0)
			{
				values_list[DIM_AMPLIFIER] = environment * 10 + amplifier;
				values_list[DIM_STATUS] = false;
			}
		}
		else if (what == _DIM_AMPLIFIER_VOLUME)
		{
			// got the volume
			int volume = msg.whatArgN(0);

			values_list[DIM_AMPLIFIER] = environment * 10 + amplifier;
			values_list[DIM_STATUS] = true;
			values_list[DIM_VOLUME] = volume;
		}
	}

	if (values_list.count() > 0)
		return true;

	return false;
}
