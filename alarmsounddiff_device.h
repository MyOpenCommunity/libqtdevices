#ifndef ALARMSOUNDDIFF_DEVICE_H
#define ALARMSOUNDDIFF_DEVICE_H

#include "device.h"


class AlarmSoundDiffDevice : public device
{
friend class TestAlarmSoundDiffDevice;
Q_OBJECT
public:

	enum Type
	{
		DIM_AMPLIFIER = 1,
		DIM_STATUS = 2,
		DIM_VOLUME = 3,
		DIM_SOURCE = 4,
		DIM_RADIO_STATION = 5
	};

	AlarmSoundDiffDevice();

	void startAlarm(int source, int radio_station, int *alarmVolumes);
	void stopAlarm(int source, int *alarmVolumes);

	void setReceiveFrames(bool receive);

protected:
	virtual void manageFrame(OpenMsg &msg);

private:
	void setRadioStation(int source, int radio_station);
	// TODO check function name
	void activateEnvironment(int environment, int source);
	void activateSource(int source);
	void setVolume(int environment, int amplifier, int volume);
	void amplifierOn(int environment, int amplifier);
	void amplifierOff(int environment, int amplifier);

private:
	bool receive_frames;
};

#endif // ALARMSOUNDDIFF_DEVICE_H
