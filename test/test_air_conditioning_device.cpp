#include "test_air_conditioning_device.h"
#include "openserver_mock.h"

#include <airconditioning_device.h>
#include <openclient.h>

#include <QtTest/QtTest>

void TestAirConditioningDevice::initTestCase()
{
	dev = new AirConditioningDevice("11");
}

void TestAirConditioningDevice::cleanupTestCase()
{
	delete dev;
}

void TestAirConditioningDevice::sendActivateScenario()
{
	QString what = "18";
	dev->activateScenario(what);
	compareCommand(what);
}

void TestAirConditioningDevice::sendSendCommand()
{
	QString what = "00";
	dev->sendCommand(what);
	compareCommand(what);
}

void TestAirConditioningDevice::compareCommand(const QString &what)
{
	client_command->flush();
	QString frame = QString("*0*%1*%2##").arg(what).arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestAirConditioningDevice::sendSendOff()
{
	QString off_cmd = "90";
	dev->setOffCommand(off_cmd);
	dev->sendOff();
	compareCommand(off_cmd);
}

void TestAirConditioningDevice::sendTurnOff()
{
	QString off_cmd = "18";
	dev->setOffCommand(off_cmd);
	dev->sendOff();
	compareCommand(off_cmd);
}
