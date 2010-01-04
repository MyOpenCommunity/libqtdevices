#ifndef TEST_ENTRYPHONE_DEVICE_H
#define TEST_ENTRYPHONE_DEVICE_H

#include "test_device.h"

class EntryphoneDevice;
class QString;


class TestEntryphoneDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();
	void init();

	void sendAnswerCall();
	void sendEndCall();
	void sendInitVctProcess();
	void sendCameraOn();
	void sendStairLightActivate();
	void sendStairLightRelease();
	void sendOpenLock();
	void sendReleaseLock();
	void sendCycleExternalUnits();

	void receiveIncomingCall();
	void receiveCallerAddress();

private:
	// init frame must be sent at constuction time
	void simulateIncomingCall(int kind, int mmtype);
	void simulateCallerAddress(int kind, int mmtype, QString where);

	EntryphoneDevice *dev;
};

#endif // TEST_ENTRYPHONE_DEVICE_H
