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


#include "test_automation_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "automation_device.h"

#include <openclient.h>

#include <QtTest/QtTest>
#include <QString>
#include <QVariant>




void TestAutomationDevice::initTestCase()
{
	dev = new AutomationDevice("1000");
}

void TestAutomationDevice::cleanupTestCase()
{
	delete dev;
}

void TestAutomationDevice::sendGoUp()
{
	dev->goUp();
	client_command->flush();
	QString res = QString("*2*1*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), res);
}

void TestAutomationDevice::sendGoDown()
{
	dev->goDown();
	client_command->flush();
	QString res = QString("*2*2*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), res);
}

void TestAutomationDevice::sendStop()
{
	dev->stop();
	client_command->flush();
	QString res = QString("*2*0*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), res);
}

void TestAutomationDevice::receiveDown()
{
	DeviceTester t(dev, AutomationDevice::DIM_DOWN);
	QString frame = QString("*2*2*%1##").arg(dev->where);
	t.check(frame, true);
}

void TestAutomationDevice::receiveUp()
{
	DeviceTester t(dev, AutomationDevice::DIM_UP);
	QString frame = QString("*2*1*%1##").arg(dev->where);
	t.check(frame, true);
}

void TestAutomationDevice::receiveStop()
{
	DeviceTester t(dev, AutomationDevice::DIM_STOP);
	QString frame = QString("*2*0*%1##").arg(dev->where);
	t.check(frame, true);
}


void TestPPTStatDevice::initTestCase()
{
	where = "10";
	dev = new PPTStatDevice(where);
}

void TestPPTStatDevice::cleanupTestCase()
{
	delete dev;
}

void TestPPTStatDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QString req(QString("*#25*%1##").arg(where));
	QCOMPARE(server->frameRequest(), req);
}

void TestPPTStatDevice::receiveStatus()
{
	DeviceTester t(dev, PPTStatDevice::DIM_STATUS);
	t.check(QString("*25*31#0*%1##").arg(where), true);
	t.check(QString("*25*31#1*%1##").arg(where), true);
	t.check(QString("*25*32#0*%1##").arg(where), false);
	t.check(QString("*25*32#1*%1##").arg(where), false);
}

