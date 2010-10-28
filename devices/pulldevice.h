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


#ifndef PULLDEVICE_H
#define PULLDEVICE_H

#include "device.h"

class QString;
class OpenMsg;


/*!
	\ingroup Core
	\brief Type of address
*/
enum AddressType
{
	NOT_MINE = 0,   /*!< The given address does not match the device address. */
	ENVIRONMENT,    /*!< The given address is an environment address in the
                         same area of the device address and has the same level. */
	GLOBAL,         /*!< The given address is a global address (either global to
                         all levels or global to the level of the device). */
	P2P,            /*!< The given address is equal to the device address. */
};

/*!
	\ingroup Core
	\sa PullDevice for a description.
*/
enum PullMode
{
	PULL,           /*!< For pull devices. */
	NOT_PULL,       /*!< For non pull devices */
	PULL_UNKNOWN,
};


/*!
	\ingroup Core
	\sa PullDevice for a description.
*/
enum AdvancedMode
{
	PULL_ADVANCED_UNKNOWN,  /*!< Basic/advanced mode of the physical device not known. */
	PULL_ADVANCED,          /*!< Physical device is advanced (interprets some of the
								 commands more advanced devices). */
	PULL_NOT_ADVANCED,      /*!< Physical device is basic (only interprets commands
								 for its device type). */
};


/*!
	\ingroup Core
*/
enum FrameHandled
{
	FRAME_NOT_HANDLED,   /*!< The physical device never handles this type of frames. */
	FRAME_HANDLED,       /*!< The physical device always handles this type of frame. */
	FRAME_MAYBE_HANDLED, /*!< Only advanced devices handle this type of frame. */
};

/*!
	\ingroup Core
	\brief Check if the address found in a frame is valid for the device.

	Frame address must be environment or general. Addresses must be complete, ie
	must have area+point and extension part (if any).
*/
AddressType checkAddressIsForMe(const QString &msg_where, const QString &dev_where);


/*!
	\ingroup Core
	\brief Implements the algorithm for basic/advanced actuators.

	\sa PullDevice for the details for the basic/advanced detection algorithm.
*/
class PullStateManager
{
friend class TestLightingDevice;
friend class TestPullManager;
public:

	/*!
		Acts as a filter for frames; the return value can be:

		\li FRAME_NOT_HANDLED: the pull/not pull algorithm is not run for this frame;
		\li FRAME_HANDLED: the frame is considered for the pull/non pull algorithm;
		\li FRAME_MAYBE_HANDLED: the device might react to the frame (in this case
		it is in non pull mode, and an "advanced" device) or it might not react
		(in this case the pull mode is unknown).
	*/
	typedef FrameHandled (*FrameChecker)(OpenMsg &msg);

	/*!
		The first item of the pair is \c true if PullDevice must send a status
		request to the physical device.

		The second item of the pair determines wether the device must process
		the frame or not.
	*/
	typedef QPair<bool, FrameHandled> CheckResult;

	/*!
		\brief Constructor

		If \a checker is NULL, all frames are treated as handled by the device.
	*/
	PullStateManager(PullMode m, AdvancedMode adv = PULL_ADVANCED_UNKNOWN, FrameChecker checker = NULL);

	/*!
		\brief Executes the basic/advanced detection algorithm.
	*/
	CheckResult moreFrameNeeded(OpenMsg &msg, bool is_environment);

	/*!
		\brief Returns the pull mode for the device.
	*/
	PullMode getPullMode();

	/*!
		\brief Called by PullDevice after sending the status request frame.
	*/
	void setStatusRequested(bool status);

	/*!
		\brief Returns the basic/advanced mode of the physical device.
	*/
	AdvancedMode getAdvanced() { return advanced; }

	/*!
		\brief Returns wether the PullStateManager needs to process more frames.
	*/
	bool isDetectionComplete() { return mode != PULL_UNKNOWN && advanced != PULL_ADVANCED_UNKNOWN; }

private:
	int status;
	bool status_requested;
	PullMode mode;
	AdvancedMode advanced;

	// filters the frames interpreted by this device
	FrameChecker frame_checker;
	// the handled/maybe handled status of the last environment frame processed
	FrameHandled last_handled;
};


/*!
	\ingroup Core
	\brief A base class for lighting and automation devices.

	This class provides some common functionality needed for all pull/non-pull devices;
	it is not meant to be used directly and adds no public functions.

	Derived classes must reimplement:
	\li parseFrame(): called for every received frame. Derived classes must parse
	the frame and put the results into the DeviceValues.
	\li requestPullStatus(): send a status request to the device. This can be
	reimplemented depending on device necessities.

	Derived classes MUST NOT reimplement manageFrame(), as they will override
	ADVANCED mode discovery logic.

	\section1 Pull/non-pull devices

	All physical devices react to commands sent directly to the device
	(point-to-point commands) by executing the command and sending a status update.
	Non-pull devices also react to global and environment commands by executing
	the command, but without sending back a status update.  Pull devices ignore
	all global and environment commands.

	Since physical non-pull device do not send status update frames for
	global/environment commands, the device class must interpret these commands
	and emit the required valueReceived() signals.

	Since pull/non-pull device type is only used when interpreting frames, this
	information is not specified in the configuration file for devices only used
	to send frames (for example light/dimmer groups).

	\section1 Basic/advanced devices

	Light actuators and 10-level dimmers can come both basic and advanced variants;
	basic devices only react to command frames for their exact device type while
	advanced devices react to some frames directed to more specialized devices.
	For example a basic light actuator will not react to a dimmer 100 on frame while
	an advances light actuator will turn on.

	This is not a problem for point-to-point frames (the device will send back a
	status update anyway) but for global/environment frames this information is
	critical to keep device state synchronized with the physical device state;
	to complicate matters further, this information is not included in the
	configuration file but must be inferred from the way the device reacts to
	frames.

	\section1 Determining basic/advanced device type

	This is only relevant for pull devices; non-pull devices will always be
	created as basic (since pull physical devices do not react to global/environment
	frames, there is no need to know the basic/advanced type). Also, once this
	algorithm runs to completion and the device is determined to be either
	PULL_ADVANCED or PULL_NOT_ADVANCED, received frames are interpreted as in
	all other devices.

	This algorithm is implemented in the PullStateManager.

	The device starts in PULL_ADVANCED_UNKNOWN mode, and the physical device
	state is unknown.  A status request is sent at startup to read initial device
	state.

	When receiving a status update from the device, the physical device state is
	stored in the device instance.

	When receiving a global/environment command, there can be 3 cases:
	\li the frame is never handled by the physical device;
	\li the frame is always handled by the physical device;
	\li the frame is not handled by basic devices, but is handled by advanced ones.

	In the first case the frame is discarded, in the second is passed parseFrame().

	In the third case the device sends a status request to the physical device
	in order to determine if the device reacted to the frame or not. Note that
	status requests are delayed from 1.5 to 9 seconds depending on device type.

	When receiving the requested status update, if the previous device state was
	known and the last received command was one of the frames only handled by
	advanced physical devices and the device changed its state then the device
	is a PULL_ADVANCED device, otherwise a PULL_NOT_ADVANCED device. Note that
	this is only performed if the status update was requested by the device; if
	a non-requested status update arrives the basic/advanced device type is not changed.
*/
class PullDevice : public device
{
friend class TestLightingDevice;
friend class TestDimmerDevice;
Q_OBJECT
public:
	virtual void manageFrame(OpenMsg &msg);

protected:
	/*!
		\brief Constructor

		Constructs a PullDevice of type \a m for the given \a who \a where
		using \a pull_delay as a delay for status requests in basic/advanced
		detection, \a sdv as the basic/advanced mode and \a checker to classify
		incoming frames during detection.
	 */
	PullDevice(QString who, QString where, PullMode m, int openserver_id, int pull_delay, AdvancedMode adv = PULL_ADVANCED_UNKNOWN, PullStateManager::FrameChecker checker = NULL);

	/*!
		\brief Send a status request frame for the basic/advanced detection algorithm.
		\internal Different devices may need different status requests (eg. Dimmer100).
	*/
	virtual void requestPullStatus() = 0;

	/*!
		\brief Return true if basic/advanced detection algoritm completed and the
		device is advanced.
	*/
	bool isAdvanced() { return state.getAdvanced() == PULL_ADVANCED; }

private slots:
	/*!
		\brief Sends the delayed status request and notifies the PullStateManager
		that the request was sent.
	*/
	void delayedStatusRequest();

private:
	PullStateManager state;
	QTimer delayed_request;
};
#endif // PULLDEVICE_H

/*! \file */ /* otherwise doxygen does not generate documentation for enums */
