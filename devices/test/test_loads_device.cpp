#include "test_loads_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "loads_device.h"

#include <openmsg.h>

#include <QtTest/QtTest>


void TestLoadsDevice::initTestCase()
{
	dev = new LoadsDevice("75#3");
}

void TestLoadsDevice::cleanupTestCase()
{
	delete dev;
	dev = NULL;
}

void TestLoadsDevice::sendForceOn()
{
	dev->forceOn();
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*18*74*%1##").arg(dev->where));
}

void TestLoadsDevice::sendForceOff()
{
	dev->forceOff(150);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*18*73#15*%1##").arg(dev->where));
}

void TestLoadsDevice::sendRequestCurrent()
{
	dev->requestCurrent();
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#18*%1*113##").arg(dev->where));
}

void TestLoadsDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#18*%1*71##").arg(dev->where));
}

void TestLoadsDevice::sendRequestTotal()
{
	dev->requestTotal(0);
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#18*%1*72#1##").arg(dev->where));
}

void TestLoadsDevice::sendRequestLevel()
{
	dev->requestLevel();
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#18*%1*73##").arg(dev->where));
}

void TestLoadsDevice::sendResetCommand()
{
	dev->resetTotal(1);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*18*75#2*%1##").arg(dev->where));
}

void TestLoadsDevice::receiveStatus()
{
	DeviceTester tst_enabled(dev, LoadsDevice::DIM_ENABLED, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tst_forced(dev, LoadsDevice::DIM_FORCED, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tst_threshold(dev, LoadsDevice::DIM_ABOVE_THRESHOLD, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tst_protection(dev, LoadsDevice::DIM_PROTECTION, DeviceTester::MULTIPLE_VALUES);

	tst_enabled.check(QString("*#18*%1*71*0*0*0*0*0##").arg(dev->where), true);
	tst_enabled.check(QString("*#18*%1*71*1*0*0*0*0##").arg(dev->where), false);

	tst_forced.check(QString("*#18*%1*71*0*0*0*0*0##").arg(dev->where), false);
	tst_forced.check(QString("*#18*%1*71*0*1*0*0*0##").arg(dev->where), true);

	tst_threshold.check(QString("*#18*%1*71*0*0*0*0*0##").arg(dev->where), true);
	tst_threshold.check(QString("*#18*%1*71*0*0*1*0*0##").arg(dev->where), false);

	tst_protection.check(QString("*#18*%1*71*0*0*0*0*0##").arg(dev->where), false);
	tst_protection.check(QString("*#18*%1*71*0*0*0*1*0##").arg(dev->where), true);
}

void TestLoadsDevice::receiveCurrent()
{
	DeviceTester t(dev, LoadsDevice::DIM_CURRENT);

	t.check(QString("*#18*%1*113*74##").arg(dev->where), 74);
}

void TestLoadsDevice::receiveLoad()
{
	DeviceTester t(dev, LoadsDevice::DIM_LOAD);

	t.check(QString("*#18*%1*73*4##").arg(dev->where), 4);
}

void TestLoadsDevice::receiveTotals()
{
	DeviceTester tst_period(dev, LoadsDevice::DIM_PERIOD, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tst_total(dev, LoadsDevice::DIM_TOTAL, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tst_date(dev, LoadsDevice::DIM_RESET_DATE, DeviceTester::MULTIPLE_VALUES);

	tst_period.check(QString("*#18*%1*72#2*1432*1*10*2009*17*12##").arg(dev->where), 1);
	tst_total.check(QString("*#18*%1*72#2*1432*1*10*2009*17*12##").arg(dev->where), 1432);
	tst_date.check(QString("*#18*%1*72#2*1432*1*10*2009*17*12##").arg(dev->where), QDateTime(QDate(2009, 10, 1), QTime(17, 12, 0)));
}
