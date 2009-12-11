#include "test_entryphone_device.h"
#include "openserver_mock.h"
#include "device_tester.h"

#include <openclient.h>
#include <openmsg.h>
#include <entryphone_device.h>

#include <QtTest/QtTest>

void TestEntryphoneDevice::initTestCase()
{
	dev = new EntryphoneDevice("11");
}

void TestEntryphoneDevice::cleanupTestCase()
{
	delete dev;
}

void TestEntryphoneDevice::simulateIncomingCall(int kind, int mmtype)
{
	QString incoming_call = QString("*8*1#%1#%2#21*%3##").arg(kind).arg(mmtype).arg(dev->where);
	// I HATE YOU OpenMsg!
	OpenMsg msg(incoming_call.toStdString());
	dev->manageFrame(msg);
}

void TestEntryphoneDevice::sendAnswerCall()
{
	// ringtone 1
	int kind = 1;
	// audio and video call
	int mmtype = 4;
	// simulate an incoming call, since we must remember kind and mmtype
	simulateIncomingCall(kind, mmtype);

	dev->answerCall();
	client_command->flush();
	// answers require device address as 'where'
	QCOMPARE(server->frameCommand(), QString("*8*2#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where));
}

void TestEntryphoneDevice::sendEndCall()
{
	// ringtone 1
	int kind = 1;
	// audio and video call
	int mmtype = 4;
	simulateIncomingCall(kind, mmtype);

	dev->endCall();
	client_command->flush();
	QString frame = QString("*8*3#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestEntryphoneDevice::receiveIncomingCall()
{
	DeviceTester t(dev, EntryphoneDevice::INCOMING_CALL);
	QString frame = QString("*8*1#%1#%2#21*%3##").arg(1).arg(4).arg(dev->where);
	t.check(frame, true);
}
