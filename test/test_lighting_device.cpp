#include "test_lighting_device.h"
#include "openserver_mock.h"
#include "openclient.h"

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

void buildGeneral(const QString &global_addr, LightAddress &addr)
{
	QString frame = QString("*1*1*%1##").arg(global_addr);
	OpenMsg msg(frame.toLatin1().constData());
	LightAddress la(msg);
	addr = la;
}

void TestLightingDevice::testLightAddrGeneral()
{
	QString global = "0";
	LightAddress addr;
	buildGeneral(global, addr);

	QCOMPARE(addr.isReceiver(where), true);
}

void TestLightingDevice::testLightAddrGeneral2()
{
	// if extended, there's a different outcome for the test
	bool ext = (where.contains("#") == true);
	QString global = QString("0") + LIGHT_ADDR_EXTENSION;
	LightAddress addr;
	buildGeneral(global, addr);

	QCOMPARE(addr.isReceiver(where), ext ? true : false);
}

void TestLightingDevice::testLightAddrGeneral3()
{
	QString global = QString("0") + LIGHT_ADDR_EXTENSION_2;
	LightAddress addr;
	buildGeneral(global, addr);

	QCOMPARE(addr.isReceiver(where), false);
}
