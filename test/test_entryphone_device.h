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
	void sendEndCall();
	void sendInitVctProcess();

	void receiveIncomingCall();

private:
	// init frame must be sent at constuction time
	void testVctInitialization();
	void simulateIncomingCall(int kind, int mmtype);
	EntryphoneDevice *dev;
};

#endif // TEST_ENTRYPHONE_DEVICE_H
