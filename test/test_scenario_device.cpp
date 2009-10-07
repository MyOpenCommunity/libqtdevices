#include "test_scenario_device.h"
#include "openserver_mock.h"

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
