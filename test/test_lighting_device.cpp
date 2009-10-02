#include "test_lighting_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "generic_functions.h"

#include <openmsg.h>

#include <QtTest/QtTest>

TestLightingDevice::TestLightingDevice()
{
}

void TestLightingDevice::initTestCase()
{
	dev = new LightingDevice(LIGHT_DEVICE_WHERE, LightingDevice::PULL);
	dimmer = new Dimmer(LIGHT_DEVICE_WHERE, LightingDevice::PULL);
}

void TestLightingDevice::cleanupTestCase()
{
	delete dev;
	delete dimmer;
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
	// TODO: to be done, based on variableTiming() parameters
}

void TestLightingDevice::sendRequestVariableTiming()
{
	dev->requestVariableTiming();
	client_request->flush();
	QString req = QString("*#1*%1*2##").arg(dev->where);
	QCOMPARE(server->frameRequest(), req);
}

void TestLightingDevice::sendDimmerDecreaseLevel()
{
	dimmer->decreaseLevel();
	client_command->flush();
	QString cmd = QString("*1*31*%1##").arg(dimmer->where);
	QCOMPARE(server->frameCommand(), cmd);
}

void TestLightingDevice::sendDimmerIncreaseLevel()
{
	dimmer->increaseLevel();
	client_command->flush();
	QString cmd = QString("*1*30*%1##").arg(dimmer->where);
	QCOMPARE(server->frameCommand(), cmd);
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

void TestLightingDevice::receiveDimmerLevel()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_LEVEL);
	QString frame = QString("*1*%1*%2##").arg(9).arg(dimmer->where);
	t.check(frame, 9);
}

void TestLightingDevice::receiveDimmerProblem()
{
	DeviceTester t(dimmer, LightingDevice::DIM_DIMMER_PROBLEM);
	QString frame = QString("*1*%1*%2##").arg(19).arg(dimmer->where);
	t.check(frame, true);
}

void TestLightingDevice::testCheckAddressGlobal()
{
	QString global = "0";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), true);
	QCOMPARE(checkAddressIsForMe(global, "01"), true);

}

void TestLightingDevice::testCheckAddressGlobalLevel3()
{
	QString global = "0#3";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), false);
	QCOMPARE(checkAddressIsForMe(global, "01"), true);
}

void TestLightingDevice::testCheckAddressGlobalInterface()
{
	QString global = "0#4#12";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), true);
	QCOMPARE(checkAddressIsForMe(global, "01"), false);
}

void TestLightingDevice::testCheckAddressGlobalInterfaceFalse()
{
	QString global = "0#4#01";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), false);
	QCOMPARE(checkAddressIsForMe(global, "01"), false);
}

void TestLightingDevice::testCheckAddressEnvironment10()
{
	QString environment = "100";
	QCOMPARE(checkAddressIsForMe(environment, "1015#4#12"), true);
	QCOMPARE(checkAddressIsForMe(environment, "1001"), true);
}

void TestLightingDevice::testCheckAddressEnvironment0()
{
	QString environment = "00#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "01#4#12"), true);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#12"), true);
	QCOMPARE(checkAddressIsForMe(environment, "09"), false);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#99"), false);
}

void TestLightingDevice::testCheckAddressEnvironmentInt()
{
	QString environment = "3#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "0313#4#12"), true);
}

void TestLightingDevice::testCheckAddressEnvironmentLevel3()
{
	QString environment = "3#3";
	QCOMPARE(checkAddressIsForMe(environment, "0313"), true);
	environment = "00#3";
	QCOMPARE(checkAddressIsForMe(environment, "01"), true);
	environment = "100#3";
	QCOMPARE(checkAddressIsForMe(environment, "1014"), true);
}
