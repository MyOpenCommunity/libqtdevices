#include "test_scenario_device.h"
#include "openserver_mock.h"
#include "device_tester.h"

#include <openclient.h>
#include <scenario_device.h>

#include <QtTest/QtTest>

void TestScenarioDevice::initTestCase()
{
	dev = new ScenarioDevice("33");
}

void TestScenarioDevice::cleanupTestCase()
{
	delete dev;
}

void TestScenarioDevice::sendActivateScenario()
{
	dev->activateScenario(22);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*0*%1*%2##").arg(22).arg(dev->where));
}

void TestScenarioDevice::sendStartProgramming()
{
	dev->startProgramming(10);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*0*40#10*%1##").arg(dev->where));
}

void TestScenarioDevice::sendStopProgramming()
{
	dev->stopProgramming(1);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*0*41#1*%1##").arg(dev->where));
}

void TestScenarioDevice::sendDeleteAll()
{
	dev->deleteAll();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*0*42*%1##").arg(dev->where));
}

void TestScenarioDevice::sendDeleteScenario()
{
	dev->deleteScenario(31);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*0*42#31*%1##").arg(dev->where));
}

void TestScenarioDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#0*%1##").arg(dev->where));
}

void TestScenarioDevice::receiveStartProgramming()
{
	int scenario_number = 22;
	DeviceTester t(dev, ScenarioDevice::DIM_START);
	QString frame = QString("*0*40#%1*%2##").arg(scenario_number).arg(dev->where);
	QPair<bool, int> check_value(true, scenario_number);
	t.check(frame, check_value);
}

void TestScenarioDevice::receiveStopProgramming()
{
	int scenario_number = 22;
	DeviceTester t(dev, ScenarioDevice::DIM_START);
	QString frame = QString("*0*41#%1*%2##").arg(scenario_number).arg(dev->where);
	QPair<bool, int> check_value(false, scenario_number);
	t.check(frame, check_value);
}

void TestScenarioDevice::receiveLockDevice()
{
	DeviceTester t(dev, ScenarioDevice::DIM_LOCK);
	QString frame = QString("*0*43*%1##").arg(dev->where);
	t.check(frame, true);
}

void TestScenarioDevice::receiveUnlockDevice()
{
	DeviceTester t(dev, ScenarioDevice::DIM_LOCK);
	QString frame = QString("*0*44*%1##").arg(dev->where);
	t.check(frame, false);
}

// what = 40 frames must be processed by all devices
void TestScenarioDevice::receiveGenericModuleIsProgramming()
{
	DeviceTester t(dev, ScenarioDevice::DIM_START);
	// use a frame addressed to another device
	QString where_other = QString::number(dev->where.toInt() + 1);
	QString frame = QString("*0*40*%1##").arg(where_other);
	t.checkSignals(frame, 1);
}

void TestScenarioDevice::receiveGenericStopProgramming()
{
	DeviceTester t(dev, ScenarioDevice::DIM_START);
	QString frame = QString("*0*41*%1##").arg(dev->where);
	QPair<bool, int> check_value(false, ScenarioDevice::ALL_SCENARIOS);
	t.check(frame, check_value);
}

void TestScenarioDevice::receiveGenericStartProgramming()
{
	DeviceTester t(dev, ScenarioDevice::DIM_START);
	QString frame = QString("*0*40*%1##").arg(dev->where);
	QPair<bool, int> check_value(true, ScenarioDevice::ALL_SCENARIOS);
	t.check(frame, check_value);
}
