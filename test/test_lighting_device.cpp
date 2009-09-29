#include "test_lighting_device.h"
#include "openserver_mock.h"
#include "openclient.h"

#include <lighting_device.h>

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
