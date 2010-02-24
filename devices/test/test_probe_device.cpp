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


#include "test_probe_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "probe_device.h"

#include <openmsg.h>

#include <QtTest/QtTest>

#define ZERO_FILL QLatin1Char('0')


// TestNonControlledProbeDevice implementation

void TestNonControlledProbeDevice::initTestCase()
{
	dev = new NonControlledProbeDevice("11", NonControlledProbeDevice::INTERNAL);
}

void TestNonControlledProbeDevice::cleanupTestCase()
{
	delete dev;
	dev = NULL;
}

void TestNonControlledProbeDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*11*0##"));
}

void TestNonControlledProbeDevice::receiveTemperature()
{
	DeviceTester tst(dev, NonControlledProbeDevice::DIM_TEMPERATURE);

	tst.check("*#4*11*0*123##", 123);
}


// TestExternalProbeDevice implementation

void TestExternalProbeDevice::initTestCase()
{
	dev = new NonControlledProbeDevice("11", NonControlledProbeDevice::EXTERNAL);
}

void TestExternalProbeDevice::cleanupTestCase()
{
	delete dev;
	dev = NULL;
}

void TestExternalProbeDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*11*15#1##"));
}

void TestExternalProbeDevice::receiveTemperature()
{
	DeviceTester tst(dev, NonControlledProbeDevice::DIM_TEMPERATURE);

	tst.check("*#4*11*15*1*123*1111##", 123);
}


// TestExternalProbeDevice implementation

void TestControlledProbeDevice::initTestCase()
{
	dev = new ControlledProbeDevice("23#1", "1", "23", ControlledProbeDevice::CENTRAL_99ZONES, ControlledProbeDevice::FANCOIL);
}

void TestControlledProbeDevice::cleanupTestCase()
{
	delete dev;
	dev = NULL;
}

void TestControlledProbeDevice::sendSetManual()
{
	dev->setManual(250);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*#23#1*#14*0250*3##"));
}

void TestControlledProbeDevice::sendSetAutomatic()
{
	dev->setAutomatic();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*4*311*#23#1##"));
}

void TestControlledProbeDevice::sendSetFancoilSpeed()
{
	dev->setFancoilSpeed(3);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*23*#11*3##"));
}

void TestControlledProbeDevice::sendRequestFancoilStatus()
{
	dev->requestFancoilStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*23*11##"));
}

void TestControlledProbeDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*#23#1##" "*#4*23##" "*#4*23*11##"));
}

void TestControlledProbeDevice::receiveFancoilStatus()
{
	DeviceTester tst(dev, ControlledProbeDevice::DIM_FANCOIL_STATUS, DeviceTester::MULTIPLE_VALUES);

	tst.check("*#4*23*11*2##", 2);
}

void TestControlledProbeDevice::receiveTemperature()
{
	DeviceTester tst(dev, ControlledProbeDevice::DIM_TEMPERATURE, DeviceTester::MULTIPLE_VALUES);

	tst.check("*#4*23*0*0220##", 220);
}

void TestControlledProbeDevice::receiveManual()
{
	DeviceTester tst(dev, ControlledProbeDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*4*310*23##");

	tst.check<int>(frame, ControlledProbeDevice::ST_MANUAL);
}

void TestControlledProbeDevice::receiveAuto()
{
	DeviceTester tst(dev, ControlledProbeDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*4*311*23##");

	tst.check<int>(frame, ControlledProbeDevice::ST_AUTO);
}

void TestControlledProbeDevice::receiveAntifreeze()
{
	DeviceTester tst(dev, ControlledProbeDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*4*302*23##");

	tst.check<int>(frame, ControlledProbeDevice::ST_PROTECTION);
}

void TestControlledProbeDevice::receiveOff()
{
	DeviceTester tst(dev, ControlledProbeDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*4*303*23##");

	tst.check<int>(frame, ControlledProbeDevice::ST_OFF);
}

void TestControlledProbeDevice::receiveSetPoint()
{
	DeviceTester tst(dev, ControlledProbeDevice::DIM_SETPOINT, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#4*23*14*0220*3##");

	tst.check(frame, 220);
}

void TestControlledProbeDevice::receiveLocalOffset()
{
	DeviceTester tso(dev, ControlledProbeDevice::DIM_OFFSET, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tst(dev, ControlledProbeDevice::DIM_LOCAL_STATUS, DeviceTester::MULTIPLE_VALUES);

	tso.check("*#4*23*13*00##", 0);
	tso.check("*#4*23*13*03##", 3);
	tso.check("*#4*23*13*12##", -2);

	tst.check<int>("*#4*23*13*01##", ControlledProbeDevice::ST_NORMAL);
}

void TestControlledProbeDevice::receiveLocalOff()
{
	DeviceTester tst(dev, ControlledProbeDevice::DIM_LOCAL_STATUS, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#4*23*13*4##");

	tst.check<int>(frame, ControlledProbeDevice::ST_OFF);
}

void TestControlledProbeDevice::receiveLocalAntifreeze()
{
	DeviceTester tst(dev, ControlledProbeDevice::DIM_LOCAL_STATUS, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#4*23*13*5##");

	tst.check<int>(frame, ControlledProbeDevice::ST_PROTECTION);
}

void TestControlledProbeDevice::receiveSetPointAdjusted()
{
	DeviceTester tso(dev, ControlledProbeDevice::DIM_OFFSET, DeviceTester::MULTIPLE_VALUES);

	tso.check("*#4*23*13*03##", 3);

	DeviceTester tst(dev, ControlledProbeDevice::DIM_SETPOINT, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#4*23*12*0250*3##");

	tst.check(frame, 220);
}
