#include "alarmsounddiff_device.h"
#include "hardware_functions.h" // AMPLI_NUM


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
	for (int i = 0; i < 10; ++i)
		environments[i] = false;

	for (int amplifier = 0; amplifier < AMPLI_NUM; ++amplifier)
	{
		if (alarmVolumes[amplifier] < 0)
			continue;

		int environment = amplifier / 10;
		// TODO check why original code only sent the frame for some environments
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
