#ifndef TEST_ENTRYPHONE_DEVICE_H
#define TEST_ENTRYPHONE_DEVICE_H

#include "test_device.h"

class EntryphoneDevice;

class TestEntryphoneDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendAnswerCall();
	void sendCallEnd();

private:
	EntryphoneDevice *dev;
};

#endif // TEST_ENTRYPHONE_DEVICE_H
