#include "test_automation_device.h"
#include "openserver_mock.h"
#include "device_tester.h"

#include <automation_device.h>
#include <openclient.h>

#include <QtTest/QtTest>

void TestAutomationDevice::initTestCase()
{
	dev = new AutomationDevice("1000");
}

void TestAutomationDevice::cleanupTestCase()
{
	delete dev;
}

void TestAutomationDevice::sendGoUp()
{
	dev->goUp();
	client_command->flush();
	QString res = QString("*2*1*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), res);
}

void TestAutomationDevice::sendGoDown()
{
	dev->goDown();
	client_command->flush();
	QString res = QString("*2*2*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), res);
}

void TestAutomationDevice::sendStop()
{
	dev->stop();
	client_command->flush();
	QString res = QString("*2*0*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), res);
}

void TestAutomationDevice::receiveDown()
{
	DeviceTester t(dev, AutomationDevice::DIM_DOWN);
	QString frame = QString("*2*2*%1##").arg(dev->where);
	t.check(frame, true);
}

void TestAutomationDevice::receiveUp()
{
	DeviceTester t(dev, AutomationDevice::DIM_UP);
	QString frame = QString("*2*1*%1##").arg(dev->where);
	t.check(frame, true);
}

void TestAutomationDevice::receiveStop()
{
	DeviceTester t(dev, AutomationDevice::DIM_STOP);
	QString frame = QString("*2*0*%1##").arg(dev->where);
	t.check(frame, true);
}
