#ifndef TEST_PROBE_DEVICE_H
#define TEST_PROBE_DEVICE_H

#include "test_device.h"

class NonControlledProbeDevice;

class TestNonControlledProbeDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();

	void receiveTemperature();

private:
	NonControlledProbeDevice *dev;
};

class TestExternalProbeDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();

	void receiveTemperature();

private:
	NonControlledProbeDevice *dev;
};

#endif // TEST_PROBE_DEVICE_H
