#ifndef TEST_LOADS_DEVICE_H
#define TEST_LOADS_DEVICE_H

#include "test_device.h"

class LoadsDevice;

class TestLoadsDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendForceOn();
	void sendForceOff();

	void sendRequestCurrent();
	void sendRequestStatus();
	void sendRequestTotal();
	void sendRequestLevel();

	void sendResetCommand();

	void receiveStatus();
	void receiveCurrent();
	void receiveLoad();
	void receiveTotals();

private:
	LoadsDevice *dev;
};

#endif // TEST_PROBE_DEVICE_H
