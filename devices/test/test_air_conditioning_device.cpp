/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "test_air_conditioning_device.h"
#include "openserver_mock.h"
#include "device_tester.h"

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

void TestAdvancedAirConditioningDevice::receiveSetStatusSetOk()
{
	DeviceTester test(dev, AdvancedAirConditioningDevice::DIM_SETSTATUS_ERROR);

	dev->setStatus(AdvancedAirConditioningDevice::MODE_FAN, 0, AdvancedAirConditioningDevice::VEL_MIN, AdvancedAirConditioningDevice::SWING_ON);
	client_command->flush();
	QString frame = QString("*#4*%1*#22*3**1*1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);

	test.checkSignals(QString("*#4*%1*22*3*123*1*1##").arg(dev->where), 0);
}

void TestAdvancedAirConditioningDevice::receiveSetStatusSetError()
{
	DeviceTester test(dev, AdvancedAirConditioningDevice::DIM_SETSTATUS_ERROR);

	dev->setStatus(AdvancedAirConditioningDevice::MODE_DEHUM, 123, AdvancedAirConditioningDevice::VEL_MIN, AdvancedAirConditioningDevice::SWING_ON);
	client_command->flush();
	QString frame = QString("*#4*%1*#22*4**1*1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);

	test.check(QString("*#4*%1*22*4*123*2*1##").arg(dev->where), true);

	// subsequent frames cause no error
	test.checkSignals(QString("*#4*%1*22*4*123*2*1##").arg(dev->where), 0);
}

void TestAdvancedAirConditioningDevice::receiveSetStatusNotSet()
{
	DeviceTester test(dev, AdvancedAirConditioningDevice::DIM_SETSTATUS_ERROR);

	test.checkSignals(QString("*#4*%1*22*4*123*2*1##").arg(dev->where), 0);
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

void TestAdvancedAirConditioningDevice::testStatusToString2()
{
	int temp = 50;
	AirConditionerStatus st(AdvancedAirConditioningDevice::MODE_WINTER, temp, AdvancedAirConditioningDevice::VEL_MED,
		AdvancedAirConditioningDevice::SWING_INVALID);
	QString str = dev->statusToString(st);
	QString check = "22*1*50*2*";
	QCOMPARE(str, check);
}

void TestAdvancedAirConditioningDevice::testStatusToString3()
{
	int temp = 50;
	AirConditionerStatus st(AdvancedAirConditioningDevice::MODE_WINTER, temp, AdvancedAirConditioningDevice::VEL_INVALID,
		AdvancedAirConditioningDevice::SWING_ON);
	QString str = dev->statusToString(st);
	QString check = "22*1*50**1";
	QCOMPARE(str, check);
}

void TestAdvancedAirConditioningDevice::testStatusToString4()
{
	int temp = 50;
	AirConditionerStatus st(AdvancedAirConditioningDevice::MODE_WINTER, temp, AdvancedAirConditioningDevice::VEL_INVALID,
		AdvancedAirConditioningDevice::SWING_INVALID);
	QString str = dev->statusToString(st);
	QString check = "22*1*50**";
	QCOMPARE(str, check);
}
