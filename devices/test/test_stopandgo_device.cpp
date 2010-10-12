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


#define WHERE "666"


// Stop and go device
void TestStopAndGoDevice::init()
{
	dev = new StopAndGoDevice(WHERE);
}

void TestStopAndGoDevice::cleanup()
{
	delete dev;
}

void TestStopAndGoDevice::sendAutoResetActivation()
{
	dev->sendAutoResetActivation();
	flushCompressedFrames(dev);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*26*%1##").arg(WHERE));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*%1*250##").arg(WHERE));
}

void TestStopAndGoDevice::sendAutoResetDisactivation()
{
	dev->sendAutoResetDisactivation();
	flushCompressedFrames(dev);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*27*%1##").arg(WHERE));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*%1*250##").arg(WHERE));
}

void TestStopAndGoDevice::requestICMState()
{
	dev->requestICMState();
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#18*%1*250##").arg(WHERE));
}

void TestStopAndGoDevice::receiveICMState()
{
	DeviceValues values;
	QVERIFY(values.isEmpty());

	OpenMsg false_request(QString("*#18*%1*250*0000000000000##").arg(WHERE).toStdString());

	bool managed = dev->parseFrame(false_request, values);

	QVERIFY(managed);

	for (int i = 0; i < 13; i++)
		QCOMPARE(values[i].toBool(), false);

	values.clear();

	QVERIFY(values.isEmpty());

	OpenMsg true_request(QString("*#18*%1*250*1111111111111##").arg(WHERE).toStdString());

	managed = dev->parseFrame(true_request, values);

	QVERIFY(managed);

	for (int i = 0; i < 13; i++)
		QVERIFY(values[i].toBool());
}

void TestStopAndGoDevice::testSingleState()
{
	for (int i = 0; i < 13; i++)
	{
		DeviceValues values;
		QVERIFY(values.isEmpty());

		int val = 0b1000000000000 >> (13 - i - 1);

		OpenMsg request(QString("*#18*%1*250*%2##").arg(WHERE).arg(val, 13, 2, QChar('0')).toStdString());

		bool managed = dev->parseFrame(request, values);
		QVERIFY(managed);

		// Check a DIM a time if it's correctly setted.
		for (int j = 0; j < 13; j++)
		{
			if (j == i)
				QVERIFY(values[j].toBool());
			else
				QVERIFY(!values[j].toBool());
		}
	}
}

// Stop and go plus device
void TestStopAndGoPlusDevice::init()
{
	dev = new StopAndGoPlusDevice(WHERE);
	plus = qobject_cast<StopAndGoPlusDevice *>(dev);
}

void TestStopAndGoPlusDevice::cleanup()
{
	delete dev;
}

void TestStopAndGoPlusDevice::sendClose()
{
	plus->sendClose();
	flushCompressedFrames(dev);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*21*%1##").arg(WHERE));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*%1*250##").arg(WHERE));
}

void TestStopAndGoPlusDevice::sendOpen()
{
	plus->sendOpen();
	flushCompressedFrames(dev);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*22*%1##").arg(WHERE));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*%1*250##").arg(WHERE));
}

void TestStopAndGoPlusDevice::sendTrackingSystemActivation()
{
	plus->sendTrackingSystemActivation();
	flushCompressedFrames(dev);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*28*%1##").arg(WHERE));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*%1*250##").arg(WHERE));
}

void TestStopAndGoPlusDevice::sendTrackingSystemDisactivation()
{
	plus->sendTrackingSystemDisactivation();
	flushCompressedFrames(dev);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*29*%1##").arg(WHERE));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*%1*250##").arg(WHERE));
}


// Stop and go btest device
void TestStopAndGoBTestDevice::init()
{
	dev = new StopAndGoBTestDevice(WHERE);
	btest = qobject_cast<StopAndGoBTestDevice *>(dev);
}

void TestStopAndGoBTestDevice::cleanup()
{
	delete dev;
}

void TestStopAndGoBTestDevice::sendDiffSelftestActivation()
{
	btest->sendDiffSelftestActivation();
	flushCompressedFrames(dev);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*23*%1##").arg(WHERE));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*%1*250##").arg(WHERE));
}

void TestStopAndGoBTestDevice::sendDiffSelftestDisactivation()
{
	btest->sendDiffSelftestDisactivation();
	flushCompressedFrames(dev);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*18*24*%1##").arg(WHERE));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*%1*250##").arg(WHERE));
}

void TestStopAndGoBTestDevice::sendSelftestFreq(int days)
{
	btest->sendSelftestFreq(10);

	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#18*%1*#212*10##").arg(WHERE));

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*%1*212##").arg(WHERE));
}

void TestStopAndGoBTestDevice::requestSelftestFreq()
{
	btest->requestSelftestFreq();
	client_request->flush();

	QCOMPARE(server->frameRequest(), QString("*#18*%1*212##").arg(WHERE));
}

void TestStopAndGoBTestDevice::receiveSelftestFreq()
{
	DeviceValues values;
	QVERIFY(values.isEmpty());

	OpenMsg request(QString("*#18*%1*#212*10##").arg(WHERE).toStdString());

	bool managed = btest->parseFrame(request, values);

	QVERIFY(managed);
	QCOMPARE(values[StopAndGoBTestDevice::DIM_AUTOTEST_FREQ].toInt(), 10);
}
