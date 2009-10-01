#include "test_lighting_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "generic_functions.h"

#include <openmsg.h>

#include <QtTest/QtTest>

TestLightingDevice::TestLightingDevice(QString w, LightingDevice::PullMode m)
{
	where = w;
	mode = m;
}

void TestLightingDevice::initTestCase()
{
	dev = new LightingDevice(where, mode);
}

void TestLightingDevice::cleanupTestCase()
{
	delete dev;
}

void TestLightingDevice::sendTurnOn()
{
	dev->turnOn();
	client_command->flush();
	QString cmd = QString("*1*1*") + where + "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestLightingDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QString req = QString("*#1*") + where + "##";
	QCOMPARE(server->frameRequest(), req);
}

void TestLightingDevice::receiveLightOnOff()
{
	DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
	t.check(QString("*1*1*%1##").arg(where), true);
	t.check(QString("*1*0*%1##").arg(where), false);
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
	// no extension
	if (where.indexOf("#") < 0 && mode == LightingDevice::PULL)
		checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknown()
{
	if (where.indexOf("#") < 0 && mode == LightingDevice::PULL_UNKNOWN)
		checkPullUnknown();
}


void TestLightingDevice::receiveLightOnOffPullExt()
{
	// extension
	if (where.indexOf("#") > 0 && mode == LightingDevice::PULL)
		checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffUnknownExt()
{
	// extension
	if (where.indexOf("#") > 0 && mode == LightingDevice::PULL_UNKNOWN)
		checkPullUnknown();
}

void TestLightingDevice::receiveLightOnOffNotPull()
{
	if (where.indexOf("#") < 0 && mode == LightingDevice::NOT_PULL)
	{
		DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
		QString global_on = "*1*1*0##";
		QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);

		OpenMsg msg(global_on.toStdString());
		t.check(global_on, true);

		OpenMsg msg2(env_off.toStdString());
		t.check(env_off, false);
	}
}

void TestLightingDevice::receiveLightOnOffNotPullExt()
{
	if (where.indexOf("#") > 0 && mode == LightingDevice::NOT_PULL)
	{
		DeviceTester t(dev, LightingDevice::DIM_DEVICE_ON);
		QString global_on = "*1*1*0##";
		QString env_off = QString("*1*0*3%1##").arg(LIGHT_ADDR_EXTENSION);

		OpenMsg msg(global_on.toStdString());
		t.check(global_on, true);

		OpenMsg msg2(env_off.toStdString());
		t.check(env_off, false);
	}
}

void TestLightingDevice::testCheckAddress()
{
	QString global = "0";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), true);
	QCOMPARE(checkAddressIsForMe(global, "01"), true);

	global = "0#3";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), false);
	QCOMPARE(checkAddressIsForMe(global, "01"), true);

	global = "0#4#12";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), true);
	QCOMPARE(checkAddressIsForMe(global, "01"), false);

	global = "0#4#01";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), false);
	QCOMPARE(checkAddressIsForMe(global, "01"), false);

	QString environment = "100";
	QCOMPARE(checkAddressIsForMe(environment, "1015#4#12"), true);
	QCOMPARE(checkAddressIsForMe(environment, "1001"), true);

	environment = "00#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "01#4#12"), true);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#12"), true);
	QCOMPARE(checkAddressIsForMe(environment, "09"), false);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#99"), false);

	environment = "3#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "0313#4#12"), true);
}
