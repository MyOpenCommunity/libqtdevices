#ifndef TEST_LIGHTING_DEVICE_H
#define TEST_LIGHTING_DEVICE_H

#include "test_device.h"

class LightingDevice;

class TestLightingDevice : public TestDevice
{
Q_OBJECT
public:
	TestLightingDevice(QString w);

private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendTurnOn();
	void sendRequestStatus();

private:
	LightingDevice *dev;
	QString where;
};

#endif // TEST_LIGHTING_DEVICE_H
