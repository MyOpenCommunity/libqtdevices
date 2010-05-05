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
#include "test_stopandgo_device.h"

#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "stopandgo_device.h"
#include "openmsg.h"

#include <QtTest>


// Stop and go device
void TestStopAndGoDevice::init()
{
	dev = new StopAndGoDevice("666");
}

void TestStopAndGoDevice::cleanup()
{
	delete dev;
}

void TestStopAndGoDevice::sendAutoResetActivation()
{
	dev->sendAutoResetActivation();

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*26*666##"));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*666*250##"));
}

void TestStopAndGoDevice::sendAutoResetDisactivation()
{
	dev->sendAutoResetDisactivation();

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*27*666##"));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*666*250##"));
}

void TestStopAndGoDevice::requestICMState()
{
	dev->requestICMState();
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#18*666*250##"));
}

void TestStopAndGoDevice::receiveICMState()
{
	DeviceValues values;
	QVERIFY(values.isEmpty());

	OpenMsg request("*#18*666*250*12345##");

	bool managed = dev->parseFrame(request, values);

	QVERIFY(managed);
	QCOMPARE(values[StopAndGoDevice::DIM_ICM_STATE].toString(), QString("12345"));
}


// Stop and go plus device
void TestStopAndGoPlusDevice::init()
{
	dev = new StopAndGoPlusDevice("666");
	plus = qobject_cast<StopAndGoPlusDevice *>(dev);
}

void TestStopAndGoPlusDevice::cleanup()
{
	delete dev;
}

void TestStopAndGoPlusDevice::sendClose()
{
	plus->sendClose();

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*21*666##"));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*666*250##"));
}

void TestStopAndGoPlusDevice::sendOpen()
{
	plus->sendOpen();

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*22*666##"));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*666*250##"));
}

void TestStopAndGoPlusDevice::sendTrackingSystemActivation()
{
	plus->sendTrackingSystemActivation();

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*28*666##"));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*666*250##"));
}

void TestStopAndGoPlusDevice::sendTrackingSystemDisactivation()
{
	plus->sendTrackingSystemDisactivation();

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*29*666##"));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*666*250##"));
}


// Stop and go btest device
void TestStopAndGoBTestDevice::init()
{
	dev = new StopAndGoBTestDevice("666");
	btest = qobject_cast<StopAndGoBTestDevice *>(dev);
}

void TestStopAndGoBTestDevice::cleanup()
{
	delete dev;
}

void TestStopAndGoBTestDevice::sendDiffSelftestActivation()
{
	btest->sendDiffSelftestActivation();

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*23*666##"));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*666*250##"));
}

void TestStopAndGoBTestDevice::sendDiffSelftestDisactivation()
{
	btest->sendDiffSelftestDisactivation();

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*24*666##"));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*666*250##"));
}

void TestStopAndGoBTestDevice::sendSelftestFreq(int days)
{
	btest->sendSelftestFreq(10);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#18*666*#212*10##"));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*666*212##"));
}

void TestStopAndGoBTestDevice::requestSelftestFreq()
{
	btest->requestSelftestFreq();
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#18*666*212##"));
}

void TestStopAndGoBTestDevice::receiveSelftestFreq()
{
	DeviceValues values;
	QVERIFY(values.isEmpty());

	OpenMsg request("*#18*666*#212*10##");

	bool managed = btest->parseFrame(request, values);

	QVERIFY(managed);
	QCOMPARE(values[StopAndGoBTestDevice::DIM_AUTOTEST_FREQ].toInt(), 10);
}
