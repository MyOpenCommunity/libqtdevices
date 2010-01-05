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

void TestAirConditioningDevice::sendTurnOff()
{
	QString off_cmd = "18";
	dev->setOffCommand(off_cmd);
	dev->turnOff();
	compareCommand(off_cmd);
}


#define ADVANCED_AIR_COND_DIM 22
typedef AdvancedAirConditioningDevice::AirConditionerStatus AirConditionerStatus;

void TestAdvancedAirConditioningDevice::initTestCase()
{
	dev = new AdvancedAirConditioningDevice("45#1");
}

void TestAdvancedAirConditioningDevice::cleanupTestCase()
{
	delete dev;
}

void TestAdvancedAirConditioningDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QString frame = QString ("*#4*%1*%2##").arg(dev->where).arg(ADVANCED_AIR_COND_DIM);
	QCOMPARE(server->frameRequest(), frame);
}

void TestAdvancedAirConditioningDevice::testStatusToString()
{
	int temp = 300;
	AirConditionerStatus st(AdvancedAirConditioningDevice::MODE_OFF, temp, AdvancedAirConditioningDevice::VEL_MED,
		AdvancedAirConditioningDevice::SWING_ON);
	QString str = dev->statusToString(st);
	QString check = QString("22*0***");
	QCOMPARE(str, check);
}
