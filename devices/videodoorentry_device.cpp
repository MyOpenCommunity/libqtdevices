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


#include "videodoorentry_device.h"
#include "frame_functions.h" // createCommandFrame
#include "openclient.h" // MAIN_OPENSERVER
#include "devices_cache.h"

#include <openmsg.h>
#include <QDebug>

// this value must be sent before the address of the unit that terminates the call
// otherwise CALL_END frame is not valid.
static const char *END_ALL_CALLS = "4";

enum
{
	CALL = 1,
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

BasicVideoDoorEntryDevice::BasicVideoDoorEntryDevice(const QString &where, QString mode, int openserver_id) :
	device(QString("8"), where, openserver_id)
{
	if (mode.isNull())
		vct_mode = NONE;
	else if (mode.toInt() == 1)
		vct_mode = IP_MODE;
	else
		vct_mode = SCS_MODE;
	initVctProcess();
}

void BasicVideoDoorEntryDevice::stairLightActivate(QString target_where) const
{
	sendCommand(QString::number(STAIRCASE_ACTIVATE), target_where);
}

void BasicVideoDoorEntryDevice::stairLightRelease(QString target_where) const
{
	sendCommand(QString::number(STAIRCASE_RELEASE), target_where);
}

void BasicVideoDoorEntryDevice::openLock(QString target_where) const
{
	sendCommand(QString::number(OPEN_LOCK), target_where);
}

void BasicVideoDoorEntryDevice::releaseLock(QString target_where) const
{
	sendCommand(QString::number(RELEASE_LOCK), target_where);
}

void BasicVideoDoorEntryDevice::initVctProcess()
{
	if (vct_mode != NONE)
	{
		int type = vct_mode == SCS_MODE ? 1 : 2;
		QString what = QString("%1#%2").arg(READY).arg(type);

		// We use this method instead of using the "init()" in order to send the frame
		// before the frames sent by the other devices.
		bt_global::devices_cache.addInitCommandFrame(MAIN_OPENSERVER, createCommandFrame(who, what, where));
	}
}

bool BasicVideoDoorEntryDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (static_cast<int>(msg.what()) == CALLER_ADDRESS)
	{
		values_list[CALLER_ADDRESS] = QString::fromStdString(msg.whereFull());
		return true;
	}

	return false;
}


VideoDoorEntryDevice::VideoDoorEntryDevice(const QString &where, QString mode, int openserver_id) :
	BasicVideoDoorEntryDevice(where, mode, openserver_id)
{
	// invalid values
	kind = mmtype = -1;
	is_calling = false;
	ip_call = false;
	is_waiting_pager_answer = false;
}

void VideoDoorEntryDevice::answerCall() const
{
	QString what = QString("%1#%2#%3").arg(ANSWER_CALL).arg(kind).arg(mmtype);
	sendCommand(what);
}

void VideoDoorEntryDevice::answerPagerCall() const
{
	QString cmd = QString("*8*%1#%2#%3#%4*4##").arg(ANSWER_CALL).arg(kind).arg(mmtype).arg(where);
	sendFrame(cmd);
}

void VideoDoorEntryDevice::internalIntercomCall(QString _where)
{
	kind = 6;
	mmtype = 2;
	is_calling = true;

	QString what = QString("%1#%2#%3#%4").arg(CALL).arg(kind).arg(mmtype).arg(where);
	sendCommand(what, _where);
}

void VideoDoorEntryDevice::externalIntercomCall(QString _where)
{
	kind = 7;
	mmtype = 2;
	is_calling = true;

	QString what = QString("%1#%2#%3#%4").arg(CALL).arg(kind).arg(mmtype).arg(where);
	sendCommand(what, _where);
}

void VideoDoorEntryDevice::pagerCall()
{
	kind = 14;
	mmtype = 2;
	is_calling = true;
	is_waiting_pager_answer = true;

	QString cmd = QString("*8*%1#%2#%3#%4*4##").arg(CALL).arg(kind).arg(mmtype).arg(where);
	sendFrame(cmd);
}

void VideoDoorEntryDevice::pagerAnswer()
{
	kind = 14;
	mmtype = 2;
	is_calling = true;
	is_waiting_pager_answer = true; // waits for ANSWER_CALL frame to confirm call

	QString cmd = QString("*8*%1#%2#%3#%4*4##").arg(ANSWER_CALL).arg(kind).arg(mmtype).arg(where);
	sendFrame(cmd);
}

void VideoDoorEntryDevice::cameraOn(QString _where) const
{
	QString what = QString("%1#%2").arg(AUTOSWITCHING).arg(where);
	sendCommand(what, _where);
}

void VideoDoorEntryDevice::stairLightActivate() const
{
	BasicVideoDoorEntryDevice::stairLightActivate(where);
}

void VideoDoorEntryDevice::stairLightRelease() const
{
	BasicVideoDoorEntryDevice::stairLightRelease(where);
}

void VideoDoorEntryDevice::openLock() const
{
	BasicVideoDoorEntryDevice::openLock(is_calling ? caller_address : where);
}

void VideoDoorEntryDevice::releaseLock() const
{
	BasicVideoDoorEntryDevice::releaseLock(is_calling ? caller_address : where);
}

void VideoDoorEntryDevice::cycleExternalUnits() const
{
	// Cycling on the external units configured means send a specific frame to
	// the first unit that we turn on, no matter what is the current camera
	// displayed.
	sendCommand(QString("%1#%2").arg(CYCLE_EXT_UNIT).arg(where), master_caller_address);
}

void VideoDoorEntryDevice::endCall()
{
	QString what = QString("%1#%2#%3").arg(END_OF_CALL).arg(kind).arg(mmtype);
	sendFrame(createCommandFrame(who, what, QString(END_ALL_CALLS) + where));

	// In the vct full ip we have to wait to execute the scripts after the end
	// of the calls. So we wait for the END_OF_CALL frame that the underlying
	// bt_<process> forward on the monitor channel to close the call from the
	// GUI/device point of view.
	if (vct_mode == SCS_MODE)
		resetCallState();
}

void VideoDoorEntryDevice::cameraMovePress(int move_type) const
{
	QString what = QString("%1#%2").arg(move_type).arg(1);
	sendCommand(what, caller_address);
}

void VideoDoorEntryDevice::cameraMoveRelease(int move_type) const
{
	QString what = QString("%1#%2").arg(move_type).arg(2);
	sendCommand(what, caller_address);
}

void VideoDoorEntryDevice::moveUpPress() const
{
	cameraMovePress(MOVE_UP);
}

void VideoDoorEntryDevice::moveUpRelease() const
{
	cameraMoveRelease(MOVE_UP);
}

void VideoDoorEntryDevice::moveDownPress() const
{
	cameraMovePress(MOVE_DOWN);
}

void VideoDoorEntryDevice::moveDownRelease() const
{
	cameraMoveRelease(MOVE_DOWN);
}

void VideoDoorEntryDevice::moveLeftPress() const
{
	cameraMovePress(MOVE_LEFT);
}

void VideoDoorEntryDevice::moveLeftRelease() const
{
	cameraMoveRelease(MOVE_LEFT);
}

void VideoDoorEntryDevice::moveRightPress() const
{
	cameraMovePress(MOVE_RIGHT);
}

void VideoDoorEntryDevice::moveRightRelease() const
{
	cameraMoveRelease(MOVE_RIGHT);
}

bool VideoDoorEntryDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	qWarning() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
	int what = msg.what();
	qWarning() << what;
	if (what == SILENCE_MM_AMPLI || what == RESTORE_MM_AMPLI)
	{
		values_list[what] = true;
		return true;
	}

	// We want parse all frames if we are in connected state, and only the CALL
	// frame if we are in unconnected state.
	bool is_call = (what == CALL);
	bool is_my_where = (QString::fromStdString(msg.whereFull()) == where);
	bool is_broadcast_where = (QString::fromStdString(msg.whereFull()) == QString("4"));
	bool is_pager_call = (msg.whatArgN(0) % 100 == 14);
	bool is_answer_call = (what == ANSWER_CALL);
	qWarning() << is_call << is_my_where << is_broadcast_where << is_pager_call << is_answer_call << is_waiting_pager_answer;

	// if someone else answers to a pager call, I send an END_OF_CALL frame
	if (!is_waiting_pager_answer && is_pager_call && is_answer_call)
	{
		qWarning() << "sending end of call";
		resetCallState();
		values_list[END_OF_CALL] = true;
		return true;
	}

	qWarning() << is_calling;

	if (!is_calling)
	{
		if (!is_call) // ignores all frames except CALL
			return false;
		if (!is_my_where && !is_broadcast_where) // ignores if where is not mine
			return false;
	}

	switch (what)
	{
	case CALL:
	{
		Q_ASSERT_X(msg.whatArgCnt() >= 2, __PRETTY_FUNCTION__, "Incomplete open frame received");

		ip_call = msg.whatArgN(0) > 1000;
		if ((ip_call && vct_mode != IP_MODE) || (!ip_call && vct_mode != SCS_MODE))
			qWarning() << "The incoming call has a different mode than the configured one";

		int kind_val = msg.whatArgN(0) % 100;
		int ringtone = -1;
		switch (kind_val)
		{
		case 1:
			what = VCT_CALL;
			ringtone = PE1;
			break;
		case 2:
			what = VCT_CALL;
			ringtone = PE2;
			break;
		case 3:
			what = VCT_CALL;
			ringtone = PE3;
			break;
		case 4:
			what = VCT_CALL;
			ringtone = PE4;
			break;
		case 5:
			what = AUTO_VCT_CALL;
			break;
		case 6:
			what = INTERCOM_CALL;
			ringtone = PI_INTERCOM;
			break;
		case 7:
			what = INTERCOM_CALL;
			ringtone = PE_INTERCOM;
			break;
		case 13:
			values_list[RINGTONE] = FLOORCALL;
			return true;
		case 14:
			what = PAGER_CALL;
			ringtone = PI_INTERCOM;
			break;
		default:
			qWarning() << "Kind" << msg.whatArgN(0) << "not supported by VideoDoorEntryDevice, skip frame";
			return false;
		}

		kind = msg.whatArgN(0);
		mmtype = msg.whatArgN(1);

		if (ringtone != -1)
			values_list[RINGTONE] = ringtone;

		// we can safely ignore caller address, we will receive a frame later, except for pager call
		values_list[what] = (mmtype == 2 ? ONLY_AUDIO : AUDIO_VIDEO);
		is_calling = true;
		// In the ip calls, the caller address is extracted from the call frame.
		if (!ip_call)
			break;
	}
	case CALLER_ADDRESS:
	{
		master_caller_address = QString::fromStdString(ip_call ? msg.whatArg(2) : msg.whereFull());
		int kind_val = msg.whatArgN(0) % 100;
		values_list[CALLER_ADDRESS] = (kind_val == 5 ? QChar('@') + master_caller_address : master_caller_address);
	}
		// manage the other things like in the rearm session case
	case REARM_SESSION:
	{
		caller_address = QString::fromStdString(ip_call ? msg.whatArg(2) : msg.whereFull());

		Q_ASSERT_X(msg.whatArgCnt() >= 2, __PRETTY_FUNCTION__, "Incomplete open frame received");
		kind = msg.whatArgN(0);
		mmtype = msg.whatArgN(1);
		// The third digit means if the camera can receive movement instructions
		// or not.
		int kind_m = kind % 1000;
		values_list[MOVING_CAMERA] = (kind_m >= 101 && kind_m <= 105);

		// The type of videocall can change only when I change the camera observed.
		if (what == REARM_SESSION)
			values_list[VCT_TYPE] = (mmtype == 2 ? ONLY_AUDIO : AUDIO_VIDEO);
		break;
	}

	case ANSWER_CALL:
	{
		ip_call = msg.whatArgN(0) > 1000;
		qWarning() << ip_call << vct_mode;
		if ((ip_call && vct_mode != IP_MODE) || (!ip_call && vct_mode != SCS_MODE))
			qWarning() << "The incoming call has a different mode than the configured one";

		qWarning() << is_waiting_pager_answer;
		if (is_waiting_pager_answer)
		{
			Q_ASSERT_X(msg.whatArgCnt() >= 2, __PRETTY_FUNCTION__, "Incomplete open frame received");
			kind = msg.whatArgN(0);
			mmtype = msg.whatArgN(1);
		}
		qWarning() << kind << mmtype;

		values_list[ANSWER_CALL] = true;
		break;
	}

	case END_OF_CALL:
		if (msg.whatArgN(1) == 3) // with mmtype == 3 we have to stop the video
		{
			if (vct_mode == SCS_MODE) // we manage the frame only for the scs vct.
				values_list[STOP_VIDEO] = true;
		}
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

void VideoDoorEntryDevice::resetCallState()
{
	is_calling = false;
	ip_call = false;
	caller_address = "";
	master_caller_address = "";
	kind = -1;
	mmtype = -1;
	is_waiting_pager_answer = false;
}
