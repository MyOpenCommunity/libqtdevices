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


#include "test_entryphone_device.h"
#include "openserver_mock.h"
#include "device_tester.h"

#include <openclient.h>
#include <openmsg.h>
#include <entryphone_device.h>

#include <QtTest/QtTest>

void TestEntryphoneDevice::initTestCase()
{
	dev = new EntryphoneDevice("11");
}

void TestEntryphoneDevice::cleanupTestCase()
{
	delete dev;
}

void TestEntryphoneDevice::init()
{
	dev->resetCallState();
}

void TestEntryphoneDevice::simulateIncomingCall(int kind, int mmtype)
{
	QString incoming_call = QString("*8*1#%1#%2#21*%3##").arg(kind).arg(mmtype).arg(dev->where);
	// I HATE YOU OpenMsg!
	OpenMsg msg(incoming_call.toStdString());
	dev->manageFrame(msg);
}

void TestEntryphoneDevice::simulateCallerAddress(int kind, int mmtype, QString where)
{
	QString frame = QString("*8*9#%1#%2*%3##").arg(kind).arg(mmtype).arg(where);

	OpenMsg msg(frame.toStdString());
	dev->manageFrame(msg);
}

void TestEntryphoneDevice::simulateRearmSession(int kind, int mmtype, QString where)
{
	QString frame = QString("*8*40#%1#%2*%3##").arg(kind).arg(mmtype).arg(where);

	OpenMsg msg(frame.toStdString());
	dev->manageFrame(msg);
}

void TestEntryphoneDevice::sendAnswerCall()
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

void TestEntryphoneDevice::sendEndCall()
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

void TestEntryphoneDevice::sendInitVctProcess()
{
	// call type accepted, 1 = scs bus only
	int type = 1;
	dev->initVctProcess();
	client_command->flush();
	QString frame = QString("*8*37#%1*%2##").arg(type).arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestEntryphoneDevice::sendCameraOn()
{
	QString where_camera = "20";
	dev->cameraOn(where_camera);
	client_command->flush();
	QString frame = QString("*8*4#%1*%2##").arg(dev->where).arg(where_camera);
	QCOMPARE(server->frameCommand(), frame);
}

void TestEntryphoneDevice::sendStairLightActivate()
{
	dev->stairLightActivate();
	client_command->flush();
	QString frame = QString("*8*21*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestEntryphoneDevice::sendStairLightRelease()
{
	dev->stairLightRelease();
	client_command->flush();
	QString frame = QString("*8*22*%1##").arg(dev->where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestEntryphoneDevice::sendOpenLock()
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

void TestEntryphoneDevice::sendReleaseLock()
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

void TestEntryphoneDevice::receiveIncomingCall()
{
	int kind = 1;
	int mmtype = 4;
	DeviceTester t(dev, EntryphoneDevice::VCT_CALL, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*8*1#%1#%2#21*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.check(frame, true);
}

void TestEntryphoneDevice::receiveStopVideo()
{
	int kind = 1;
	int mmtype = 3;
	DeviceTester t(dev, EntryphoneDevice::STOP_VIDEO);
	QString frame = QString("*8*3#%1#%2*%3##").arg(kind).arg(mmtype).arg(dev->where);
	t.check(frame, true);
}

void TestEntryphoneDevice::receiveCallerAddress1()
{
	int kind = 1;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, caller_addr);

	QCOMPARE(dev->caller_address, caller_addr);
	QCOMPARE(dev->master_caller_address, caller_addr);
}

void TestEntryphoneDevice::receiveCallerAddress2()
{
	int kind = 1;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);

	DeviceTester t1(dev, EntryphoneDevice::CALLER_ADDRESS, DeviceTester::MULTIPLE_VALUES);
	DeviceTester t2(dev, EntryphoneDevice::MOVING_CAMERA, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*8*9#%1#%2*%3##").arg(kind).arg(mmtype).arg(caller_addr);
	t1.check(frame, true);
	t2.check(frame, false);
}

void TestEntryphoneDevice::receiveCallerAddress3()
{
	int kind = 101;
	int mmtype = 4;
	QString caller_addr = "20";
	simulateIncomingCall(kind, mmtype);
	QString frame = QString("*8*9#%1#%2*%3##").arg(kind).arg(mmtype).arg(caller_addr);

	DeviceTester t1(dev, EntryphoneDevice::CALLER_ADDRESS, DeviceTester::MULTIPLE_VALUES);
	DeviceTester t2(dev, EntryphoneDevice::MOVING_CAMERA, DeviceTester::MULTIPLE_VALUES);
	t1.check(frame, true);
	t2.check(frame, true);
}

void TestEntryphoneDevice::receiveRearmSession()
{
	int kind = 1;
	int mmtype = 4;
	QString caller_addr = "20";
	QString master_caller_addr = "21";
	simulateIncomingCall(kind, mmtype);
	simulateCallerAddress(kind, mmtype, master_caller_addr);
	simulateRearmSession(kind, mmtype, caller_addr);

	QCOMPARE(dev->caller_address, caller_addr);
	QCOMPARE(dev->master_caller_address, master_caller_addr);
}

void TestEntryphoneDevice::sendCycleExternalUnits()
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

void TestEntryphoneDevice::sendInternalIntercomCall()
{
	QString where = "16";
	dev->internalIntercomCall(where);
	client_command->flush();
	QString frame = QString("*8*1#6#2#%1*%2##").arg(dev->where).arg(where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestEntryphoneDevice::sendExternalIntercomCall()
{
	QString where = "16";
	dev->externalIntercomCall(where);
	client_command->flush();
	QString frame = QString("*8*1#7#2#%1*%2##").arg(dev->where).arg(where);
	QCOMPARE(server->frameCommand(), frame);
}

void TestEntryphoneDevice::sendMoveUpPress()
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

void TestEntryphoneDevice::sendMoveUpRelease()
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

void TestEntryphoneDevice::sendMoveDownPress()
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

void TestEntryphoneDevice::sendMoveDownRelease()
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

void TestEntryphoneDevice::sendMoveLeftPress()
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

void TestEntryphoneDevice::sendMoveLeftRelease()
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

void TestEntryphoneDevice::sendMoveRightPress()
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

void TestEntryphoneDevice::sendMoveRightRelease()
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

