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


#include "pulldevice.h"
#include "frame_functions.h" // isWriteDimensionFrame, isDimensionFrame
#include "main.h" // bt_global::config, TS_NUMBER_FRAME_DELAY
#include <openmsg.h>

#include <QPair>
#include <QDebug>
#include <QString>

/*
 * Pull/non-pull devices:
 * a pull device does not react to environment/general frames; at startup we do not
 * know if a specific HW device is pull or non-pull (no indication in the configuration
 * file)
 *
 * Pull/non pull discovery algorithmm:
 *
 * The device starts with unknown state (opening/closing/stopped) and unknown pull
 * mode (pull/non-pull)
 *
 * After receiving one point-to-point measure frame, the device state is set to the state
 * contained in the frame.
 *
 * When receiving an environment/generic frame, and if the device state is known,
 * we send a point-to-point status request for the device, because we want to know if
 * the device reacted to the environment/general frame.
 *
 * When receiving the point-to-point measure frame in answer to the previous request, if
 * the status is equal to the saved status it means that the device did not react to the
 * environment/generic frame (and so is a PULL device); if the status differs from the
 * saved status, then the device is a  NON-PULL device
 *
 *
 * If the environment/generic frame is received before any point-to-point frames (when
 * both status and mode are unknown), the device always requests the point to point state to:
 * - bootstrap the discovery process
 * - retrieve the status of the device
 *
 *
 * There is a twist in that some actuators may or may not react to some frames; for example
 * some advanced light actuators react to dimmer 100 on/off frames.  The FrameChecker is used
 * to handle this case; if it returns FRAME_NOT_HANDLED the frame is completely ignored; if it
 * returns FRAME_HANDLED the detection algorithm works as described above; when it returns
 * FRAME_MAYBE_HANDLED, the algorithm is run as above, but the pull state is changed only if it
 * is non pull (because we do not know if the device does not react to frames because it is in pull
 * mode or because it can't understand the frames).
 */

/*
 * Split a where into a+pf part and address extension.
 */
QPair<QString, QString> splitWhere(const QString &w)
{
	int idx = w.indexOf("#");
	if (idx < 0)
		return qMakePair(w, QString());
	else
		return qMakePair(w.left(idx), w.mid(idx));
}

/*
 * Return the environment part from a+pf string
 */
QString getEnvironment(const QString &w)
{
	if (w.length() == 2)
		return w.mid(0, 1);
	else if (w.length() == 4)
		return w.mid(0, 2);
	else if (w.length() == 3)
	{
		Q_ASSERT_X(w == "100", "getEnvironment", "Environment with legth 3 must be 100 only");
		return "10";
	}
	else if (w.length() == 1)
		return w;

	return QString();
}


AddressType checkAddressIsForMe(const QString &msg_where, const QString &dev_where)
{
	if (msg_where == dev_where)
		return P2P;

	// frame where (input)
	QPair<QString, QString> in = splitWhere(msg_where);
	// device where (our)
	QPair<QString, QString> our = splitWhere(dev_where);

	// general address: 0 = all levels
	if (in.first == "0" && in.second.isEmpty())
		return GLOBAL;

	// no level means level 3 (except for the global address that is taken care of above)
	if (in.second == "#3")
		in.second = QString();

	// 0#lev = general for the level
	if (in.first == "0" && in.second == our.second)
		return GLOBAL;

	// for environment frames, check that level matches
	if (in.second != our.second)
		return NOT_MINE;

	// here we don't need to care about extension anymore

	// environment address. The first part must be "00", "100" or numbers 1 to 9
	// use toInt() to remove differences between "00" "0" and so on.
	if (in.first == "00" || in.first == "100" || in.first.length() == 1)
		if (getEnvironment(our.first).toInt() == getEnvironment(in.first).toInt())
			return ENVIRONMENT;

	return NOT_MINE;
}

enum
{
	INVALID_STATE = -1,
	VARIABLE_TIMING_STATE = 1234,   // something different from all other 'what' of automation system (ie 0 to 18
	                                // for normal frames and 100 to 200 for dimmer100 level frames).
};

PullStateManager::PullStateManager(PullMode m, AdvancedMode adv, FrameChecker checker)
{
	mode = m;
	status = INVALID_STATE;
	status_requested = false;
	frame_checker = checker;
	last_handled = FRAME_NOT_HANDLED;
	if (m == PULL && adv == PULL_ADVANCED_UNKNOWN)
		advanced = PULL_NOT_ADVANCED;
	else
		advanced = adv;
}

PullMode PullStateManager::getPullMode()
{
	return mode;
}

/*
 * When ignoring the frame, return false so that the calling code doesn't generate useless
 * traffic on the bus.
 */
PullStateManager::CheckResult PullStateManager::moreFrameNeeded(OpenMsg &msg, bool is_environment)
{
	FrameHandled handled = frame_checker ? frame_checker(msg) : FRAME_HANDLED;
	if (handled == FRAME_NOT_HANDLED)
		return qMakePair(false, handled);

	// PullStateManager will be used for automation and lighting only.
	// I'll handle all 'what' combinations here, split to a different function or class when needed
	// We need to look for write environment commands
	int new_state = msg.what();
	bool measure_frame = (is_environment && isWriteDimensionFrame(msg)) || (!is_environment && isDimensionFrame(msg));
	bool ignore_frame = false;
	if (measure_frame)
	{
		// dimmer 100 status
		if (msg.what() == 1)
			new_state = msg.whatArgN(0) - 100;
		// variable temporization
		// this is the trickiest one. Remember that devices don't send status updates if they are light on
		// by a general var timing frame. We have two cases:
		// - if the frame is environment and we are ON, we can't decide anything; we need to skip this frame
		// - if the frame is PP, we skip the frame since we can decide the state based on other PP frames
		else
		{
			if (is_environment)
				status > 0 ? ignore_frame = true : new_state = VARIABLE_TIMING_STATE;
			else
				ignore_frame = true;
		}
	}

	if (ignore_frame)
		return qMakePair(false, handled);

	if (mode != PULL_UNKNOWN && handled == FRAME_HANDLED && is_environment)
	{
		status = new_state;
		last_handled = handled;

		return qMakePair(false, handled);
	}
	else if (is_environment)
	{
		last_handled = handled;
		if (status == INVALID_STATE || status != new_state)
			return qMakePair(true, handled);
	}
	else
	{
		// We can decide the mode only if we have seen an environment frame previously.
		// If we just get PP frames, we can't decide the mode!
		if (status_requested && status != INVALID_STATE)
		{
			if (mode == NOT_PULL)
			{
				// if the last frame was a definitely handled frame, we can't know anything
				// about the base/advanced state of the device
				if (last_handled == FRAME_MAYBE_HANDLED)
					advanced = status == new_state ? PULL_NOT_ADVANCED : PULL_ADVANCED;
			}
			else if (status == new_state && last_handled == FRAME_HANDLED)
			{
				mode = PULL;
				advanced = PULL_NOT_ADVANCED;
			}
			else if (status != new_state)
			{
				mode = NOT_PULL;
				if (last_handled == FRAME_MAYBE_HANDLED)
					advanced = PULL_ADVANCED;
			}
		}
		else
		{
			status = new_state;
			status_requested = false;
		}
	}

	return qMakePair(false, handled);
}

void PullStateManager::setStatusRequested(bool status)
{
	status_requested = status;
}


PullDevice::PullDevice(QString who, QString where, PullMode m, int openserver_id, int pull_delay, AdvancedMode adv, PullStateManager::FrameChecker checker) :
	device(who, where, openserver_id),
	state(m, adv, checker)
{
	delayed_request.setSingleShot(true);
	delayed_request.setInterval(pull_delay + (*bt_global::config)[TS_NUMBER].toInt() * TS_NUMBER_FRAME_DELAY);
	connect(&delayed_request, SIGNAL(timeout()), SLOT(delayedStatusRequest()));
}


void PullDevice::delayedStatusRequest()
{
	state.setStatusRequested(true);
	requestPullStatus();
}

void PullDevice::manageFrame(OpenMsg &msg)
{
	switch (checkAddressIsForMe(QString::fromStdString(msg.whereFull()), where))
	{
	case NOT_MINE:
		return;
	case GLOBAL:
	case ENVIRONMENT:
		if (!state.isDetectionComplete())
		{
			PullStateManager::CheckResult res = state.moreFrameNeeded(msg, true);
			// we need to delay the status request in order for the
			// state of the device to stabilize
			if (res.first)
				delayed_request.start();
			// when the device is NOT_PULL and the frame is definitely handled, we fall through
			// and let the parsing continue
			if (state.getPullMode() != NOT_PULL || res.second != FRAME_HANDLED)
				return;
		}
		else if (state.getPullMode() == PULL)
			return;
		// when NOT_PULL we must parse the frame and emit valueReceived() signal
		break;
	default:
		if (!state.isDetectionComplete())
			state.moreFrameNeeded(msg, false);
		break;
	}

	DeviceValues values_list;
	parseFrame(msg, values_list);

	// DeviceValues may be empty, avoid emitting a signal in such cases
	if (values_list.size() > 0)
		emit valueReceived(values_list);
}

