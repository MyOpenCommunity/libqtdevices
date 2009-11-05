#include "alarmsounddiff_device.h"
#include "hardware_functions.h" // AMPLI_NUM

#include <stdlib.h> // atoi
#include <openmsg.h>

#include <QStringList>

#define ARRAY_SIZE(x) int(sizeof(x)/sizeof(x[0]))


AlarmSoundDiffDevice::AlarmSoundDiffDevice()
	: device("22", "")
{
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
		setVolume(environment, amplifier, alarmVolumes[amplifier]);
		amplifierOn(environment, amplifier);
	}
}

void AlarmSoundDiffDevice::stopAlarm(int source, int *alarmVolumes)
{
	for (int amplifier = 0; amplifier < AMPLI_NUM; ++amplifier)
	{
		if (alarmVolumes[amplifier] < 0)
			continue;

		int environment = amplifier / 10;
		amplifierOff(environment, amplifier);
	}
}

void AlarmSoundDiffDevice::setRadioStation(int source, int radio_station)
{
	QString f = QString("*#22*2#%1*#6*%2##").arg(source - 100).arg(radio_station);
	sendFrame(f);
}

void AlarmSoundDiffDevice::activateSource(int source)
{
	QString f = QString("*22*35#4#0#%1*3#1#0##").arg(source - 100);
	sendFrame(f);
}

void AlarmSoundDiffDevice::activateEnvironment(int environment, int source)
{
	QString f = QString("*22*35#4#%1#%2*3*1*%1*0##").arg(environment).arg(source - 100);
	sendFrame(f);
}

void AlarmSoundDiffDevice::setVolume(int environment, int amplifier, int volume)
{
	QString f = QString("*#22*3#%1#%2*#1*%3##").arg(environment).arg(amplifier).arg(volume);
	sendFrame(f);
}

void AlarmSoundDiffDevice::amplifierOn(int environment, int amplifier)
{
	QString f = QString("*22*1#4#%1*3#%1#%2##").arg(environment).arg(amplifier);
	sendFrame(f);
}

void AlarmSoundDiffDevice::amplifierOff(int environment, int amplifier)
{
	QString f = QString("*22*0#4#%1*3#%1#%2##").arg(environment).arg(amplifier);
	sendFrame(f);
}

void AlarmSoundDiffDevice::manageFrame(OpenMsg &msg)
{
	int where = msg.where();
	int what = msg.what();

	StatusList sl;

	if (where == 2)
	{
		if (what == 5)
		{
			// got active source, request radio station
			QStringList l = QString(msg.Extract_grandezza()).split('#');
			if (l.size() != 3)
				return;

			int source = l[2].toInt();

			sl[DIM_SOURCE] = source;

			// request the radio station to check if the source is a radio
			QString f = QString("*#22*2#%1*11##").arg(source);
			sendFrame(f);
		}
		else if (what == 11)
		{
			// got radio station
			int station = msg.whatArgN(2);

			sl[DIM_RADIO_STATION] = station;
		}
	}
	else if (where == 3)
	{
		if (msg.whereArgCnt() != 2)
			return;
		// int environment = atoi(msg.whereArg(0).c_str());
		int amplifier = atoi(msg.whereArg(1).c_str());

		if (what == 12)
		{
			// got on/off state
			int state = msg.whatArgN(0);

			// if we got an "on" state, wait for the volume to emit the
			// status_changed notification
			if (state == 0)
			{
				sl[DIM_AMPLIFIER] = amplifier;
				sl[DIM_STATUS] = false;
			}
		}
		else if (what == 1)
		{
			// got the volume
			int volume = msg.whatArgN(0);

			sl[DIM_AMPLIFIER] = amplifier;
			sl[DIM_STATUS] = true;
			sl[DIM_VOLUME] = volume;
		}
	}

	if (sl.count() > 0)
		emit status_changed(sl);
}
