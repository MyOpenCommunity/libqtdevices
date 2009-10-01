#include "test_dev_automation.h"
#include "device_tester.h"
#include "openserver_mock.h"
#include "openclient.h"

#include <dev_automation.h>
#include <openmsg.h>

#include <QtTest/QtTest>
#include <QStringList>
#include <QVariant>


void TestPPTStatDevice::initTestCase()
{
	where = "10";
	dev = new PPTStatDevice(where);
}

void TestPPTStatDevice::cleanupTestCase()
{
	delete dev;
}

void TestPPTStatDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QString req(QString("*#25*%1##").arg(where));
	QCOMPARE(server->frameRequest(), req);
}

void TestPPTStatDevice::receiveStatus()
{
	DeviceTester t(dev, PPTStatDevice::DIM_STATUS);
	t.check(QString("*25*31#0*%1##").arg(where), true);
	t.check(QString("*25*31#1*%1##").arg(where), true);
	t.check(QString("*25*32#0*%1##").arg(where), false);
	t.check(QString("*25*32#1*%1##").arg(where), false);
}

