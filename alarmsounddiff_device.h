#ifndef ALARMSOUNDDIFF_DEVICE_H
#define ALARMSOUNDDIFF_DEVICE_H

#include "device.h"


class AlarmSoundDiffDevice : public device
{
friend class TestAlarmSoundDiffDevice;
Q_OBJECT
public:
	AlarmSoundDiffDevice();

	void startAlarm(int source, int radio_station, int *alarmVolumes);
	void stopAlarm(int source, int *alarmVolumes);

private:
	void setRadioStation(int source, int radio_station);
	// TODO check function name
	void activateEnvironment(int environment, int source);
	void activateSource(int source);
	void setVolume(int environment, int amplifier, int volume);
	void amplifierOn(int environment, int amplifier);
	void amplifierOff(int environment, int amplifier);
};

#endif // ALARMSOUNDDIFF_DEVICE_H
