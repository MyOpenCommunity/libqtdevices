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


#ifndef TEST_ENTRYPHONE_DEVICE_H
#define TEST_ENTRYPHONE_DEVICE_H

#include "test_device.h"

class EntryphoneDevice;
class QString;


class TestEntryphoneDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();
	void init();

	void sendAnswerCall();
	void sendEndCall();
	void sendInitVctProcess();
	void sendCameraOn();
	void sendStairLightActivate();
	void sendStairLightRelease();
	void sendOpenLock();
	void sendReleaseLock();
	void sendCycleExternalUnits();
	void sendInternalIntercomCall();
	void sendExternalIntercomCall();
	void sendMoveUpPress();
	void sendMoveUpRelease();
	void sendMoveDownPress();
	void sendMoveDownRelease();
	void sendMoveLeftPress();
	void sendMoveLeftRelease();
	void sendMoveRightPress();
	void sendMoveRightRelease();

	void receiveIncomingCall();
	void receiveCallerAddress();
	void receiveRearmSession();

private:
	// init frame must be sent at constuction time
	void simulateIncomingCall(int kind, int mmtype);
	void simulateCallerAddress(int kind, int mmtype, QString where);
	void simulateRearmSession(int kind, int mmtype, QString where);

	EntryphoneDevice *dev;
};

#endif // TEST_ENTRYPHONE_DEVICE_H
