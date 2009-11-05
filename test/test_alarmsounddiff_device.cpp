#include "test_alarmsounddiff_device.h"
#include "alarmsounddiff_device.h"
#include "openserver_mock.h"
#include "openclient.h"
#include "device_tester.h"

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

void TestAlarmSoundDiffDevice::receiveStatusOn()
{
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_STATUS);
	QString frame = QString("*#22*3#5#57*12*1*22##");

	tss.checkSignals(frame, 0);
}

void TestAlarmSoundDiffDevice::receiveVolume()
{
	DeviceTester tsa(dev, AlarmSoundDiffDevice::DIM_AMPLIFIER, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tsv(dev, AlarmSoundDiffDevice::DIM_VOLUME, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#22*3#5#57*1*13##");

	tsa.check(frame, 57);
	tss.check(frame, true);
	tsv.check(frame, 13);
}

void TestAlarmSoundDiffDevice::receiveStatusOff()
{
	DeviceTester tsa(dev, AlarmSoundDiffDevice::DIM_AMPLIFIER, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#22*3#5#57*12*0*22##");

	tsa.check(frame, 57);
	tss.check(frame, false);
}

void TestAlarmSoundDiffDevice::receiveSource()
{
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_SOURCE);
	QString frame = QString("*#22*2#4#5*5#2#57##");

	tss.check(frame, 57);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#22*2#57*11##"));
}

void TestAlarmSoundDiffDevice::receiveRadioStation()
{
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_RADIO_STATION);
	QString frame = QString("*#22*2#57*11*22*33*7##");

	tss.check(frame, 7);
}
