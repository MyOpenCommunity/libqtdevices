#include "test_poweramplifier_device.h"
#include "device_tester.h"
#include "openserver_mock.h"
#include "openclient.h"

#include <poweramplifier_device.h>

#include <QtTest/QtTest>
#include <QVariant>


void TestPowerAmplifierDevice::initTestCase()
{
	where = "35";
	dev = new PowerAmplifierDevice(where);
}

void TestPowerAmplifierDevice::cleanupTestCase()
{
	delete dev;
}

void TestPowerAmplifierDevice::requestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QString req(QString("*#22*3#%1#%2*12##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameRequest() == req);
}

void TestPowerAmplifierDevice::readStatus()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_STATUS);
	t.check(QString("*#22*3#%1#%2*12*1*0##").arg(where[0]).arg(where[1]), 1);
}

