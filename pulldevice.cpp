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

#include <openmsg.h>

#include <QPair>
#include <QDebug>
#include <QString>

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

PullStateManager::PullStateManager(PullMode m)
{
	mode = m;
	status = INVALID_STATE;
	status_requested = false;
}

PullMode PullStateManager::getPullMode()
{
	return mode;
}

/*
 * When ignoring the frame, return false so that the calling code doesn't generate useless
 * traffic on the bus.
 */
bool PullStateManager::moreFrameNeeded(OpenMsg &msg, bool is_environment)
{
	// PullStateManager will be used for automation and lighting only.
	// I'll handle all 'what' combinations here, split to a different function or class when needed
	// We need to look for write environment commands
	int new_state = msg.what();
	bool measure_frame = (is_environment && msg.IsWriteFrame()) || (!is_environment && msg.IsMeasureFrame());
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
		return false;

	if (is_environment)
	{
		if (status == INVALID_STATE || status != new_state)
			return true;
	}
	else
	{
		// We can decide the mode only if we have seen an environment frame previously.
		// If we just get PP frames, we can't decide the mode!
		if (status_requested && status != INVALID_STATE)
		{
			if (status == new_state)
				mode = PULL;
			else
				mode = NOT_PULL;
		}
		else
		{
			status = new_state;
			status_requested = false;
		}
	}

	return false;
}

void PullStateManager::setStatusRequested(bool status)
{
	status_requested = status;
}


PullDevice::PullDevice(QString who, QString where, PullMode m, int pull_delay) :
	device(who, where),
	state(m)
{
	delayed_request.setSingleShot(true);
	delayed_request.setInterval(pull_delay);
	connect(&delayed_request, SIGNAL(timeout()), SLOT(delayedStatusRequest()));
}

void PullDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));
	if (who.toInt() == msg.who())
		manageFrame(msg);
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
		if (state.getPullMode() == PULL_UNKNOWN)
		{
			// we need to delay the status request in order for the
			// state of the device to stabilize
			if (state.moreFrameNeeded(msg, true))
				delayed_request.start();
			return;
		}
		else if (state.getPullMode() == PULL)
			return;
		// when NOT_PULL we must parse the frame and emit status_changed() signal
		break;
	default:
		if (state.getPullMode() == PULL_UNKNOWN)
			state.moreFrameNeeded(msg, false);
		break;
	}

	StatusList sl;
	parseFrame(msg, &sl);

	// StatusList may be empty, avoid emitting a signal in such cases
	if (sl.size() > 0)
		emit status_changed(sl);
}

