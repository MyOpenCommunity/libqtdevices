#ifndef TEST_POWERAMPLIFIER_DEVICE_H
#define TEST_POWERAMPLIFIER_DEVICE_H

#include "test_device.h"

#include <QString>

class PowerAmplifierDevice;
class OpenServerMock;


class TestPowerAmplifierDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();
	void sendTurnOn();
	void sendTurnOff();
	void sendVolumeUp();
	void sendVolumeDown();
	void sendTrebleUp();
	void sendTrebleDown();
	void sendBassUp();
	void sendBassDown();

	void receiveStatus();
	void receiveVolume();
	void receiveLoud();

private:
	PowerAmplifierDevice *dev;
	QString where;
};

#endif // TEST_POWERAMPLIFIER_DEVICE_H
