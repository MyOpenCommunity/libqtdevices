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


#include "test_videodoorentry_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "devices_cache.h" // bt_global::device_cache

#include <openclient.h>
#include <openmsg.h>
#include <videodoorentry_device.h>
#include <ringtonesmanager.h>

#include <QtTest/QtTest>

void TestVideoDoorEntryDevice::initTestCase()
{
	dev = new VideoDoorEntryDevice("11", "0");
}

void TestVideoDoorEntryDevice::cleanupTestCase()
{
	delete dev;
}

void TestVideoDoorEntryDevice::init()
{
	dev->resetCallState();
}

void TestVideoDoorEntryDevice::simulateIncomingCall(int kind, int mmtype)
{
	QString incoming_call = QString("*8*1#%1#%2#21*%3##").arg(kind).arg(mmtype).arg(dev->where);
	OpenMsg msg(incoming_call.toStdString());
	dev->manageFrame(msg);
}

void TestVideoDoorEntryDevice::simulateCallerAddress(int kind, int mmtype, QString where)
{
	QString frame = QString("*8*9#%1#%2*%3##").arg(kind).arg(mmtype).arg(where);

	OpenMsg msg(frame.toStdString());
	dev->manageFrame(msg);
}

void TestVideoDoorEntryDevice::sendAnswerCall()
{
	// ringtone 1
	int kind = 1;
	// audio and video call
	int mmtype = 4;
	// simulate an incoming call, since we must remember kind and mmtype
	simulateIncomingCall(kind, mmtype);
	QString caller_addr = "20";
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->answerCall();
	client_command->flush();
	// answers require device address as 'where'
	QCOMPARE(server->frameCommand(), QString("*8*2#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where));
}

void TestVideoDoorEntryDevice::sendEndCall()
{
	// ringtone 1
	int kind = 1;
	// audio and video call
	int mmtype = 4;
	simulateIncomingCall(kind, mmtype);
	const char *end_all_calls = "4";

	QString caller_addr = "20";
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->endCall();
	client_command->flush();
	QString frame = QString("*8*3#%1#%2*%3%4##").arg(kind).arg(mmtype).arg(end_all_calls).arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendInitVctProcess()
{
	VideoDoorEntryDevice::VctMode old_mode = dev->vct_mode;
	dev->vct_mode = VideoDoorEntryDevice::SCS_MODE;
	// call type accepted, 1 = scs bus only
	dev->initVctProcess();
	QVERIFY(bt_global::devices_cache.init_frames[0].contains(QString("*8*37#%1*%2##").arg(1).arg(dev->where)));
	bt_global::devices_cache.init_frames[0].clear();

	dev->vct_mode = VideoDoorEntryDevice::IP_MODE;
	// call type accepted, 1 = scs bus only
	dev->initVctProcess();
	QVERIFY(bt_global::devices_cache.init_frames[0].contains(QString("*8*37#%1*%2##").arg(2).arg(dev->where)));
	bt_global::devices_cache.init_frames[0].clear();

	// Restore the default status for the tests
	dev->vct_mode = old_mode;
}

void TestVideoDoorEntryDevice::sendCameraOn()
{
	QString where_camera = "20";
	dev->cameraOn(where_camera);
	client_command->flush();
	QString frame = QString("*8*4#%1*%2##").arg(dev->where).arg(where_camera);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendStairLightActivate()
{
	dev->stairLightActivate();
	client_command->flush();
	QString frame = QString("*8*21*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendStairLightRelease()
{
	dev->stairLightRelease();
	client_command->flush();
	QString frame = QString("*8*22*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendOpenLock()
{
	int kind = 1;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->openLock();
	client_command->flush();
	QString frame = QString("*8*19*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendReleaseLock()
{
	int kind = 1;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->releaseLock();
	client_command->flush();
	QString frame = QString("*8*20*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::receiveIncomingCall()
{
	int kind = 1;
	int mmtype = 4;
	MultiDeviceTester t(dev);
	t << makePair(VideoDoorEntryDevice::VCT_CALL, (int)VideoDoorEntryDevice::AUDIO_VIDEO);
	t << makePair(VideoDoorEntryDevice::RINGTONE, (int)Ringtones::PE1);
	QString frame = QString("*8*1#%1#%2#21*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.check(frame);
}

void TestVideoDoorEntryDevice::receiveIncomingIpCall()
{
	VideoDoorEntryDevice::VctMode old_mode = dev->vct_mode;
	dev->vct_mode = VideoDoorEntryDevice::IP_MODE;

	int kind = 1001;
	int mmtype = 4;
	int caller_address = 21;
	QString frame = QString("*8*1#%1#%2#%3*%4##").arg(kind).arg(mmtype).arg(caller_address).arg(dev->where);

	MultiDeviceTester t(dev);
	t << makePair(VideoDoorEntryDevice::VCT_CALL, (int)VideoDoorEntryDevice::AUDIO_VIDEO);
	t << makePair(VideoDoorEntryDevice::CALLER_ADDRESS, "21");
	t << makePair(VideoDoorEntryDevice::RINGTONE, (int)Ringtones::PE1);
	t << makePair(VideoDoorEntryDevice::MOVING_CAMERA, false);
	t.check(frame);
	QCOMPARE(dev->caller_address, QString::number(caller_address));
	QCOMPARE(dev->master_caller_address, QString::number(caller_address));
	dev->vct_mode = old_mode;
}

void TestVideoDoorEntryDevice::receiveIncomingIpCall2()
{
	VideoDoorEntryDevice::VctMode old_mode = dev->vct_mode;
	dev->vct_mode = VideoDoorEntryDevice::IP_MODE;

	int kind = 1005;
	int mmtype = 2;
	int caller_address = 21;
	QString frame = QString("*8*1#%1#%2#%3*%4##").arg(kind).arg(mmtype).arg(caller_address).arg(dev->where);

	MultiDeviceTester t(dev);
	t << makePair(VideoDoorEntryDevice::AUTO_VCT_CALL, (int)VideoDoorEntryDevice::ONLY_AUDIO);
	t << makePair(VideoDoorEntryDevice::CALLER_ADDRESS, "@21");
	t << makePair(VideoDoorEntryDevice::MOVING_CAMERA, false);
	t.check(frame);
	dev->vct_mode = old_mode;
}

void TestVideoDoorEntryDevice::receiveAutoswitchCall()
{
	int kind = 5;
	int mmtype = 4;

	DeviceTester t(dev, VideoDoorEntryDevice::AUTO_VCT_CALL);
	QString frame = QString("*8*1#%1#%2#21*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.check(frame, (int)VideoDoorEntryDevice::AUDIO_VIDEO);
}

void TestVideoDoorEntryDevice::receiveAudioCall()
{
	int kind = 5;
	int mmtype = 2;
	DeviceTester t(dev, VideoDoorEntryDevice::AUTO_VCT_CALL);
	QString frame = QString("*8*1#%1#%2#21*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.check(frame, (int)VideoDoorEntryDevice::ONLY_AUDIO);
}

void TestVideoDoorEntryDevice::receiveFloorCall()
{
	int kind = 1;
	int mmtype = 4;
	simulateIncomingCall(kind, mmtype);

	kind = 13;
	mmtype = 2;
	DeviceTester t(dev, VideoDoorEntryDevice::RINGTONE);
	QString frame = QString("*8*1#%1#%2#21*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.check(frame, (int)Ringtones::FLOORCALL);
	QCOMPARE(dev->kind, 1);
	QCOMPARE(dev->mmtype, 4);
}

void TestVideoDoorEntryDevice::receiveAnswer()
{
	int kind = 1;
	int mmtype = 4;
	simulateIncomingCall(kind, mmtype);
	DeviceTester t(dev, VideoDoorEntryDevice::ANSWER_CALL);
	QString frame = QString("*8*2#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.check(frame, true);
}

void TestVideoDoorEntryDevice::receiveUnconnectedAnswer()
{
	int kind = 1;
	int mmtype = 4;
	DeviceTester t(dev, VideoDoorEntryDevice::ANSWER_CALL);
	QString frame = QString("*8*2#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.checkSignals(frame, 0);
}

void TestVideoDoorEntryDevice::receiveStopVideo()
{
	int kind = 1;
	int mmtype = 3;
	simulateIncomingCall(kind, 4);
	DeviceTester t(dev, VideoDoorEntryDevice::STOP_VIDEO);
	QString frame = QString("*8*3#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.check(frame, true);
}

void TestVideoDoorEntryDevice::receiveUnconnectedStopVideo()
{
	int kind = 1;
	int mmtype = 3;
	DeviceTester t(dev, VideoDoorEntryDevice::STOP_VIDEO);
	QString frame = QString("*8*3#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.checkSignals(frame, 0);
}

void TestVideoDoorEntryDevice::receiveCallerAddress1()
{
	int kind = 1;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	QCOMPARE(dev->caller_address, caller_addr);
	QCOMPARE(dev->master_caller_address, caller_addr);
}

void TestVideoDoorEntryDevice::receiveCallerAddress2()
{
	int kind = 1;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	QString frame = QString("*8*9#%1#%2*%3##").arg(kind).arg(mmtype).arg(caller_addr);

	MultiDeviceTester t(dev);
	t << makePair(VideoDoorEntryDevice::CALLER_ADDRESS, true);
	t << makePair(VideoDoorEntryDevice::MOVING_CAMERA, false);
	t.check(frame);
}

void TestVideoDoorEntryDevice::receiveCallerAddress3()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	QString frame = QString("*8*9#%1#%2*%3##").arg(kind).arg(mmtype).arg(caller_addr);

	MultiDeviceTester t(dev);
	t << makePair(VideoDoorEntryDevice::CALLER_ADDRESS, true);
	t << makePair(VideoDoorEntryDevice::MOVING_CAMERA, true);
	t.check(frame);
}

void TestVideoDoorEntryDevice::receiveEndOfCall()
{
	int kind = 1;
	int mmtype = 4;
	simulateIncomingCall(kind, mmtype);
	DeviceTester t(dev, VideoDoorEntryDevice::END_OF_CALL);
	QString frame = QString("*8*3#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.check(frame, true);
}

void TestVideoDoorEntryDevice::receiveUnconnectedEndOfCall()
{
	int kind = 1;
	int mmtype = 4;
	DeviceTester t(dev, VideoDoorEntryDevice::END_OF_CALL);
	QString frame = QString("*8*3#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.checkSignals(frame, 0);
}

void TestVideoDoorEntryDevice::receiveRearmSession()
{
	int kind = 1;
	int mmtype = 4;
	QString caller_addr = "20";
	QString master_caller_addr = "21";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, master_caller_addr);

	QString frame = QString("*8*40#%1#%2*%3##").arg(kind).arg(mmtype).arg(caller_addr);
	MultiDeviceTester t(dev);
	t << makePair(VideoDoorEntryDevice::VCT_TYPE, (int)VideoDoorEntryDevice::AUDIO_VIDEO);
	t << makePair(VideoDoorEntryDevice::MOVING_CAMERA, false);
	t.check(frame);

	QCOMPARE(dev->caller_address, caller_addr);
	QCOMPARE(dev->master_caller_address, master_caller_addr);
}

void TestVideoDoorEntryDevice::receiveRearmSession2()
{
	int kind = 1;
	int mmtype = 2;
	QString caller_addr = "20";
	QString master_caller_addr = "21";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, master_caller_addr);

	QString frame = QString("*8*40#%1#%2*%3##").arg(kind).arg(mmtype).arg(caller_addr);
	MultiDeviceTester t(dev);
	t << makePair(VideoDoorEntryDevice::VCT_TYPE, (int)VideoDoorEntryDevice::ONLY_AUDIO);
	t << makePair(VideoDoorEntryDevice::MOVING_CAMERA, false);
	t.check(frame);

	QCOMPARE(dev->caller_address, caller_addr);
	QCOMPARE(dev->master_caller_address, master_caller_addr);
}

void TestVideoDoorEntryDevice::sendCycleExternalUnits()
{
	int kind = 1;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->cycleExternalUnits();
	client_command->flush();
	QString frame = QString("*8*6#%1*%2##").arg(dev->where).arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendInternalIntercomCall()
{
	QString where = "16";
	QCOMPARE(dev->is_calling, false);
	dev->internalIntercomCall(where);
	QCOMPARE(dev->is_calling, true);
	client_command->flush();
	QString frame = QString("*8*1#6#2#%1*%2##").arg(dev->where).arg(where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendExternalIntercomCall()
{
	QString where = "16";
	QCOMPARE(dev->is_calling, false);
	dev->externalIntercomCall(where);
	QCOMPARE(dev->is_calling, true);
	client_command->flush();
	QString frame = QString("*8*1#7#2#%1*%2##").arg(dev->where).arg(where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendPagerCall()
{
	QCOMPARE(dev->is_calling, false);
	QCOMPARE(dev->is_waiting_pager_answer, false);
	dev->pagerCall();
	QCOMPARE(dev->is_calling, true);
	QCOMPARE(dev->is_waiting_pager_answer, true);
	client_command->flush();
	QString frame = QString("*8*1#14#2#11*4##");
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendPagerAnswer()
{
	QCOMPARE(dev->is_calling, false);
	dev->pagerAnswer();
	QCOMPARE(dev->is_calling, true);
	client_command->flush();
	QString frame = QString("*8*2#14#2#11*4##");
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::receivePagerCall()
{
	int kind = 14;
	int mmtype = 2;

	MultiDeviceTester t(dev);
	t << makePair(VideoDoorEntryDevice::PAGER_CALL, (int)VideoDoorEntryDevice::ONLY_AUDIO);
	t << makePair(VideoDoorEntryDevice::RINGTONE, (int)Ringtones::PI_INTERCOM);
	QString frame = QString("*8*1#%1#%2#%3*4##").arg(kind).arg(mmtype).arg("16");
	t.check(frame);
	QCOMPARE(dev->kind, 14);
	QCOMPARE(dev->mmtype, 2);
	QCOMPARE(dev->caller_address, QString("16"));
}

void TestVideoDoorEntryDevice::receivePagerAnswer()
{
	int kind = 14;
	int mmtype = 2;

	MultiDeviceTester t0(dev);
	t0 << makePair(VideoDoorEntryDevice::END_OF_CALL, true);

	// arrives an answer without having made a call, ignores it
	QCOMPARE(dev->is_waiting_pager_answer, false);
	QCOMPARE(dev->is_calling, false);
	QCOMPARE(dev->ip_call, false);
	QCOMPARE(dev->caller_address, QString());
	QCOMPARE(dev->master_caller_address, QString());
	QCOMPARE(dev->kind, -1);
	QCOMPARE(dev->mmtype, -1);
	t0.check(QString("*8*2#%1#%2#%3*4##").arg(kind).arg(mmtype).arg("16"));
	QCOMPARE(dev->is_waiting_pager_answer, false);
	QCOMPARE(dev->is_calling, false);
	QCOMPARE(dev->ip_call, false);
	QCOMPARE(dev->caller_address, QString());
	QCOMPARE(dev->master_caller_address, QString());
	QCOMPARE(dev->kind, -1);
	QCOMPARE(dev->mmtype, -1);

	// sends a pager call
	QCOMPARE(dev->is_waiting_pager_answer, false);
	dev->pagerCall();
	QCOMPARE(dev->is_waiting_pager_answer, true);
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*8*1#14#2#11*4##"));

	MultiDeviceTester t(dev);

	// receives the pager answer it is waiting for
	t << makePair(VideoDoorEntryDevice::ANSWER_CALL, true);
	t << makePair(VideoDoorEntryDevice::CALLER_ADDRESS, "16");
	t.check(QString("*8*2#%1#%2#%3*4##").arg(kind).arg(mmtype).arg("16"));
	QCOMPARE(dev->kind, 14);
	QCOMPARE(dev->mmtype, 2);
	QCOMPARE(dev->caller_address, QString("16"));
}

void TestVideoDoorEntryDevice::sendMoveUpPress()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->moveUpPress();
	client_command->flush();
	QString frame = QString("*8*59#1*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendMoveUpRelease()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->moveUpRelease();
	client_command->flush();
	QString frame = QString("*8*59#2*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendMoveDownPress()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->moveDownPress();
	client_command->flush();
	QString frame = QString("*8*60#1*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendMoveDownRelease()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->moveDownRelease();
	client_command->flush();
	QString frame = QString("*8*60#2*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendMoveLeftPress()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->moveLeftPress();
	client_command->flush();
	QString frame = QString("*8*61#1*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendMoveLeftRelease()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->moveLeftRelease();
	client_command->flush();
	QString frame = QString("*8*61#2*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendMoveRightPress()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->moveRightPress();
	client_command->flush();
	QString frame = QString("*8*62#1*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

void TestVideoDoorEntryDevice::sendMoveRightRelease()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	dev->moveRightRelease();
	client_command->flush();
	QString frame = QString("*8*62#2*%1##").arg(caller_addr);
	QCOMPARE(server->frameCommand(), frame);
}

