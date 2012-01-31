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


#include "test_scenevodevicescond.h"
#include "scenevodevicescond.h"
#include "openserver_mock.h"
#include "openclient.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "frame_classes.h" // Clients

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QHash>
#include <QPair>


/**
 * This empty class is required because every DeviceCondition use a
 * DeviceConditionDisplayInterface* to draw a graphical representation of the
 * condition. In this tests we don't care about it.
 */
class DeviceConditionDisplayMock : public DeviceConditionDisplayInterface
{
public:
	virtual void updateText(int min_condition_value, int max_condition_value)
	{
		Q_UNUSED(min_condition_value)
		Q_UNUSED(max_condition_value)
	}
};


TestScenEvoDevicesCond::TestScenEvoDevicesCond()
{
	server = new OpenServerMock;
	QHash<int, ClientReader*> monitors;
	client_monitor = server->connectMonitor();
	monitors[0] = client_monitor;
	FrameReceiver::setClientsMonitor(monitors);

	QHash<int, Clients> clients;
	client_command = clients[0].command = server->connectCommand();
	client_request = clients[0].request = server->connectRequest();
	device::setClients(clients);

	mock_display = new DeviceConditionDisplayMock;
	dev_where = "10";
}

void TestScenEvoDevicesCond::init()
{
	// We need to clear the device cache because we have to insert in the cache
	// different devices with the same key.
	bt_global::devices_cache.clear();
}

TestScenEvoDevicesCond::~TestScenEvoDevicesCond()
{
	delete server;
}

void TestScenEvoDevicesCond::checkCondition(QSignalSpy &spy, QString frame, bool satisfied)
{
	client_monitor->manageFrame(frame.toAscii());
	QCOMPARE(spy.count(), satisfied ? 1 : 0);
	spy.clear();
}

void TestScenEvoDevicesCond::testLightOn()
{
	DeviceConditionLight cond(mock_display, "1", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testLightOff()
{
	DeviceConditionLight cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testLightConditionChange1()
{
	DeviceConditionLight cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	cond.set_current_value(1);
	cond.save();

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#1*%1##").arg(dev_where));
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testLightConditionChange2()
{
	DeviceConditionLight cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
	cond.set_current_value(1);
	cond.save();

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#1*%1##").arg(dev_where));
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testLightConditionChange3()
{
	DeviceConditionLight cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	cond.save();
	client_request->flush();

	QCOMPARE(client_request->bytesAvailable(), 0);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimmingOff()
{
	DeviceConditionDimming cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimmingRange1()
{
	DeviceConditionDimming cond(mock_display, "2-4", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*2*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*4*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*3*%1##").arg(dev_where), false);
}

void TestScenEvoDevicesCond::testDimmingRange2()
{
	DeviceConditionDimming cond(mock_display, "5-7", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*6*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*7*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*6*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*8*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimmingRange3()
{
	DeviceConditionDimming cond(mock_display, "8-10", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*10*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*7*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*10*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*8*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*9*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), false);
}

void TestScenEvoDevicesCond::testDimmingConditionChange1()
{
	DeviceConditionDimming cond(mock_display, "5-7", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*7*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	cond.set_current_value_min(8);
	cond.set_current_value_max(10);
	cond.save();

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#1*%1##").arg(dev_where));
	checkCondition(spy, QString("*1*8*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*9*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*8*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimmingConditionChange2()
{
	DeviceConditionDimming cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*2*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
	cond.set_current_value_min(5);
	cond.set_current_value_max(7);
	cond.save();

	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#1*%1##").arg(dev_where));
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimmingConditionChange3()
{
	DeviceConditionDimming cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*2*%1##").arg(dev_where), false);
	cond.save();

	client_request->flush();
	QCOMPARE(client_request->bytesAvailable(), 0);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimming100Off()
{
	DeviceConditionDimming100 cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*100*0##").arg(dev_where), true);
	checkCondition(spy, QString("*#1*%1*1*101*0##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimming100Range1()
{
	DeviceConditionDimming100 cond(mock_display, "1-20", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*2*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*105*0##").arg(dev_where), true);
	checkCondition(spy, QString("*#1*%1*1*121*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*120*0##").arg(dev_where), true);
	checkCondition(spy, QString("*#1*%1*1*180*0##").arg(dev_where), false);
	checkCondition(spy, QString("*1*4*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*#1*%1*1*180*0##").arg(dev_where), false);
	checkCondition(spy, QString("*1*2*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimming100Range2()
{
	DeviceConditionDimming100 cond(mock_display, "21-40", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#1*%1*1*121*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*160*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*121*0##").arg(dev_where), true);
	checkCondition(spy, QString("*#1*%1*1*122*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*141*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*160*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*140*0##").arg(dev_where), true);
	checkCondition(spy, QString("*1*2*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*2*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*6*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimming100Range3()
{
	DeviceConditionDimming100 cond(mock_display, "41-70", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*7*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*103*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*141*0##").arg(dev_where), true);
	checkCondition(spy, QString("*#1*%1*1*103*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*170*0##").arg(dev_where), true);
	checkCondition(spy, QString("*1*6*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*7*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*9*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*8*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimming100Range4()
{
	DeviceConditionDimming100 cond(mock_display, "71-100", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*10*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*#1*%1*1*170*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*171*0##").arg(dev_where), true);
	checkCondition(spy, QString("*1*8*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*200*0##").arg(dev_where), true);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*9*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*8*%1##").arg(dev_where), false);
}

void TestScenEvoDevicesCond::testDimming100ConditionChange1()
{
	DeviceConditionDimming100 cond(mock_display, "71-100", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#1*%1*1*170*0##").arg(dev_where), false);
	cond.set_current_value_min(41);
	cond.set_current_value_max(70);
	cond.save();

	client_request->flush();
	QStringList frames = server->frameRequest().split("##", QString::SkipEmptyParts);
	QVERIFY(frames.contains(QString("*#1*%1").arg(dev_where)));
	QVERIFY(frames.contains(QString("*#1*%1*1").arg(dev_where)));

	checkCondition(spy, QString("*#1*%1*1*141*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*140*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*141*0##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimming100ConditionChange2()
{
	DeviceConditionDimming100 cond(mock_display, "21-40", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#1*%1*1*170*0##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*121*0##").arg(dev_where), true);
	cond.set_current_value_min(71);
	cond.set_current_value_max(100);
	cond.save();

	client_request->flush();
	QStringList frames = server->frameRequest().split("##", QString::SkipEmptyParts);
	QVERIFY(frames.contains(QString("*#1*%1").arg(dev_where)));
	QVERIFY(frames.contains(QString("*#1*%1*1").arg(dev_where)));

	checkCondition(spy, QString("*#1*%1*1*200*0##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimming100ConditionChange3()
{
	DeviceConditionDimming100 cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*3*%1##").arg(dev_where), false);
	cond.save();

	client_request->flush();
	QCOMPARE(client_request->bytesAvailable(), 0);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testInternalTemperature1()
{
	DeviceConditionTemperature cond(mock_display, "0000", dev_where, false);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*0*0100##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0000##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*0*0100##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0010##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testInternalTemperature2()
{
	DeviceConditionTemperature cond(mock_display, "0235", dev_where, false);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*0*0000##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0225##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*0*0224##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0245##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*0*0246##").arg(dev_where), false);
}

void TestScenEvoDevicesCond::testInternalTemperature3()
{
	DeviceConditionTemperature cond(mock_display, "1010", dev_where, false);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*0*0400##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*1010##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*0*0001##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0000##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testInternalTemperatureConditionChange1()
{
	DeviceConditionTemperature cond(mock_display, "1010", dev_where, false);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*0*0400##").arg(dev_where), false);

	cond.set_current_value(200);
	cond.save();
	checkCondition(spy, QString("*#4*%1*0*0210##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*1010##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0210##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testInternalTemperatureConditionChange2()
{
	DeviceConditionTemperature cond(mock_display, "1010", dev_where, false);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*0*0400##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0000##").arg(dev_where), true);

	cond.set_current_value(200);
	cond.save();
	checkCondition(spy, QString("*#4*%1*0*0210##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testInternalTemperatureConditionChange3()
{
	DeviceConditionTemperature cond(mock_display, "1010", dev_where, false);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*0*0400##").arg(dev_where), false);
	cond.save();
	checkCondition(spy, QString("*#4*%1*0*0000##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testInternalTemperatureMinMax()
{
	// A temperature condition should be in the range [-0.5, + 50] Celsius.

	// Check the min condition temperature
	DeviceConditionTemperature cond_out_min(mock_display, "1100", dev_where, false);
	QSignalSpy spy_min(&cond_out_min, SIGNAL(condSatisfied()));

	checkCondition(spy_min, QString("*#4*%1*0*1039##").arg(dev_where), false);
	checkCondition(spy_min, QString("*#4*%1*0*1050##").arg(dev_where), true);
	checkCondition(spy_min, QString("*#4*%1*0*1039##").arg(dev_where), false);
	checkCondition(spy_min, QString("*#4*%1*0*1040##").arg(dev_where), true);

	// Check the max condition temperature
	DeviceConditionTemperature cond_out_max(mock_display, "0700", dev_where);
	QSignalSpy spy_max(&cond_out_max, SIGNAL(condSatisfied()));

	checkCondition(spy_max, QString("*#4*%1*0*0489##").arg(dev_where), false);
	checkCondition(spy_max, QString("*#4*%1*0*0499##").arg(dev_where), true);
	checkCondition(spy_max, QString("*#4*%1*0*0489##").arg(dev_where), false);
	checkCondition(spy_max, QString("*#4*%1*0*0500##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testExternalTemperature1()
{
	DeviceConditionTemperature cond(mock_display, "0000", dev_where, true);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*15*1*0100*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0000*1111##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*15*1*0100*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0010*1111##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testExternalTemperature2()
{
	DeviceConditionTemperature cond(mock_display, "0235", dev_where, true);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*15*1*0000*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0225*1111##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*15*1*0224*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0245*1111##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*15*1*0246*1111##").arg(dev_where), false);
}

void TestScenEvoDevicesCond::testExternalTemperature3()
{
	DeviceConditionTemperature cond(mock_display, "1010", dev_where, true);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*15*1*0400*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*1010*1111##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*15*1*0001*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0000*1111##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testExternalTemperatureConditionChange1()
{
	DeviceConditionTemperature cond(mock_display, "1010", dev_where, true);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*15*1*0400*1111##").arg(dev_where), false);

	cond.set_current_value(200);
	cond.save();
	checkCondition(spy, QString("*#4*%1*15*1*0201*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0400*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0201*1111##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testExternalTemperatureConditionChange2()
{
	DeviceConditionTemperature cond(mock_display, "0235", dev_where, true);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*15*1*0000*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0225*1111##").arg(dev_where), true);

	cond.set_current_value(400);
	cond.save();
	checkCondition(spy, QString("*#4*%1*15*1*0395*1111##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testExternalTemperatureConditionChange3()
{
	DeviceConditionTemperature cond(mock_display, "0235", dev_where, true);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*15*1*0000*1111##").arg(dev_where), false);
	cond.save();
	checkCondition(spy, QString("*#4*%1*15*1*0235*1111##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testExternalTemperatureMinMax()
{
	// A temperature condition should be in the range [-0.5, + 50] Celsius.

	// Check the min condition temperature
	DeviceConditionTemperature cond_out_min(mock_display, "1100", dev_where, true);
	QSignalSpy spy_min(&cond_out_min, SIGNAL(condSatisfied()));

	checkCondition(spy_min, QString("*#4*%1*15*1*1039*1111##").arg(dev_where), false);
	checkCondition(spy_min, QString("*#4*%1*15*1*1050*1111##").arg(dev_where), true);
	checkCondition(spy_min, QString("*#4*%1*15*1*1039*1111##").arg(dev_where), false);
	checkCondition(spy_min, QString("*#4*%1*15*1*1040*1111##").arg(dev_where), true);

	// Check the max condition temperature
	DeviceConditionTemperature cond_out_max(mock_display, "0700", dev_where);
	QSignalSpy spy_max(&cond_out_max, SIGNAL(condSatisfied()));

	checkCondition(spy_max, QString("*#4*%1*15*1*0489*1111##").arg(dev_where), false);
	checkCondition(spy_max, QString("*#4*%1*15*1*0499*1111##").arg(dev_where), true);
	checkCondition(spy_max, QString("*#4*%1*15*1*0489*1111##").arg(dev_where), false);
	checkCondition(spy_max, QString("*#4*%1*15*1*0500*1111##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testAux()
{
	DeviceConditionAux cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testAuxAtStart()
{
	DeviceConditionAux cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testAuxConditionChange1()
{
	DeviceConditionAux cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), false);
	cond.set_current_value(1);
	cond.save();
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testAuxConditionChange2()
{
	DeviceConditionAux cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), true);
	cond.set_current_value(1);
	cond.save();
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testAuxConditionChange3()
{
	DeviceConditionAux cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), false);
	cond.save();
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testVolumeOn()
{
	DeviceConditionVolume cond(mock_display, "0-31", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*20##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
}

void TestScenEvoDevicesCond::testVolumeOff()
{
	DeviceConditionVolume cond(mock_display, "-1", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*0##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*0##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
}

void TestScenEvoDevicesCond::testVolumeRange1()
{
	DeviceConditionVolume cond(mock_display, "0-6", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*0##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*0##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
	checkCondition(spy, QString("*#22*3#%1#%2*1*20##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*6##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*6##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*0##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
}

void TestScenEvoDevicesCond::testVolumeRange2()
{
	DeviceConditionVolume cond(mock_display, "7-12", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*7##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*6##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*7##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
	checkCondition(spy, QString("*#22*3#%1#%2*1*7##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*20##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*12##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*12##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*12##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
}

void TestScenEvoDevicesCond::testVolumeRange3()
{
	DeviceConditionVolume cond(mock_display, "13-22", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*13##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*13##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
	checkCondition(spy, QString("*#22*3#%1#%2*1*13##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*20##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*20##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*22##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
}

void TestScenEvoDevicesCond::testVolumeRange4()
{
	DeviceConditionVolume cond(mock_display, "23-31", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*31##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*13##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*31##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
	checkCondition(spy, QString("*#22*3#%1#%2*1*25##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
}

void TestScenEvoDevicesCond::testVolumeConditionChange1()
{
	DeviceConditionVolume cond(mock_display, "23-31", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*13##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	cond.set_current_value_min(13);
	cond.set_current_value_max(22);
	cond.save();
	checkCondition(spy, QString("*#22*3#%1#%2*1*13##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*2##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*22##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
}

void TestScenEvoDevicesCond::testVolumeConditionChange2()
{
	DeviceConditionVolume cond(mock_display, "23-31", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*13##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*23##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
	cond.set_current_value_min(-1);
	cond.set_current_value_max(-1);
	cond.save();
	checkCondition(spy, QString("*#22*3#%1#%2*12*0*10##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
}

void TestScenEvoDevicesCond::testVolumeConditionChange3()
{
	DeviceConditionVolume cond(mock_display, "23-31", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*#22*3#%1#%2*12*1*4##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	checkCondition(spy, QString("*#22*3#%1#%2*1*13##").arg(dev_where.at(0)).arg(dev_where.at(1)), false);
	cond.save();
	checkCondition(spy, QString("*#22*3#%1#%2*1*23##").arg(dev_where.at(0)).arg(dev_where.at(1)), true);
}

