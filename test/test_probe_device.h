#ifndef TEST_PROBE_DEVICE_H
#define TEST_PROBE_DEVICE_H

#include "test_device.h"

class NonControlledProbeDevice;
class ControlledProbeDevice;

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

class TestControlledProbeDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendSetManual();
	void sendSetAutomatic();
	void sendSetFancoilSpeed();
	void sendRequestFancoilStatus();
	void sendRequestStatus();

	void receiveFancoilStatus();
	void receiveTemperature();
	void receiveManual();
	void receiveAuto();
	void receiveAntifreeze();
	void receiveOff();
	void receiveSetPoint();
	void receiveLocalOffset();
	void receiveLocalOff();
	void receiveLocalAntifreeze();
	void receiveSetPointAdjusted();

private:
	ControlledProbeDevice *dev;
};

#endif // TEST_PROBE_DEVICE_H
