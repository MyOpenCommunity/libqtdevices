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
