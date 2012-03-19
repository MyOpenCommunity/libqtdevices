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
#include "hardware_functions.h" // AMPLI_NUM

#include <media_device.h>

#include <QtTest>

#define AMPLI_AREA "3"
#define AMPLI_POINT "1"
#define AMPLI_WRONG_AREA "4"
#define AMPLI_WRONG_POINT "2"


void TestAlarmSoundDiffDevice::initTestCase()
{
	// clear the source/amplifier list created by earlier tests
	AlarmSoundDiffDevice::sources.clear();
	AlarmSoundDiffDevice::amplifiers.clear();
	dev = new AlarmSoundDiffDevice();
	dev->setReceiveFrames(true);
}

void TestAlarmSoundDiffDevice::cleanupTestCase()
{
	delete dev;
	AlarmSoundDiffDevice::alarm_device = NULL; // shouldn't be necessary, but does not hurt
	SourceDevice::setIsMultichannel(false);
}

void TestAlarmSoundDiffDevice::init()
{
	AlarmSoundDiffDevice::sources.clear();
	AlarmSoundDiffDevice::amplifiers.clear();
}

void TestAlarmSoundDiffDevice::testStartAlarm()
{
	int alarmVolumes[AMPLI_NUM];
	for (int i = 0; i < AMPLI_NUM; ++i)
		alarmVolumes[i] = -1;

	alarmVolumes[20] = 3;
	alarmVolumes[6] = 5;

	// Create the devices required by the AlarmSoundDiffDevice.
	RadioSourceDevice radio("7");
	AmplifierDevice ampl1("20");
	AmplifierDevice ampl2("06");

	dev->startAlarm(true, 7, 33, alarmVolumes);
	client_command->flush();

	QStringList frames = server->frameCommand().split("##", QString::SkipEmptyParts);
	QVERIFY(frames.contains("*22*35#4#0#7*3#0#0")); // turn on the source with id 7
	QVERIFY(frames.contains("*#22*2#7*#6*33")); // set the radio station

	// First amplifier with address 20
	QVERIFY(frames.contains("*22*35#4#2#7*3#2#0")); // turn on the source in the area
	QVERIFY(frames.contains("*#22*3#2#0*#1*3")); // set the volume
	QVERIFY(frames.contains("*22*34#4#2*3#2#0")); // turn on the amplifier

	// Second amplifier with address 6
	QVERIFY(frames.contains("*#22*3#0#6*#1*5")); // set the volume
	QVERIFY(frames.contains("*22*34#4#0*3#0#6")); // turn on the amplifier
	QVERIFY(frames.count() == 7);
}

void TestAlarmSoundDiffDevice::testStopAlarm()
{
	int alarmVolumes[AMPLI_NUM];
	for (int i = 0; i < AMPLI_NUM; ++i)
		alarmVolumes[i] = -1;

	SourceDevice source("7");
	AmplifierDevice ampl1("20");
	AmplifierDevice ampl2("06");
	alarmVolumes[20] = 3;
	alarmVolumes[6] = 5;
	dev->stopAlarm(7, alarmVolumes);
	client_command->flush();

	QStringList frames = server->frameCommand().split("##", QString::SkipEmptyParts);
	QVERIFY(frames.contains("*22*0#4#2*3#2#0")); // amplifier with address 20
	QVERIFY(frames.contains("*22*0#4#0*3#0#6")); // amplifier with address 6
}

void TestAlarmSoundDiffDevice::sendSetVolume()
{
	AmplifierDevice amplifier("57");
	dev->setVolume(57, 22);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#22*3#5#7*#1*22##"));
}

void TestAlarmSoundDiffDevice::receiveStatus()
{
	AmplifierDevice amplifier("57");
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_STATUS);
	tss.addReceiver(&amplifier);
	tss.checkSignals("*#22*3#5#7*12*1*22##", 0);
	tss.checkSignals("*#22*3#5#7*12*0*22##", 1);
}

void TestAlarmSoundDiffDevice::receiveVolume()
{
	AmplifierDevice amplifier("57");
	MultiDeviceTester t(dev);
	t.addReceiver(&amplifier);
	t << makePair(AlarmSoundDiffDevice::DIM_AMPLIFIER, 57);
	t << makePair(AlarmSoundDiffDevice::DIM_STATUS, true);
	t << makePair(AlarmSoundDiffDevice::DIM_VOLUME, 13);
	t.check("*#22*3#5#7*1*13##");
}

void TestAlarmSoundDiffDevice::receiveStatusOff()
{
	AmplifierDevice amplifier("57");
	MultiDeviceTester t(dev);
	t.addReceiver(&amplifier);
	t << makePair(AlarmSoundDiffDevice::DIM_AMPLIFIER, 57);
	t << makePair(AlarmSoundDiffDevice::DIM_STATUS, false);
	t.check("*#22*3#5#7*12*0*22##");
}

void TestAlarmSoundDiffDevice::receiveSource()
{
	SourceDevice source("21");
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_SOURCE);
	tss.addReceiver(&source);

	tss.check("*#22*2#21*12*1*4##", 21);
}

void TestAlarmSoundDiffDevice::receiveRadioStation()
{
	RadioSourceDevice source("21");
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_RADIO_STATION);
	tss.addReceiver(&source);

	tss.check("*#22*5#2#21*11*22*33*7##", 7);
	tss.check("*#22*2#21*6*4##", 4);
}

void TestAlarmSoundDiffDevice::receiveAreaUpdate()
{
	SourceDevice::setIsMultichannel(true);

	SourceDevice source("21");
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_SOURCE);
	tss.addReceiver(&source);

	dev->setActiveArea("2");
	tss.check("*22*2#4#2*5#2#21##", 21);

	dev->setActiveArea("3");
	tss.checkSignals("*22*2#4#2*5#2#21##", 0);
}

void TestAlarmSoundDiffDevice::setCurrentArea()
{
	SourceDevice::setIsMultichannel(true);

	SourceDevice source("21");
	DeviceTester tss(dev, AlarmSoundDiffDevice::DIM_SOURCE);
	tss.addReceiver(&source);

	dev->setActiveArea("3");
	tss.checkSignals("*22*2#4#2*5#2#21##", 0);

	dev->setActiveArea("2");
	tss.check(21);
}


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

void TestSourceDevice::sendGeneralTurnOn()
{
	dev->turnOn(QString());
	client_command->flush();
	QStringList l;
	for (int area = 1; area <= 8; ++area)
		l << QString("*22*35#4#%2#%1*3#%2#0##").arg(source_id).arg(area);
	QCOMPARE(server->frameCommand(), l.join(""));
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

void TestSourceDevice::receiveStatus1()
{
	DeviceTester t(dev, SourceDevice::DIM_STATUS);
	t.check(QString("*#22*2#%1*12*1*4##").arg(source_id), true);
}

void TestSourceDevice::receiveStatus2()
{
	MultiDeviceTester t(dev);
	t << makePair(SourceDevice::DIM_STATUS, false);
	t << makePair(SourceDevice::DIM_AREAS_UPDATED, true);
	t.check(QString("*#22*2#%1*12*0*4##").arg(source_id), MultiDeviceTester::CONTAINS);
}

void TestSourceDevice::receiveTrack()
{
	DeviceTester t(dev, SourceDevice::DIM_TRACK);
	t.check(QString("*#22*2#%1*6*3##").arg(source_id), 3);
}

void TestSourceDevice::testActiveAreas()
{
	SourceDevice::setIsMultichannel(true);
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

void TestSourceDevice::testActiveAreas2()
{
	SourceDevice::setIsMultichannel(true);
	QString other_id = "2";
	SourceDevice *other = new SourceDevice(other_id);

	QString area = "5", area2 = "6";
	QCOMPARE(dev->isActive(area), false);
	QCOMPARE(other->isActive(area2), false);

	OpenMsg frame_on(qPrintable(QString("*22*2#4#%1*5#2#%2##").arg(area).arg(source_id)));
	dev->manageFrame(frame_on);

	OpenMsg frame_on2(qPrintable(QString("*22*2#4#%1*5#2#%2##").arg(area2).arg(other_id)));
	other->manageFrame(frame_on2);

	QCOMPARE(dev->isActive(area), true);
	QCOMPARE(other->isActive(area2), true);

	// turning on a source in area 6 turns off active sources in that area
	OpenMsg frame_areas(qPrintable(QString("*#22*5#2#%1*13*0*0*0*0*0*0*1*0*0*0*0*0*0*0*0*1##").arg(source_id)));
	dev->manageFrame(frame_areas);
	other->manageFrame(frame_areas);
	QCOMPARE(dev->isActive(area), false);
	QCOMPARE(dev->isActive(area2), true);
	QCOMPARE(dev->isActive("15"), true);
	QCOMPARE(other->isActive(area2), false);

	// turning on a source in area 6 turns off active sources in that area
	OpenMsg frame_on3(qPrintable(QString("*22*2#4#%1*5#2#%2##").arg(area2).arg(other_id)));
	dev->manageFrame(frame_on3);
	other->manageFrame(frame_on3);
	QCOMPARE(dev->isActive(area), false);
	QCOMPARE(dev->isActive(area2), false);
	QCOMPARE(dev->isActive("15"), true);
	QCOMPARE(other->isActive(area2), true);
}

void TestSourceDevice::testMonochannelArea()
{
	dev->active_areas.clear();
	SourceDevice::setIsMultichannel(false);
	QString monochannel_area = "0";
	QCOMPARE(dev->isActive(monochannel_area), false);

	OpenMsg frame_on(qPrintable(QString("*22*2#4#%1*5#2#%2##").arg("5").arg(source_id)));
	dev->manageFrame(frame_on);
	QCOMPARE(dev->isActive(monochannel_area), true);
	QCOMPARE(dev->active_areas.count(), 1);
}

void TestSourceDevice::testCrash()
{
	OpenMsg frame_request_freq(qPrintable(QString("*#22*2#%1*5##").arg(source_id)));
	dev->manageFrame(frame_request_freq);

	OpenMsg frame_request_track(qPrintable(QString("*#22*2#%1*6##").arg(source_id)));
	dev->manageFrame(frame_request_track);

	OpenMsg frame_request_status(qPrintable(QString("*#22*2#%1##").arg(source_id)));
	dev->manageFrame(frame_request_status);

	OpenMsg frame_request_high(qPrintable(QString("*#22*3#1#8*2##")));
	dev->manageFrame(frame_request_high);
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

void TestRadioSourceDevice::sendSetStation()
{
	dev->setStation("7");
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#22*2#%1*#6*7##").arg(source_id));
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
	dev->rds_updates = true;
	dev->manageFrame(frame);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*22*31*2#%1##").arg(source_id));
}

void TestRadioSourceDevice::receiveMemorizedStation()
{
	MultiDeviceTester t(dev);
	t << makePair(RadioSourceDevice::DIM_FREQUENCY, 9800);
	t << makePair(RadioSourceDevice::DIM_TRACK, 5);
	t.check(QString("*#22*5#2#%1*11*1*9800*5##").arg(source_id));
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

void TestVirtualSourceDevice::sendNextTrack()
{
	// only emits a signal
}

void TestVirtualSourceDevice::sendPrevTrack()
{
	// only emits a signal
}

void TestVirtualSourceDevice::receiveNextTrack()
{
	DeviceTester t(dev, VirtualSourceDevice::REQ_NEXT_TRACK);

	// normal point-to-point frame
	t.check(QString("*22*9*2#%1##").arg(source_id), true);

	// general frame from amplifier, monochannel
	dev->active_areas = QSet<QString>() << "0";
	t.check(QString("*22*9*5#3#2#3##"), true);
	t.check(QString("*22*9*5#3#7#2##"), true);

	// general frame from amplifier, multichannel, same area
	dev->active_areas = QSet<QString>() << "2" << "4";
	t.check(QString("*22*9*5#3#2#3##"), true);
	t.check(QString("*22*9*5#3#4#1##"), true);

	// general frame from amplifier, multichannel, different area
	dev->active_areas = QSet<QString>() << "3";
	t.checkSignals(QString("*22*9*5#3#2#3##"), 0);
	t.checkSignals(QString("*22*9*5#3#4#1##"), 0);
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
	t.check(QString("*22*1#4#%1*2#%2##").arg(area).arg(source_id), area);
}

void TestVirtualSourceDevice::receiveSourceOff()
{
	MultiDeviceTester t(dev);
	t << makePair(VirtualSourceDevice::REQ_SOURCE_OFF, true);
	t << makePair(VirtualSourceDevice::DIM_STATUS, false);
	t << makePair(VirtualSourceDevice::DIM_AREAS_UPDATED, true);
	t.check(QString("*#22*2#%1*12*0*4##").arg(source_id));
}

void TestVirtualSourceDevice::testInitFrame()
{
	// multichannel, source
	QCOMPARE(VirtualSourceDevice::createMediaInitFrame(true, "3", ""), QString("*#22*7*#15*3***9*9**3*1*1*0*1##"));

	// monochannel, source
	QCOMPARE(VirtualSourceDevice::createMediaInitFrame(false, "3", ""), QString("*#22*7*#15*3***9*9***1*1*0*1##"));

	// multichannel, ampli
	QCOMPARE(VirtualSourceDevice::createMediaInitFrame(true, "", "28"), QString("*#22*7*#15*0*2*8*9*9***0*1*1*1##"));

	// monochannel, ampli
	QCOMPARE(VirtualSourceDevice::createMediaInitFrame(false, "", "28"), QString("*#22*7*#15*0*2*8*9*9***0*1*1*1##"));
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
	dev = d ? d : new AmplifierDevice(area + point);
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

void TestAmplifierDevice::sendSetVolume()
{
	int volume = 5;
	dev->setVolume(volume);
	client_command->flush();
	QString cmd(QString("*#22*3#%1#%2*#1*%3##").arg(area).arg(point).arg(volume));
	QCOMPARE(server->frameCommand(), cmd);
}


void TestAmplifierDevice::receiveStatus()
{
	DeviceTester t(dev, AmplifierDevice::DIM_STATUS);
	t.check(QString("*#22*3#%1#%2*12*1*0##").arg(area).arg(point), true);
	t.check(QString("*#22*3#%1#%2*12*0*255##").arg(area).arg(point), false);

	// global/environment frames
	t.check(QString("*#22*5#3#0#0*12*0*255##"), false);
	t.check(QString("*#22*4#%1*12*0*255##").arg(area), false);
	t.checkSignals(QString("*#22*4#%1*12*0*255##").arg(AMPLI_WRONG_AREA), 0);
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

void TestAmplifierDevice::testCrash()
{
	OpenMsg frame_request_high(qPrintable(QString("*#22*3#%1#%2*2##").arg(area).arg(point)));
	dev->manageFrame(frame_request_high);
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
		dev = new VirtualAmplifierDevice(area + point);
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
	QString cmd(QString("*#22*5#3#%1#%2*1*%3##").arg(area).arg(point).arg(NEW_VOL));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestVirtualAmplifierDevice::sendUpdateStatus()
{
	const int STATUS = 1;
	dev->updateStatus(STATUS);
	client_command->flush();
	QString cmd(QString("*#22*5#3#%1#%2*12*%3*3##").arg(area).arg(point).arg(STATUS));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestVirtualAmplifierDevice::sendVolumeUp()
{
	MultiDeviceTester t(dev);
	t << makePair(VirtualAmplifierDevice::REQ_VOLUME_UP, 1);
	t << makePair(VirtualAmplifierDevice::DIM_SELF_REQUEST, true);
	dev->volumeUp();
	t.check();
}

void TestVirtualAmplifierDevice::sendVolumeDown()
{
	MultiDeviceTester t(dev);
	t << makePair(VirtualAmplifierDevice::REQ_VOLUME_DOWN, 1);
	t << makePair(VirtualAmplifierDevice::DIM_SELF_REQUEST, true);
	dev->volumeDown();
	t.check();
}

void TestVirtualAmplifierDevice::sendTurnOn()
{
	MultiDeviceTester t(dev);
	t << makePair(VirtualAmplifierDevice::REQ_AMPLI_ON, true);
	t << makePair(VirtualAmplifierDevice::DIM_SELF_REQUEST, true);
	dev->turnOn();
	t.check();

	client_command->flush();
	QString cmd(QString("*22*34#4#%1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestVirtualAmplifierDevice::sendTurnOff()
{
	MultiDeviceTester t(dev);
	t << makePair(VirtualAmplifierDevice::REQ_AMPLI_ON, false);
	t << makePair(VirtualAmplifierDevice::DIM_SELF_REQUEST, true);
	dev->turnOff();
	t.check();

	client_command->flush();
	QString cmd(QString("*22*0#4#%1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestVirtualAmplifierDevice::sendSetVolume()
{
	MultiDeviceTester t(dev);
	t << makePair(VirtualAmplifierDevice::REQ_SET_VOLUME, 5);
	t << makePair(VirtualAmplifierDevice::DIM_SELF_REQUEST, true);
	dev->setVolume(5);
	t.check();
}

void TestVirtualAmplifierDevice::receiveAmplifierOn()
{
	DeviceTester t(dev, VirtualAmplifierDevice::REQ_AMPLI_ON);
	t.check(QString("*22*1#4#%1*3#%1#%2##").arg(area).arg(point), true);

	DeviceTester t1(dev, VirtualAmplifierDevice::REQ_AMPLI_ON);
	t1.check(QString("*22*1#4#0*5#3#0#0##"), true);

	DeviceTester t2(dev, VirtualAmplifierDevice::REQ_AMPLI_ON);
	t2.check(QString("*22*1#4#%1*4#%1##").arg(area), true);
}

void TestVirtualAmplifierDevice::receiveAmplifierOff()
{
	DeviceTester t(dev, VirtualAmplifierDevice::REQ_AMPLI_ON);
	t.check(QString("*22*0#4#%1*3#%1#%2##").arg(area).arg(point), false);
	t.check(QString("*22*0#4#0*5#3#0#0##"), false);
	t.check(QString("*22*0#4#%1*4#%1##").arg(area), false);
	t.check(QString("*22*0#4#15*5#1#1##"), false);
}

void TestVirtualAmplifierDevice::receiveVolumeUp()
{
	DeviceTester t(dev, VirtualAmplifierDevice::REQ_VOLUME_UP);
	// value is 4
	t.check(QString("*22*3#4*3#%1#%2##").arg(area).arg(point), 4);

	// value is missing
	t.check(QString("*22*3*3#%1#%2##").arg(area).arg(point), 1);
}

void TestVirtualAmplifierDevice::receiveVolumeDown()
{
	DeviceTester t(dev, VirtualAmplifierDevice::REQ_VOLUME_DOWN);
	// value is 25
	t.check(QString("*22*4#25*3#%1#%2##").arg(area).arg(point), 25);

	// value is missing
	t.check(QString("*22*4*3#%1#%2##").arg(area).arg(point), 1);
}

void TestVirtualAmplifierDevice::receiveSetVolume()
{
	const int VOLUME = 11;
	DeviceTester t(dev, VirtualAmplifierDevice::REQ_SET_VOLUME);
	t.check(QString("*#22*3#%1#%2*#1*%3##").arg(area).arg(point).arg(VOLUME), VOLUME);
}

void TestVirtualAmplifierDevice::receiveTemporaryOff()
{
	DeviceTester t(dev, VirtualAmplifierDevice::REQ_TEMPORARY_OFF);
	t.check(QString("*22*0#4#%1*6##").arg(area), true);
	t.check(QString("*22*22#4#%1*5#3#%1#%2##").arg(area).arg(point), true);
	t.check(QString("*22*22#4#%1*5#3#%1#%2##").arg(area).arg(AMPLI_WRONG_POINT), true);
	AmplifierDevice::setIsMultichannel(true);
	t.checkSignals(QString("*22*22#4#%1*5#3#%1#%2##").arg(AMPLI_WRONG_AREA).arg(point), 0);
	AmplifierDevice::setIsMultichannel(false);
	t.check(QString("*22*22#4#%1*5#3#%1#%2##").arg(AMPLI_WRONG_AREA).arg(point), true);
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
		dev = new PowerAmplifierDevice(area + point);
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
	QString cmd(QString("*22*40#1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendTrebleDown()
{
	dev->trebleDown();
	client_command->flush();
	QString cmd(QString("*22*41#1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendBassUp()
{
	dev->bassUp();
	client_command->flush();
	QString cmd(QString("*22*36#1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendBassDown()
{
	dev->bassDown();
	client_command->flush();
	QString cmd(QString("*22*37#1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendBalanceUp()
{
	dev->balanceUp();
	client_command->flush();
	QString cmd(QString("*22*42#1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendBalanceDown()
{
	dev->balanceDown();
	client_command->flush();
	QString cmd(QString("*22*43#1*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendNextPreset()
{
	dev->nextPreset();
	client_command->flush();
	QString cmd(QString("*22*55*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendPrevPreset()
{
	dev->prevPreset();
	client_command->flush();
	QString cmd(QString("*22*56*3#%1#%2##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendSetPreset()
{
	dev->setPreset(12);
	client_command->flush();
	QString cmd(QString("*#22*3#%1#%2*#19*%3##").arg(area).arg(point).arg(12));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendLoudOn()
{
	dev->loudOn();
	client_command->flush();
	QString cmd(QString("*#22*3#%1#%2*#20*1##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::sendLoudOff()
{
	dev->loudOff();
	client_command->flush();
	QString cmd(QString("*#22*3#%1#%2*#20*0##").arg(area).arg(point));
	QCOMPARE(server->frameCommand(), cmd);
}

void TestPowerAmplifierDevice::receiveLoud()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_LOUD);
	t.check(QString("*#22*3#%1#%2*20*0##").arg(area).arg(point), false);
	t.check(QString("*#22*3#%1#%2*20*1##").arg(area).arg(point), true);
}

void TestPowerAmplifierDevice::receivePreset()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_PRESET);
	t.check(QString("*#22*3#%1#%2*19*2##").arg(area).arg(point), 0);
	t.check(QString("*#22*3#%1#%2*19*6##").arg(area).arg(point), 4);
	t.check(QString("*#22*3#%1#%2*19*11##").arg(area).arg(point), 9);
	t.checkSignals(QString("*#22*3#%1#%2*19*12##").arg(area).arg(point), 0);
	t.checkSignals(QString("*#22*3#%1#%2*19*15##").arg(area).arg(point), 0);
	t.check(QString("*#22*3#%1#%2*19*16##").arg(area).arg(point), 10);
	t.check(QString("*#22*3#%1#%2*19*25##").arg(area).arg(point), 19);
}

void TestPowerAmplifierDevice::receiveTreble()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_TREBLE);
	t.check(QString("*#22*3#%1#%2*2*0##").arg(area).arg(point), -10);
	t.check(QString("*#22*3#%1#%2*2*3##").arg(area).arg(point), -9);
	t.check(QString("*#22*3#%1#%2*2*4##").arg(area).arg(point), -9);
	t.check(QString("*#22*3#%1#%2*2*30##").arg(area).arg(point), 0);
	t.check(QString("*#22*3#%1#%2*2*60##").arg(area).arg(point), 10);
}

void TestPowerAmplifierDevice::receiveBass()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_BASS);
	t.check(QString("*#22*3#%1#%2*4*0##").arg(area).arg(point), -10);
	t.check(QString("*#22*3#%1#%2*4*3##").arg(area).arg(point), -9);
	t.check(QString("*#22*3#%1#%2*4*4##").arg(area).arg(point), -9);
	t.check(QString("*#22*3#%1#%2*4*30##").arg(area).arg(point), 0);
	t.check(QString("*#22*3#%1#%2*4*60##").arg(area).arg(point), 10);
	t.checkSignals(QString("*22*4#1*3#%1#%2##").arg(area).arg(point), 0);
}

void TestPowerAmplifierDevice::receiveBalance()
{
	DeviceTester t(dev, PowerAmplifierDevice::DIM_BALANCE);
	t.check(QString("*#22*3#%1#%2*17*00##").arg(area).arg(point), 0);
	t.check(QString("*#22*3#%1#%2*17*03##").arg(area).arg(point), -1);
	t.check(QString("*#22*3#%1#%2*17*04##").arg(area).arg(point), -1);
	t.check(QString("*#22*3#%1#%2*17*030##").arg(area).arg(point), -10);
	t.check(QString("*#22*3#%1#%2*17*10##").arg(area).arg(point), 0);
	t.check(QString("*#22*3#%1#%2*17*11##").arg(area).arg(point), 0);
	t.check(QString("*#22*3#%1#%2*17*13##").arg(area).arg(point), 1);
	t.check(QString("*#22*3#%1#%2*17*115##").arg(area).arg(point), 5);
}


void TestAuxDevice::initTestCase()
{
	dev = new AuxDevice("22");
}

void TestAuxDevice::cleanupTestCase()
{
	delete dev;
}

void TestAuxDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#9*%1##").arg(dev->where));
}

void TestAuxDevice::receiveStatus()
{
	DeviceTester t(dev, AuxDevice::DIM_STATUS);
	t.check(QString("*9*1*%1##").arg(dev->where), true);
	t.check(QString("*9*0*%1##").arg(dev->where), false);
}

