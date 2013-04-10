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

#include "test_antintrusion_device.h"
#include "antintrusion_device.h"
#include "openserver_mock.h"
#include "openclient.h"
#include "device_tester.h"

#include <QtTest/QtTest>


void TestAntintrusionDevice::init()
{
	dev = new AntintrusionDevice;
}

void TestAntintrusionDevice::cleanup()
{
	delete dev;
}

void TestAntintrusionDevice::sendToggleActivation1()
{
	QString pwd = "pwd";
	dev->toggleActivation(pwd);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*5*36#%1*0##").arg(pwd));
	QCOMPARE(dev->is_inserted, true);
}

void TestAntintrusionDevice::sendToggleActivation2()
{
	QString pwd = "pwd";
	dev->partializeZone(1, true);
	QCOMPARE(dev->partialization_needed, true);
	dev->toggleActivation(pwd);
	QCOMPARE(dev->partialization_needed, false);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*5*50#%1#%2*0##").arg(pwd).arg("10000000"));
	// A frame will sent after few seconds.. but the test doesn't check it.
}

void TestAntintrusionDevice::sendToggleActivation3()
{
	QString pwd = "pwd";
	dev->is_inserted = true;
	dev->partializeZone(1, true);
	dev->toggleActivation(pwd);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*5*36#%1*0##").arg(pwd));
}

void TestAntintrusionDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#5*0##"));
}

void TestAntintrusionDevice::testSetPartialization()
{
	QString pwd = "pwd";
	dev->setPartialization(pwd);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*5*50#%1#%2*0##").arg(pwd).arg("00000000"));
	dev->partializeZone(1, true);
	QCOMPARE(dev->partialization_needed, true);
	dev->setPartialization(pwd);
	QCOMPARE(dev->partialization_needed, false);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*5*50#%1#%2*0##").arg(pwd).arg("10000000"));
	dev->partializeZone(2, true);
	dev->partializeZone(8, true);
	QCOMPARE(dev->partialization_needed, true);
	dev->setPartialization(pwd);
	QCOMPARE(dev->partialization_needed, false);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*5*50#%1#%2*0##").arg(pwd).arg("11000001"));
	dev->partializeZone(8, false);
	QCOMPARE(dev->partialization_needed, true);
	dev->setPartialization(pwd);
	QCOMPARE(dev->partialization_needed, false);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*5*50#%1#%2*0##").arg(pwd).arg("11000000"));
}

void TestAntintrusionDevice::receiveSystemInserted()
{
	DeviceTester t(dev, AntintrusionDevice::DIM_SYSTEM_INSERTED);
	t.check("*5*8*0##", true);
	t.check("*5*9*0##", false);
}

void TestAntintrusionDevice::receiveSystemMaintenance()
{
	DeviceTester t(dev, AntintrusionDevice::DIM_SYSTEM_INSERTED); // the dimension doesn't matter.
	t.checkSignals("*5*0*##", 0);
}

void TestAntintrusionDevice::receiveZonePartialized()
{
	for (int i = 0; i < NUM_ZONES; ++i)
		dev->zones[i] = false;

	DeviceTester t(dev, AntintrusionDevice::DIM_ZONE_PARTIALIZED);
	t.check("*5*18*#2##", 2);
	QCOMPARE(dev->zones[1], true);
	t.check("*5*18*#8##", 8);
	QCOMPARE(dev->zones[7], true);

	// invalid zones
	t.checkSignals("*5*18*#16##", 0);
	t.checkSignals("*5*18*#-1##", 0);
}

void TestAntintrusionDevice::receiveZoneInserted()
{
	for (int i = 0; i < NUM_ZONES; ++i)
		dev->zones[i] = true;

	DeviceTester t(dev, AntintrusionDevice::DIM_ZONE_INSERTED);
	t.check("*5*11*#1##", 1);
	QCOMPARE(dev->zones[0], false);
	t.check("*5*11*#5##", 5);
	QCOMPARE(dev->zones[4], false);
	// invalid zones
	t.checkSignals("*5*11*#16##", 0);
	t.checkSignals("*5*11*#-1##", 0);
}

void TestAntintrusionDevice::receiveIntrusionAlarm()
{
	DeviceTester t(dev, AntintrusionDevice::DIM_INTRUSION_ALARM);
	t.check("*5*15*#1##", 1);
	t.check("*5*15*#8##", 8);
	// invalid zones
	t.checkSignals("*5*15*#9##", 0);
	t.checkSignals("*5*15*#0##", 0);
}

void TestAntintrusionDevice::receiveTamperAlarm()
{
	DeviceTester t(dev, AntintrusionDevice::DIM_TAMPER_ALARM);
	t.check("*5*16*#7##", 7);
	t.check("*5*16*#12##", 12);
	// invalid zones
	t.checkSignals("*5*16*#16##", 0);
	t.checkSignals("*5*16*#-1##", 0);
}

void TestAntintrusionDevice::receiveAntipanicAlarm()
{
	DeviceTester t(dev, AntintrusionDevice::DIM_ANTIPANIC_ALARM);
	t.check("*5*17*#9##", 9);
	// invalid zones
	t.checkSignals("*5*17*#10##", 0);
	t.checkSignals("*5*17*#8##", 0);
}

void TestAntintrusionDevice::receiveTechnicalAlarm()
{
	DeviceTester t(dev, AntintrusionDevice::DIM_TECHNICAL_ALARM);
	t.check("*5*12*#1##", 1);
	t.check("*5*12*#15##", 15);
	// invalid zones
	t.checkSignals("*5*12*#0##", 0);
	t.checkSignals("*5*12*#16##", 0);
}

void TestAntintrusionDevice::receiveResetTechnicalAlarm()
{
	DeviceTester t(dev, AntintrusionDevice::DIM_RESET_TECHNICAL_ALARM);
	t.check("*5*13*#1##", 1);
	t.check("*5*13*#15##", 15);
	// invalid zones
	t.checkSignals("*5*13*#0##", 0);
	t.checkSignals("*5*13*#16##", 0);
}

