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

void TestNonControlledProbeDevice::testCrash()
{
	DeviceTester t(dev, NonControlledProbeDevice::DIM_TEMPERATURE); // dimension not used
	QString frame = QString("*4*0*%1##").arg(dev->where);

	t.checkSignals(frame, 0);
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

void TestExternalProbeDevice::testCrash()
{
	DeviceTester t(dev, NonControlledProbeDevice::DIM_TEMPERATURE); // dimension not used
	QString frame = QString("*4*0*%1##").arg(dev->where);

	t.checkSignals(frame, 0);
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
	flushCompressedFrames(dev);
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
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#4*23*11##"));
}

void TestControlledProbeDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	client_command->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*#23#1##" "*#4*23##"));
	QCOMPARE(server->frameCommand(), QString("*#4*23*11##"));
}

void TestControlledProbeDevice::receiveFancoilStatus()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_FANCOIL_STATUS, 2);
	t.check("*#4*23*11*2##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveTemperature()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_TEMPERATURE, 220);
	t.check("*#4*23*0*0220##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveManual()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_STATUS, (int)ControlledProbeDevice::ST_MANUAL);
	t.check("*4*310*23##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveAuto()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_STATUS, (int)ControlledProbeDevice::ST_AUTO);
	t.check("*4*311*23##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveAntifreeze()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_STATUS, (int)ControlledProbeDevice::ST_PROTECTION);
	t.check("*4*302*23##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveOff()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_STATUS, (int)ControlledProbeDevice::ST_OFF);
	t.check("*4*303*23##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveSetPoint()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_SETPOINT, 220);
	t.check("*#4*23*14*0220*3##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveLocalOffset1()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_OFFSET, 3);
	t << makePair(ControlledProbeDevice::DIM_LOCAL_STATUS, (int)ControlledProbeDevice::ST_NORMAL);
	t.check("*#4*23*13*03##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveLocalOffset2()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_OFFSET, -2);
	t << makePair(ControlledProbeDevice::DIM_LOCAL_STATUS, (int)ControlledProbeDevice::ST_NORMAL);
	t.check("*#4*23*13*12##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveLocalOff()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_LOCAL_STATUS, (int)ControlledProbeDevice::ST_OFF);
	t.check("*#4*23*13*4##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveLocalAntifreeze()
{
	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_LOCAL_STATUS, (int)ControlledProbeDevice::ST_PROTECTION);
	t.check("*#4*23*13*5##", MultiDeviceTester::CONTAINS);
}

void TestControlledProbeDevice::receiveSetPointAdjusted()
{
	OpenMsg msg("*#4*23*13*03##");
	dev->manageFrame(msg);

	MultiDeviceTester t(dev);
	t << makePair(ControlledProbeDevice::DIM_SETPOINT, 220);
	t.check("*#4*23*12*0250*3##", MultiDeviceTester::CONTAINS);
}
