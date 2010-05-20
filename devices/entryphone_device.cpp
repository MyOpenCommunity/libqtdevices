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


#include "entryphone_device.h"
#include "generic_functions.h" // createCommandFrame
#include "ringtonesmanager.h"
#include "openclient.h" // MAIN_OPENSERVER

#include <openmsg.h>
#include <QDebug>

// this value must be sent before the address of the unit that terminates the call
// otherwise CALL_END frame is not valid.
static const char *END_ALL_CALLS = "4";

enum
{
	CALL = 1,
	ANSWER = 2,
	AUTOSWITCHING = 4,
	CYCLE_EXT_UNIT = 6,
	OPEN_LOCK = 19,
	RELEASE_LOCK = 20,
	STAIRCASE_ACTIVATE = 21,
	STAIRCASE_RELEASE = 22,
	READY = 37,
	REARM_SESSION = 40,
	MOVE_UP = 59,
	MOVE_DOWN = 60,
	MOVE_LEFT = 61,
	MOVE_RIGHT = 62,
};

EntryphoneDevice::EntryphoneDevice(const QString &where, QString mode, int openserver_id) :
	device(QString("8"), where, openserver_id)
{
	// invalid values
	kind = mmtype = -1;
	vct_mode = mode;
	is_calling = false;
}

void EntryphoneDevice::answerCall() const
{
	// TODO: this can be removed once the code is tested
	if (kind == -1 || mmtype == -1)
	{
		qWarning() << "Kind or mmtype are invalid, there's no active call";
		return;
	}
	QString what = QString("%1#%2#%3").arg(ANSWER).arg(kind).arg(mmtype);
	sendCommand(what);
}

void EntryphoneDevice::internalIntercomCall(QString _where)
{
	kind = 6;
	mmtype = 2;

	QString what = QString("%1#%2#%3#%4").arg(CALL).arg(kind).arg(mmtype).arg(where);
	sendCommand(what, _where);
}

void EntryphoneDevice::externalIntercomCall(QString _where)
{
	kind = 7;
	mmtype = 2;

	QString what = QString("%1#%2#%3#%4").arg(CALL).arg(kind).arg(mmtype).arg(where);
	sendCommand(what, _where);
}

void EntryphoneDevice::cameraOn(QString _where) const
{
	QString what = QString("%1#%2").arg(AUTOSWITCHING).arg(where);
	sendCommand(what, _where);
}

void EntryphoneDevice::stairLightActivate() const
{
	sendCommand(STAIRCASE_ACTIVATE);
}

void EntryphoneDevice::stairLightRelease() const
{
	sendCommand(STAIRCASE_RELEASE);
}

void EntryphoneDevice::openLock() const
{
	if (is_calling)
		sendCommand(QString::number(OPEN_LOCK), caller_address);
	else
		sendCommand(QString::number(OPEN_LOCK));
}

void EntryphoneDevice::releaseLock() const
{
	if (is_calling)
		sendCommand(QString::number(RELEASE_LOCK), caller_address);
	else
		sendCommand(QString::number(RELEASE_LOCK));
}

void EntryphoneDevice::cycleExternalUnits() const
{
	// Cycling on the external units configured means send a specific frame to
	// the first unit that we turn on, no matter what is the current camera
	// displayed.
	sendCommand(QString("%1#%2").arg(CYCLE_EXT_UNIT).arg(where), master_caller_address);
}

void EntryphoneDevice::endCall()
{
	QString what = QString("%1#%2#%3").arg(END_OF_CALL).arg(kind).arg(mmtype);
	sendFrame(createCommandFrame(who, what, QString(END_ALL_CALLS) + where));
	resetCallState();
}

void EntryphoneDevice::initVctProcess()
{
	if (!vct_mode.isNull())
	{
		// vct_mode == 0 -> SCS
		// vct_mode == 1 -> IP
		int type = vct_mode.toInt() == 0 ? 1 : 2;
		QString what = QString("%1#%2").arg(READY).arg(type);
		sendCommand(what);
	}
}

void EntryphoneDevice::cameraMovePress(int move_type) const
{
	QString what = QString("%1#%2").arg(move_type).arg(1);
	sendCommand(what, caller_address);
}

void EntryphoneDevice::cameraMoveRelease(int move_type) const
{
	QString what = QString("%1#%2").arg(move_type).arg(2);
	sendCommand(what, caller_address);
}

void EntryphoneDevice::moveUpPress() const
{
	cameraMovePress(MOVE_UP);
}

void EntryphoneDevice::moveUpRelease() const
{
	cameraMoveRelease(MOVE_UP);
}

void EntryphoneDevice::moveDownPress() const
{
	cameraMovePress(MOVE_DOWN);
}

void EntryphoneDevice::moveDownRelease() const
{
	cameraMoveRelease(MOVE_DOWN);
}

void EntryphoneDevice::moveLeftPress() const
{
	cameraMovePress(MOVE_LEFT);
}

void EntryphoneDevice::moveLeftRelease() const
{
	cameraMoveRelease(MOVE_LEFT);
}

void EntryphoneDevice::moveRightPress() const
{
	cameraMovePress(MOVE_RIGHT);
}

void EntryphoneDevice::moveRightRelease() const
{
	cameraMoveRelease(MOVE_RIGHT);
}

bool EntryphoneDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (!is_calling && QString::fromStdString(msg.whereFull()) != where)
		return false;

	int what = msg.what();

	switch (what)
	{
	case CALL:
	{
		Q_ASSERT_X(msg.whatSubArgCnt() < 2, "EntryphoneDevice::parseFrame",
			"Incomplete open frame received");
		kind = msg.whatArgN(0);
		mmtype = msg.whatArgN(1);

		int kind_val = msg.whatArgN(0) % 100;
		int ringtone = -1;
		switch (kind_val)
		{
		case 1:
			what = VCT_CALL;
			ringtone = Ringtones::PE1;
			break;
		case 2:
			what = VCT_CALL;
			ringtone = Ringtones::PE2;
			break;
		case 3:
			what = VCT_CALL;
			ringtone = Ringtones::PE3;
			break;
		case 4:
			what = VCT_CALL;
			ringtone = Ringtones::PE4;
			break;
		case 5:
			what = AUTO_VCT_CALL;
			break;
		case 6:
			what = INTERCOM_CALL;
			ringtone = Ringtones::PI_INTERCOM;
			break;
		case 7:
			what = INTERCOM_CALL;
			ringtone = Ringtones::PE_INTERCOM;
			break;
		default:
			qWarning("Kind not supported by EntryphoneDevice, skip frame");
			return false;
		}

		if (ringtone != -1)
			values_list[RINGTONE] = ringtone;

		// we can safely ignore caller address, we will receive a frame later.
		values_list[what] = true;
		is_calling = true;
		break;
	}
	case CALLER_ADDRESS:
	{
		master_caller_address = QString::fromStdString(msg.whereFull());
		int kind_val = msg.whatArgN(0) % 100;
		if (kind_val != 5)
			values_list[CALLER_ADDRESS] = true; // the value in the DeviceValues doesn't matter.
	}
		// manage the other things like in the rearm session case
	case REARM_SESSION:
	{
		caller_address = QString::fromStdString(msg.whereFull());

		Q_ASSERT_X(msg.whatSubArgCnt() < 2, "EntryphoneDevice::parseFrame",
			"Incomplete open frame received");
		kind = msg.whatArgN(0);
		mmtype = msg.whatArgN(1);
		// The third digit means if the camera can receive movement instructions
		// or not.
		int kind_m = kind % 1000;
		values_list[MOVING_CAMERA] = (kind_m >= 101 && kind_m <= 105);
		break;
	}

	case END_OF_CALL:
		if (msg.whatArgN(1) == 3) // with mmtype == 3 we have to stop the video
			values_list[STOP_VIDEO] = true;
		else
		{
			resetCallState();
			values_list[END_OF_CALL] = true;
		}
		break;
	default:
		return false;
	}
	return true;
}

void EntryphoneDevice::resetCallState()
{
	is_calling = false;
	caller_address = "";
	master_caller_address = "";
	kind = -1;
	mmtype = -1;
}
