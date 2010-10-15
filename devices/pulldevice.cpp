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


/*!
	\public
	\enum AddressType
	Type of address
 */
/*!
	\var AddressType NOT_MINE
	The given address does not match the device address.
 */
/*!
	\var AddressType ENVIRONMENT
	The given address is an environment address in the same area of the device address and has the same level.
 */
/*!
	\var AddressType GLOBAL
	The given address is a global address (either global to all levels or global to the level of the device).
 */
/*!
	\var AddressType P2P
	The given address is equal to the device address.
 */


/*!
	\enum PullMode
	\see PullDevice for a description.
 */
/*!
	\var PullMode PULL
	For pull devices.
 */
/*!
	\var PullMode NOT_PULL
	For non-pull devices.
 */


/*!
	\enum AdvancedMode
	\see PullDevice for a description.
 */
/*!
	\var AdvancedMode PULL_ADVANCED_UNKNOWN
	Basic/advanced mode of the physical device not known.
 */
/*!
	\var AdvancedMode PULL_ADVANCED
	Physical device is advanced (interprets some of the commands more advanced devices).
 */
/*!
	\var AdvancedMode PULL_NOT_ADVANCED
	Physical device is basic (only interprets commands for its device type).
 */


/*!
	\enum FrameHandled
 */
/*!
	\var FrameHandled FRAME_NOT_HANDLED
	The physical device never handles this type of frames.
 */
/*!
	\var FrameHandled FRAME_HANDLED
	The physical device always handles this type of frame.
 */
/*!
	\var FrameHandled FRAME_MAYBE_HANDLED
	Only advanced devices handle this type of frame.
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

/**
	\brief Check if the address found in a frame is valid for the device.

	Frame address must be environment or general. Addresses must be complete, ie
	must have area+point and extension part (if any).
 */
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

/**
	\class PullStateManager
	\brief Implements the algorithm for basic/advanced actuators.

	\see \a PullDevice for the details for the basic/advanced detection algorithm.
 */

/*!
	\typedef PullStateManager::FrameChecker
	Acts as a filter for frames; the return value can be:

	\li FRAME_NOT_HANDLED: the pull/not pull algorithm is not run for this frame
	\li FRAME_HANDLED: the frame is considered for the pull/non pull algorithm
	\li FRAME_MAYBE_HANDLED: the device might react to the frame (in this case
	it is in non pull mode, and an "advanced" device) or
	it might not react (in this case the pull mode is unknown)
 */

/*!
	\typedef PullStateManager::CheckResult

	The first item of the pair is \c true if \a PullDevice must send a status request to the physical device.

	The second item of the pair determines wether the device must process the frame or not.
 */

/*!
	\brief Constructor

	If \a checker is \a NULL, all frames are treated as handled by the device.
 */
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

/*!
	\brief Returns the pull mode for the device.
 */
PullMode PullStateManager::getPullMode()
{
	return mode;
}

/*!
	\brief Executes the basic/advanced detection algorithm
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

/*!
	\brief Called by \a PullDevice after sending the status request frame
 */
void PullStateManager::setStatusRequested(bool status)
{
	status_requested = status;
}

/*!
	\fn AdvancedMode PullStateManager::getAdvanced()
	\brief Returns the basic/advanced mode of the physical device.
 */

/*!
	\fn bool PullStateManager::isDetectionComplete()
	\brief Returns wether the \a PullStateManager needs to process more frames.
 */



/**
	\class PullDevice
	\brief A base class for lighting and automation devices.

	This class provides some common functionality needed for all pull/non-pull devices;
	it is not meant to be used directly and adds no public functions.

	Derived classes must reimplement:
	\list
	\li \a parseFrame(): called for every received frame. Derived classes must parse the frame and put the
	results into the DeviceValues.
	\li \a requestPullStatus(): send a status request to the device. This can be reimplemented depending on device
	necessities.

	Derived classes MUST NOT reimplement \a manageFrame(), as they will override ADVANCED mode discovery logic

	\section1 Pull/non-pull devices

	All physical devices react to commands sent directly to the device (point-to-point commands) by executing
	the command and sending a status update.  Non-pull devices also react to global and environment commands
	by executing the command, but without sending back a status update.  Pull devices ignore all global and
	environment commands.

	Since physical non-pull device do not send status update frames for global/environment commands, the device class
	must interpret these commands and emit the required \a valueReceived() signals.

	Since pull/non-pull device type is only used when interpreting frames, this information is not specified in the configuration
	file for devices only used to send frames (for example light/dimmer groups).

	\section1 Basic/advanced devices

	Light actuators and 10-level dimmers can come both basic and advanced variants; basic devices only
	react to command frames for their exact device type while advanced devices react to some frames directed
	to more specialized devices.  For example a basic light actuator will not react to a dimmer 100 on frame while
	an advances light actuator will turn on.

	This is not a problem for point-to-point frames (the device will send back a status update anyway) but for global/environment
	frames this information is critical to keep device state synchronized with the physical device state; to complicate matters
	further, this information is not included in the configuration file but must be inferred from the way the device reacts to frames.

	\section1 Determining basic/advanced device type

	This is only relevant for pull devices; non-pull devices will always be created as basic (since pull physical devices do not react
	to global/environment frames, there is no need to know the basic/advanced type).  Also, once this algorithm runs to completion and
	the device is determined to be either \a PULL_ADVANCED or \a PULL_NOT_ADVANCED, received frames are interpreted as in all other devices.

	This algorithm is implemented in the \a PullStateManager.

	The device starts in \a PULL_ADVANCED_UNKNOWN mode, and the physical device state is unknown.  A status request is sent at startup to
	read initial device state.

	When receiving a status update from the device, the physical device state is stored in the device instance.

	When receiving a global/environment command, there can be 3 cases:
	\list
	\li the frame is never handled by the physical device
	\li the frame is always handled by the physical device
	\li the frame is not handled by basic devices, but is handled by advanced ones

	In the first case the frame is discarded, in the second is passed \a parseFrame().

	In the third case the device sends a status request to the physical device in order to determine if the
	device reacted to the frame or not.  Note that status requests are delayed from 1.5 to 9 seconds depending on device type.

	When receiving the requested status update, if the previous device state was known and the last received
	command was one of the frames only handled by advanced physical devices and the device changed its state then the device
	is a \a PULL_ADVANCED device, otherwise a \a PULL_NOT_ADVANCED device.  Note that this is only performed if the status
	update was requested by the device; if a non-requested status update arrives the basic/advanced device type is not changed.
 */

/*!
	\brief Constructor
	\param who passed to device constructor
	\param where passed to device constructor
	\param m pull mode of the device
	\param openserver_id passed to device constructor
	\param pull_delay delay used for status request in basic/advanced detection
	\param adv basic/advanced mode (normally PULL_ADVANCED_UNKNOWN)
	\param checker used to classify incoming frames during basic/advanced detection
 */
PullDevice::PullDevice(QString who, QString where, PullMode m, int openserver_id, int pull_delay, AdvancedMode adv, PullStateManager::FrameChecker checker) :
	device(who, where, openserver_id),
	state(m, adv, checker)
{
	delayed_request.setSingleShot(true);
	delayed_request.setInterval(pull_delay + (*bt_global::config)[TS_NUMBER].toInt() * TS_NUMBER_FRAME_DELAY);
	connect(&delayed_request, SIGNAL(timeout()), SLOT(delayedStatusRequest()));
}

/*!
	\brief Sends the delayed status request and notifies the \a PullStateManager that the request was sent
 */
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

/*!
	\fn void PullDevice::requestPullStatus()
	\brief Send a status request frame for the basic/advanced detection algorithm.
 */

/*!
	\fn bool PullDevice::isAdvanced()
	\brief Return \a true if basic/advanced detection algoritm completed and the device is advanced
 */
