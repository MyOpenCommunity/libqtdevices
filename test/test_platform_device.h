#ifndef TEST_LANDEVICE_H
#define TEST_LANDEVICE_H

#include "test_device.h"

class PlatformDevice;


class TestPlatformDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void receiveStatus();
	void receiveIp();
	void receiveNetmask();
	void receiveMacAddress();
	void receiveGateway();
	void receiveDns1();
	void receiveDns2();
	void receiveFirmwareVersion();
	void receiveKernelVersion();

	void sendSetDate();
	void sendSetTime();

private:
	PlatformDevice *dev;
};

#endif // TEST_LANDEVICE_H
