#include "test_lighting_device.h"
#include "openserver_mock.h"
#include "openclient.h"
#include "generic_functions.h"

#include <lighting_device.h>
#include <openmsg.h>

#include <QtTest/QtTest>

TestLightingDevice::TestLightingDevice(QString w)
{
	where = w;
}

void TestLightingDevice::initTestCase()
{
	dev = new LightingDevice(where, LightingDevice::PULL);
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
}
