#include "test_lighting_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "generic_functions.h"

#include <openmsg.h>

#include <QtTest/QtTest>

void TestLightingDevice::initLightingDevice()
{
	dev = new LightingDevice(LIGHT_DEVICE_WHERE, LightingDevice::PULL);
}

void TestLightingDevice::initTestCase()
{
	initLightingDevice();
}

void TestLightingDevice::cleanupLightingDevice()
{
	delete dev;
}

void TestLightingDevice::cleanupTestCase()
{
	cleanupLightingDevice();
}

void TestLightingDevice::init()
{
	cleanBuffers();
}

void TestLightingDevice::sendTurnOn()
{
	dev->turnOn();
	client_command->flush();
	QString cmd = QString("*1*1*") + dev->where + "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestLightingDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QString req = QString("*#1*") + dev->where + "##";
	QCOMPARE(server->frameRequest(), req);
}

void TestLightingDevice::sendFixedTiming()
{
	dev->fixedTiming(11);
	client_command->flush();
	QString cmd = QString("*1*%1*%2##").arg(11).arg(dev->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestLightingDevice::sendVariableTiming()
{
	dev->variableTiming(100, 22, 50);
	client_command->flush();
	QString cmd = QString("*#1*%1*#2*%2*%3*%4##").arg(dev->where).arg(100).arg(22).arg(50);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestLightingDevice::sendRequestVariableTiming()
{
	dev->requestVariableTiming();
	client_request->flush();
	QString req = QString("*#1*%1*2##").arg(dev->where);
	QCOMPARE(server->frameRequest(), req);
}

void TestLightingDevice::receiveLightOnOff()
{
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	t.check(QString("*1*1*%1##").arg(dev->where), true);
	t.check(QString("*1*0*%1##").arg(dev->where), false);
}

void TestLightingDevice::setParams(QString w, LightingDevice::PullMode m)
{
	dev->where = w;
	dev->mode = m;
}

// test device mode
void TestLightingDevice::checkPullUnknown()
{
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1*0##";
	QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);

	t.checkSignals(global_on, 0);
	t.checkSignals(env_off, 0);
}

void TestLightingDevice::receiveLightOnOffPull()
{
	setParams(LIGHT_DEVICE_WHERE, LightingDevice::PULL);
	checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknown()
{
	setParams(LIGHT_DEVICE_WHERE, LightingDevice::PULL_UNKNOWN);
	checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknown2()
{
	setParams(LIGHT_DEVICE_WHERE, LightingDevice::PULL_UNKNOWN);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1*0##";
	t.checkSignals(global_on, 0);
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#1*%1##").arg(dev->where));
}


void TestLightingDevice::receiveLightOnOffPullExt()
{
	setParams(LIGHT_DEVICE_WHERE + LIGHT_ADDR_EXTENSION, LightingDevice::PULL);
	checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknownExt()
{
	setParams(LIGHT_DEVICE_WHERE + LIGHT_ADDR_EXTENSION, LightingDevice::PULL_UNKNOWN);
	checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknownExt2()
{
	setParams(LIGHT_DEVICE_WHERE + LIGHT_ADDR_EXTENSION, LightingDevice::PULL_UNKNOWN);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);
	t.checkSignals(env_off, 0);
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#1*%1##").arg(dev->where));
}

void TestLightingDevice::receiveLightOnOffNotPull()
{
	setParams(LIGHT_DEVICE_WHERE, LightingDevice::NOT_PULL);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1*0##";
	QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);

	t.check(global_on, true);
	t.check(env_off, false);
}

void TestLightingDevice::receiveLightOnOffNotPullExt()
{
	setParams(LIGHT_DEVICE_WHERE + LIGHT_ADDR_EXTENSION, LightingDevice::NOT_PULL);
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	QString global_on = "*1*1*0##";
	QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);

	t.check(global_on, true);
	t.check(env_off, false);
}

void TestLightingDevice::receiveVariableTiming()
{
	DeviceTester t(dev, LightingDevice::DIM_VARIABLE_TIMING);
	QList<int> l;
	l << 1 << 3 << 55;
	QString timing = QString("*#1*%1*2*%2*%3*%4##").arg(dev->where)
		.arg(1).arg(3).arg(55);

	t.check(timing, l);
}


void TestDimmer::initTestCase()
{
	initDimmer();
}

void TestDimmer::cleanupTestCase()
{
	cleanupDimmer();
}

void TestDimmer::initDimmer()
{
	initLightingDevice();
	dimmer = new Dimmer(LIGHT_DEVICE_WHERE, LightingDevice::PULL);
}

void TestDimmer::cleanupDimmer()
{
	cleanupLightingDevice();
	delete dimmer;
}

void TestDimmer::sendDimmerDecreaseLevel()
{
	dimmer->decreaseLevel();
	client_command->flush();
	QString cmd = QString("*1*31*%1##").arg(dimmer->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestDimmer::sendDimmerIncreaseLevel()
{
	dimmer->increaseLevel();
	client_command->flush();
	QString cmd = QString("*1*30*%1##").arg(dimmer->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestDimmer::checkLevel()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL);
	QString frame = QString("*1*%1*%2##").arg(9).arg(dimmer->where);
	t.check(frame, 90);
}

void TestDimmer::receiveDimmerLevel()
{
	checkLevel();
}

void TestDimmer::receiveDimmerProblem()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_PROBLEM);
	QString frame = QString("*1*%1*%2##").arg(19).arg(dimmer->where);
	t.check(frame, true);
}



void TestDimmer100::initTestCase()
{
	initDimmer();
	dimmer100 = new Dimmer100(LIGHT_DEVICE_WHERE, LightingDevice::PULL);
}

void TestDimmer100::cleanupTestCase()
{
	cleanupDimmer();
	delete dimmer100;
}

void TestDimmer100::sendDimmer100DecreaseLevel()
{
	dimmer100->decreaseLevel100(50, 10);
	client_command->flush();
	QString cmd = QString("*1*31#%1#%2*%3##").arg(50).arg(10).arg(dimmer100->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestDimmer100::sendDimmer100IncreaseLevel()
{
	dimmer100->increaseLevel100(10, 150);
	client_command->flush();
	QString cmd = QString("*1*30#%1#%2*%3##").arg(10).arg(150).arg(dimmer100->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestDimmer100::sendRequestDimmer100Status()
{
	dimmer100->requestDimmer100Status();
	client_request->flush();
	QString req = QString("*#1*%1*1##").arg(dimmer100->where);
	QCOMPARE(server->frameRequest(), req);
}

void TestDimmer100::receiveDimmer100Status()
{
	DeviceTester t(dimmer100, LightingDevice::DIM_DIMMER100_STATUS);
	QList<int> l;
	l << 134 << 50;
	QString frame = QString("*#1*%1*1*%2*%3##").arg(dimmer100->where).arg(134).arg(50);

	t.check(frame, l);
}

void TestDimmer100::checkLevel()
{
	DeviceTester t(dimmer100, LightingDevice::DIM_DIMMER_LEVEL);
	QString frame = QString("*1*%1*%2##").arg(9).arg(dimmer100->where);
	t.check(frame, 75);
	frame = QString("*1*%1*%2##").arg(5).arg(dimmer100->where);
	t.check(frame, 30);
}
