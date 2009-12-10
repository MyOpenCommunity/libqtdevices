#ifndef TEST_ALARMSOUNDDIFF_DEVICE_H
#define TEST_ALARMSOUNDDIFF_DEVICE_H

#include "test_device.h"

class AlarmSoundDiffDevice;


class TestAlarmSoundDiffDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendSetRadioStation();
	void sendActivateSource();
	void sendActivateEnvironment();
	void sendSetVolume();
	void sendAmplifierOn();
	void sendAmplifierOff();

	void receiveStatusOn();
	void receiveVolume();
	void receiveStatusOff();
	void receiveSource();
	void receiveRadioStation();

private:
	AlarmSoundDiffDevice *dev;
};

#endif // TEST_ALARMSOUNDDIFF_DEVICE_H
