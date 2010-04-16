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

#include <stdlib.h> // atoi
#include <openmsg.h>

#include <QStringList>

#define ARRAY_SIZE(x) int(sizeof(x)/sizeof(x[0]))


AlarmSoundDiffDevice::AlarmSoundDiffDevice()
	: device("22", ""), receive_frames(false)
{
}

void AlarmSoundDiffDevice::setReceiveFrames(bool receive)
{
	receive_frames = receive;
}

void AlarmSoundDiffDevice::startAlarm(int source, int radio_station, int *alarmVolumes)
{
	if (radio_station)
		setRadioStation(source, radio_station);
	activateSource(source);

	bool environments[AMPLI_NUM / 10 + 1];
	for (int i = 0; i < ARRAY_SIZE(environments); ++i)
		environments[i] = false;

	for (int amplifier = 0; amplifier < AMPLI_NUM; ++amplifier)
	{
		if (alarmVolumes[amplifier] < 0)
			continue;

		int environment = amplifier / 10;
		if (environment > 0 && environment < 9)
			if (!environments[environment])
				activateEnvironment(environment, source);
		if (alarmVolumes[amplifier] < 10)
			setVolume(amplifier, alarmVolumes[amplifier]);
		else
			setVolume(amplifier, 8);
		amplifierOn(amplifier);
	}
}

void AlarmSoundDiffDevice::stopAlarm(int source, int *alarmVolumes)
{
	for (int amplifier = 0; amplifier < AMPLI_NUM; ++amplifier)
	{
		if (alarmVolumes[amplifier] < 0)
			continue;

		amplifierOff(amplifier);
	}
}

void AlarmSoundDiffDevice::setRadioStation(int source, int radio_station)
{
	QString f = QString("*#22*2#%1*#6*%2##").arg(source).arg(radio_station);
	sendFrame(f);
}

void AlarmSoundDiffDevice::activateSource(int source)
{
	QString f = QString("*22*35#4#0#%1*3#1#0##").arg(source);
	sendFrame(f);
}

void AlarmSoundDiffDevice::activateEnvironment(int environment, int source)
{
	QString f = QString("*22*35#4#%1#%2*3*1*%1*0##").arg(environment).arg(source);
	sendFrame(f);
}

void AlarmSoundDiffDevice::setVolume(int amplifier, int volume)
{
	QString f = QString("*#22*3#%1#%2*#1*%3##").arg(amplifier / 10).arg(amplifier % 10).arg(volume);
	sendFrame(f);
}

void AlarmSoundDiffDevice::amplifierOn(int amplifier)
{
	QString f = QString("*22*1#4#%1*3#%1#%2##").arg(amplifier / 10).arg(amplifier % 10);
	sendFrame(f);
}

void AlarmSoundDiffDevice::amplifierOff(int amplifier)
{
	QString f = QString("*22*0#4#%1*3#%1#%2##").arg(amplifier / 10).arg(amplifier % 10);
	sendFrame(f);
}

void AlarmSoundDiffDevice::manageFrame(OpenMsg &msg)
{
	if (!receive_frames)
		return;

	int where = msg.where();
	int what = msg.what();

	DeviceValues values_list;

	if (where == 5)
	{
		// got active source, request radio station
		QStringList l = QString(msg.whereFull().c_str()).split('#');
		if (l.size() != 3)
			return;

		int source = l[2].toInt();

		values_list[DIM_SOURCE] = source;

		// request the radio station to check if the source is a radio
		QString f = QString("*#22*2#%1*11##").arg(source);
		sendInit(f);
	}
	else if (where == 2)
	{
		if (what == 11)
		{
			// got radio station
			int station = msg.whatArgN(2);

			values_list[DIM_RADIO_STATION] = station;
		}
	}
	else if (where == 3)
	{
		if (msg.whereArgCnt() != 2)
			return;
		int environment = atoi(msg.whereArg(0).c_str());
		int amplifier = atoi(msg.whereArg(1).c_str());

		if (what == 12)
		{
			// got on/off state
			int state = msg.whatArgN(0);

			// if we got an "on" state, wait for the volume to emit the
			// status_changed notification
			if (state == 0)
			{
				values_list[DIM_AMPLIFIER] = environment * 10 + amplifier;
				values_list[DIM_STATUS] = false;
			}
		}
		else if (what == 1)
		{
			// got the volume
			int volume = msg.whatArgN(0);

			values_list[DIM_AMPLIFIER] = environment * 10 + amplifier;
			values_list[DIM_STATUS] = true;
			values_list[DIM_VOLUME] = volume;
		}
	}

	if (values_list.count() > 0)
		emit status_changed(values_list);
}
