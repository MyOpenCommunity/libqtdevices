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

void TestPowerAmplifierDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QString req(QString("*#22*3#%1#%2*12##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameRequest() == req);
}

void TestPowerAmplifierDevice::receiveStatus()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_STATUS);
	t.check(QString("*#22*3#%1#%2*12*1*0##").arg(where[0]).arg(where[1]), true);
	t.check(QString("*#22*3#%1#%2*12*0*255##").arg(where[0]).arg(where[1]), false);
}

void TestPowerAmplifierDevice::receiveVolume()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_VOLUME);
	t.check(QString("*#22*3#%1#%2*1*20##").arg(where[0]).arg(where[1]), 20);
}

void TestPowerAmplifierDevice::receiveLoud()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_LOUD);
	t.check(QString("*#22*3#%1#%2*20*0##").arg(where[0]).arg(where[1]), false);
	t.check(QString("*#22*3#%1#%2*20*1##").arg(where[0]).arg(where[1]), true);
}

void TestPowerAmplifierDevice::sendTurnOn()
{
	dev->turnOn();
	client_command->flush();
	QString cmd(QString("*22*1#4#%1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendTurnOff()
{
	dev->turnOff();
	client_command->flush();
	QString cmd(QString("*22*0#4#%1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendVolumeUp()
{
	dev->volumeUp();
	client_command->flush();
	QString cmd(QString("*22*3#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendVolumeDown()
{
	dev->volumeDown();
	client_command->flush();
	QString cmd(QString("*22*4#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendTrebleUp()
{
	dev->trebleUp();
	client_command->flush();
	QString cmd(QString("*22*40#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendTrebleDown()
{
	dev->trebleDown();
	client_command->flush();
	QString cmd(QString("*22*41#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendBassUp()
{
	dev->bassUp();
	client_command->flush();
	QString cmd(QString("*22*36#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendBassDown()
{
	dev->bassDown();
	client_command->flush();
	QString cmd(QString("*22*37#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendBalanceUp()
{
	dev->balanceUp();
	client_command->flush();
	QString cmd(QString("*22*42#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendBalanceDown()
{
	dev->balanceDown();
	client_command->flush();
	QString cmd(QString("*22*43#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendNextPreset()
{
	dev->nextPreset();
	client_command->flush();
	QString cmd(QString("*22*55*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendPrevPreset()
{
	dev->prevPreset();
	client_command->flush();
	QString cmd(QString("*22*56*3#%1#%2##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendLoudOn()
{
	dev->loudOn();
	client_command->flush();
	QString cmd(QString("*#22*3#%1#%2*20*1##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

void TestPowerAmplifierDevice::sendLoudOff()
{
	dev->loudOff();
	client_command->flush();
	QString cmd(QString("*#22*3#%1#%2*20*0##").arg(where[0]).arg(where[1]));
	QVERIFY(server->frameCommand() == cmd);
}

