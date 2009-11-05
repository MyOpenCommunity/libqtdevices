#include "test_alarmsounddiff_device.h"
#include "alarmsounddiff_device.h"
#include "openserver_mock.h"
#include "openclient.h"

#include <QtTest/QtTest>


void TestAlarmSoundDiffDevice::initTestCase()
{
	dev = new AlarmSoundDiffDevice();
}

void TestAlarmSoundDiffDevice::cleanupTestCase()
{
	delete dev;
	dev = NULL;
}

void TestAlarmSoundDiffDevice::sendSetRadioStation()
{
	dev->setRadioStation(107, 33);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#22*2#7*#6*33##"));
}

void TestAlarmSoundDiffDevice::sendActivateSource()
{
	dev->activateSource(107);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*35#4#0#7*3#1#0##"));
}

void TestAlarmSoundDiffDevice::sendActivateEnvironment()
{
	dev->activateEnvironment(5, 107);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*35#4#5#7*3*1*5*0##"));
}

void TestAlarmSoundDiffDevice::sendSetVolume()
{
	dev->setVolume(5, 57, 22);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#22*3#5#57*#1*22##"));
}

void TestAlarmSoundDiffDevice::sendAmplifierOn()
{
	dev->amplifierOn(5, 57);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*1#4#5*3#5#57##"));
}

void TestAlarmSoundDiffDevice::sendAmplifierOff()
{
	dev->amplifierOff(5, 57);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*0#4#5*3#5#57##"));
}
