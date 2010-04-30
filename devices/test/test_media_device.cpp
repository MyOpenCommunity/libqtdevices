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


#include "test_media_device.h"
#include "openclient.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openmsg.h"

#include <media_device.h>

#include <QtTest>

#define AMPLI_AREA "3"
#define AMPLI_POINT "1"


TestSourceDevice::TestSourceDevice()
{
	source_id = "1";
}

void TestSourceDevice::initTestCase()
{
	initSource();
}

void TestSourceDevice::cleanupTestCase()
{
	delete dev;
}

void TestSourceDevice::initSource(SourceDevice *d)
{
	dev = d ? d : new SourceDevice(source_id);
}

void TestSourceDevice::sendNextTrack()
{
	dev->nextTrack();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*9*2#%1##").arg(source_id));
}

void TestSourceDevice::sendPrevTrack()
{
	dev->prevTrack();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*10*2#%1##").arg(source_id));
}

void TestSourceDevice::sendTurnOn()
{
	QString area = "2";
	dev->turnOn(area);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*35#4#%2#%1*3#%2#0##").arg(source_id).arg(area));
}

void TestSourceDevice::sendRequestTrack()
{
	dev->requestTrack();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#22*2#%1*6##").arg(source_id));
}

void TestSourceDevice::sendRequestActiveAreas()
{
	dev->requestActiveAreas();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#22*2#%1*13##").arg(source_id));
}

void TestSourceDevice::receiveStatus()
{
	DeviceTester t(dev, SourceDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	t.check(QString("*#22*2#%1*12*1*4##").arg(source_id), true);
	t.check(QString("*#22*2#%1*12*0*4##").arg(source_id), false);
}

void TestSourceDevice::receiveTrack()
{
	DeviceTester t(dev, SourceDevice::DIM_TRACK);
	t.check(QString("*#22*2#%1*6*3##").arg(source_id), 3);
}

void TestSourceDevice::testActiveAreas()
{
	QString area = "5";
	QCOMPARE(dev->isActive(area), false);
	QCOMPARE(dev->active_areas.count(), 0);

	OpenMsg frame_on(qPrintable(QString("*22*2#4#%1*5#2#%2##").arg(area).arg(source_id)));
	dev->manageFrame(frame_on);
	QCOMPARE(dev->isActive(area), true);
	QCOMPARE(dev->active_areas.count(), 1);

	OpenMsg frame_areas(qPrintable(QString("*#22*5#2#%1*13*1*0*0*0*0*0*0*0*0*0*0*0*0*0*0*1##").arg(source_id)));
	dev->manageFrame(frame_areas);
	QCOMPARE(dev->active_areas.count(), 2);
	QCOMPARE(dev->isActive(area), false);
	QCOMPARE(dev->isActive("0"), true);
	QCOMPARE(dev->isActive("15"), true);

	OpenMsg frame_off(qPrintable(QString("*#22*2#%1*12*0*4##").arg(source_id)));
	dev->manageFrame(frame_off);
	QCOMPARE(dev->isActive(area), false);
}


void TestRadioSourceDevice::initTestCase()
{
	initRadioSource();
}

void TestRadioSourceDevice::initRadioSource(RadioSourceDevice *d)
{
	if (d)
		dev = d;
	else
	{
		dev = new RadioSourceDevice(source_id);
		initSource(dev);
	}
}

void TestRadioSourceDevice::cleanupTestCase()
{
	delete dev;
}

void TestRadioSourceDevice::sendFrequenceUp()
{
	dev->frequenceUp();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*5#*2#%1##").arg(source_id));
}

void TestRadioSourceDevice::sendFrequenceDown()
{
	dev->frequenceDown();
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*6#*2#%1##").arg(source_id));
}

void TestRadioSourceDevice::sendSaveStation()
{
	dev->saveStation("6");
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*33#6*2#%1##").arg(source_id));
}

void TestRadioSourceDevice::sendRequestFrequency()
{
	dev->requestFrequency();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#22*2#%1*5##").arg(source_id));
}

void TestRadioSourceDevice::receiveFrequency()
{
	DeviceTester t(dev, RadioSourceDevice::DIM_FREQUENCY);
	t.check(QString("*#22*2#%1*5*1*100##").arg(source_id), 100);
	t.check(QString("*#22*2#%1*5*1*30##").arg(source_id), 30);
}

void TestRadioSourceDevice::receiveRDS()
{
	DeviceTester t(dev, RadioSourceDevice::DIM_RDS);
	t.check(QString("*#22*2#%1*10*104*101*108*108*111*33##").arg(source_id), "hello!");
}

void TestRadioSourceDevice::receiveStopRDS()
{
	OpenMsg frame(qPrintable(QString("*22*32*2#%1##").arg(source_id)));
	dev->manageFrame(frame);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*31*2#%1##").arg(source_id));
}


void TestVirtualSourceDevice::initTestCase()
{
	initVirtualSource();
}

void TestVirtualSourceDevice::initVirtualSource(VirtualSourceDevice *d)
{
	if (d)
		dev = d;
	else
	{
		dev = new VirtualSourceDevice(source_id);
		initSource(dev);
	}
}

void TestVirtualSourceDevice::cleanupTestCase()
{
	delete dev;
}

void TestVirtualSourceDevice::receiveNextTrack()
{
	DeviceTester t(dev, VirtualSourceDevice::REQ_NEXT_TRACK);
	t.check(QString("*22*9*2#%1##").arg(source_id), true);
}

void TestVirtualSourceDevice::receivePrevTrack()
{
	DeviceTester t(dev, VirtualSourceDevice::REQ_PREV_TRACK);
	t.check(QString("*22*10*2#%1##").arg(source_id), true);
}

void TestVirtualSourceDevice::receiveSourceOn()
{
	QString area = "5";
	DeviceTester t(dev, VirtualSourceDevice::REQ_SOURCE_ON);
	t.check(QString("*22*2#4#%1*5#2#%2##").arg(area).arg(source_id), area);
}

void TestVirtualSourceDevice::receiveSourceOff()
{
	DeviceTester t(dev, VirtualSourceDevice::REQ_SOURCE_OFF, DeviceTester::MULTIPLE_VALUES);
	t.check(QString("*#22*2#%1*12*0*4##").arg(source_id), true);
}


TestAmplifierDevice::TestAmplifierDevice()
{
	area = AMPLI_AREA;
	point = AMPLI_POINT;
}

void TestAmplifierDevice::initTestCase()
{
	initAmplifier();
}

void TestAmplifierDevice::initAmplifier(AmplifierDevice *d)
{
	dev = d ? d : new AmplifierDevice(area, point);
}

void TestAmplifierDevice::cleanupTestCase()
{
	delete dev;
}

void TestAmplifierDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QString req(QString("*#22*3#%1#%2*12##").arg(area).arg(point));
	QCOMPARE(server->frameRequest(), req);
}

void TestAmplifierDevice::sendTurnOn()
{
	dev->turnOn();
	client_command->flush();
	QString cmd(QString("*22*34#4#%1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestAmplifierDevice::sendTurnOff()
{
	dev->turnOff();
	client_command->flush();
	QString cmd(QString("*22*0#4#%1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestAmplifierDevice::sendVolumeUp()
{
	dev->volumeUp();
	client_command->flush();
	QString cmd(QString("*22*3#1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestAmplifierDevice::sendVolumeDown()
{
	dev->volumeDown();
	client_command->flush();
	QString cmd(QString("*22*4#1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestAmplifierDevice::receiveStatus()
{
	DeviceTester t(dev, AmplifierDevice::DIM_STATUS);
	t.check(QString("*#22*3#%1#%2*12*1*0##").arg(area).arg(point), true);
	t.check(QString("*#22*3#%1#%2*12*0*255##").arg(area).arg(point), false);
}

void TestAmplifierDevice::receiveVolume()
{
	DeviceTester t(dev, AmplifierDevice::DIM_VOLUME);
	t.check(QString("*#22*3#%1#%2*1*20##").arg(area).arg(point), 20);
	t.checkSignals(QString("*#22*3#%1#%2*1##").arg(area).arg(point), 0);
}

void TestAmplifierDevice::receiveStatusRequest()
{
	DeviceTester t(dev, AmplifierDevice::DIM_VOLUME); // the dim doesn't matter
	t.checkSignals(QString("*#22*3#%1#%2##").arg(area).arg(point), 0);
}



TestVirtualAmplifierDevice::TestVirtualAmplifierDevice()
{
	where = QString(AMPLI_AREA) + QString(AMPLI_POINT);
}

void TestVirtualAmplifierDevice::initTestCase()
{
	initVirtualAmplifier();
}

void TestVirtualAmplifierDevice::initVirtualAmplifier(VirtualAmplifierDevice *d)
{
	if (d)
		dev = d;
	else
	{
		dev = new VirtualAmplifierDevice(where);
		initAmplifier(dev);
	}
}

void TestVirtualAmplifierDevice::cleanupTestCase()
{
	delete dev;
}

void TestVirtualAmplifierDevice::sendUpdateVolume()
{
	const int NEW_VOL = 25;
	dev->updateVolume(NEW_VOL);
	client_command->flush();
	QString cmd(QString("*#22*3#%1#%2*1*%3##").arg(where[0]).arg(where[1]).arg(NEW_VOL));
	QCOMPARE(server->frameCommand(), cmd);
}




TestPowerAmplifierDevice::TestPowerAmplifierDevice()
{
	where = QString(AMPLI_AREA) + QString(AMPLI_POINT);
}

void TestPowerAmplifierDevice::initTestCase()
{
	initPowerAmplifier();
}

void TestPowerAmplifierDevice::initPowerAmplifier(PowerAmplifierDevice *d)
{
	if (d)
		dev = d;
	else
	{
		dev = new PowerAmplifierDevice(where);
		initAmplifier(dev);
	}
}

void TestPowerAmplifierDevice::cleanupTestCase()
{
	delete dev;
}

void TestPowerAmplifierDevice::sendTrebleUp()
{
	dev->trebleUp();
	client_command->flush();
	QString cmd(QString("*22*40#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendTrebleDown()
{
	dev->trebleDown();
	client_command->flush();
	QString cmd(QString("*22*41#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendBassUp()
{
	dev->bassUp();
	client_command->flush();
	QString cmd(QString("*22*36#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendBassDown()
{
	dev->bassDown();
	client_command->flush();
	QString cmd(QString("*22*37#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendBalanceUp()
{
	dev->balanceUp();
	client_command->flush();
	QString cmd(QString("*22*42#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendBalanceDown()
{
	dev->balanceDown();
	client_command->flush();
	QString cmd(QString("*22*43#1*3#%1#%2##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendNextPreset()
{
	dev->nextPreset();
	client_command->flush();
	QString cmd(QString("*22*55*3#%1#%2##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendPrevPreset()
{
	dev->prevPreset();
	client_command->flush();
	QString cmd(QString("*22*56*3#%1#%2##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendLoudOn()
{
	dev->loudOn();
	client_command->flush();
	QString cmd(QString("*#22*3#%1#%2*#20*1##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendLoudOff()
{
	dev->loudOff();
	client_command->flush();
	QString cmd(QString("*#22*3#%1#%2*#20*0##").arg(where[0]).arg(where[1]));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::receiveLoud()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_LOUD);
	t.check(QString("*#22*3#%1#%2*20*0##").arg(where[0]).arg(where[1]), false);
	t.check(QString("*#22*3#%1#%2*20*1##").arg(where[0]).arg(where[1]), true);
}

void TestPowerAmplifierDevice::receivePreset()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_PRESET);
	t.check(QString("*#22*3#%1#%2*19*2##").arg(where[0]).arg(where[1]), 0);
	t.check(QString("*#22*3#%1#%2*19*6##").arg(where[0]).arg(where[1]), 4);
	t.check(QString("*#22*3#%1#%2*19*11##").arg(where[0]).arg(where[1]), 9);
	t.checkSignals(QString("*#22*3#%1#%2*19*12##").arg(where[0]).arg(where[1]), 0);
	t.checkSignals(QString("*#22*3#%1#%2*19*15##").arg(where[0]).arg(where[1]), 0);
	t.check(QString("*#22*3#%1#%2*19*16##").arg(where[0]).arg(where[1]), 10);
	t.check(QString("*#22*3#%1#%2*19*25##").arg(where[0]).arg(where[1]), 19);
}

void TestPowerAmplifierDevice::receiveTreble()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_TREBLE);
	t.check(QString("*#22*3#%1#%2*2*0##").arg(where[0]).arg(where[1]), -10);
	t.check(QString("*#22*3#%1#%2*2*3##").arg(where[0]).arg(where[1]), -9);
	t.check(QString("*#22*3#%1#%2*2*4##").arg(where[0]).arg(where[1]), -9);
	t.check(QString("*#22*3#%1#%2*2*30##").arg(where[0]).arg(where[1]), 0);
	t.check(QString("*#22*3#%1#%2*2*60##").arg(where[0]).arg(where[1]), 10);
}

void TestPowerAmplifierDevice::receiveBass()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_BASS);
	t.check(QString("*#22*3#%1#%2*4*0##").arg(where[0]).arg(where[1]), -10);
	t.check(QString("*#22*3#%1#%2*4*3##").arg(where[0]).arg(where[1]), -9);
	t.check(QString("*#22*3#%1#%2*4*4##").arg(where[0]).arg(where[1]), -9);
	t.check(QString("*#22*3#%1#%2*4*30##").arg(where[0]).arg(where[1]), 0);
	t.check(QString("*#22*3#%1#%2*4*60##").arg(where[0]).arg(where[1]), 10);
	t.checkSignals(QString("*22*4#1*3#%1#%2##").arg(where[0]).arg(where[1]), 0);
}

void TestPowerAmplifierDevice::receiveBalance()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_BALANCE);
	t.check(QString("*#22*3#%1#%2*17*00##").arg(where[0]).arg(where[1]), 0);
	t.check(QString("*#22*3#%1#%2*17*03##").arg(where[0]).arg(where[1]), -1);
	t.check(QString("*#22*3#%1#%2*17*04##").arg(where[0]).arg(where[1]), -1);
	t.check(QString("*#22*3#%1#%2*17*030##").arg(where[0]).arg(where[1]), -10);
	t.check(QString("*#22*3#%1#%2*17*10##").arg(where[0]).arg(where[1]), 0);
	t.check(QString("*#22*3#%1#%2*17*11##").arg(where[0]).arg(where[1]), 0);
	t.check(QString("*#22*3#%1#%2*17*13##").arg(where[0]).arg(where[1]), 1);
	t.check(QString("*#22*3#%1#%2*17*115##").arg(where[0]).arg(where[1]), 5);
}

