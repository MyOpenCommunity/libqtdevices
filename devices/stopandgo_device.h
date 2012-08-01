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
#ifndef STOPANDGODEVICE_H
#define STOPANDGODEVICE_H

#include "device.h"

/*!
	\ingroup StopAndGo
	\brief Device to manage the Stop&Go system.

	You can use this device to retrieve the state of the systems check module and
	activate or disactivate the automatic reset.

	To request the state of the system check module you can use the
	requestICMState() method, or its compressed version (for info about
	compressed frames take a look at FrameCompressor class)
	requestCompressedICMState().

	After every command, you must send a state request to verify and syncronize
	all the BTouch devices.

	\section StopAndGoDevice-dimensions Dimensions
	\startdim
	\dim{DIM_OPENED,bool,,Opened}
	\dim{DIM_FAULT,bool,,Fault}
	\dim{DIM_LOCKED,bool,,Lock}
	\dim{DIM_OPENED_LE_N,bool,,Opened for short circuit between Le N}
	\dim{DIM_OPENED_GROUND,bool,,Opened for fault to ground}
	\dim{DIM_OPENED_VMAX,bool,,Opened for maximum voltage}
	\dim{DIM_AUTOTEST_DISACTIVE,bool,,Self test disactive}
	\dim{DIM_AUTORESET_DISACTIVE,bool,,Automatic test disactive}
	\dim{DIM_TRACKING_DISACTIVE,bool,,Tracking disactive}
	\dim{DIM_WAITING_RECLOSE,bool,,Waiting for the reclose}
	\dim{DIM_OPENED_FIRST_DAY,bool,,First 24 hours since opening}
	\dim{DIM_NO_VOLTAGE_OUTPUT,bool,,Output tension leak}
	\dim{DIM_NO_VOLTAGE_INPUT,bool,,Input tension leak}
	\enddim

	\sa \ref device-dimensions
*/
class StopAndGoDevice : public device
{
friend class TestStopAndGoDevice;

Q_OBJECT
public:
	/*!
		\refdim{StopAndGoDevice}
	*/
	enum Type
	{
		DIM_OPENED = 0,
		DIM_FAULT,
		DIM_LOCKED,
		DIM_OPENED_LE_N,
		DIM_OPENED_GROUND,
		DIM_OPENED_VMAX,
		DIM_AUTOTEST_DISACTIVE,
		DIM_AUTORESET_DISACTIVE,
		DIM_TRACKING_DISACTIVE,
		DIM_WAITING_RECLOSE,
		DIM_OPENED_FIRST_DAY,
		DIM_NO_VOLTAGE_OUTPUT,
		DIM_NO_VOLTAGE_INPUT
	};

	/*!
		\brief Constructor

		Creates a new StopAndGoDevice with the given \a where and
		\a openserver_id.
	*/
	explicit StopAndGoDevice(const QString &where, int openserver_id = 0);

	virtual void init();

	/*!
		\brief Activates the automatic reset.
	*/
	void sendAutoResetActivation();

	/*!
		\brief Disactivates the automatic reset.
	*/
	void sendAutoResetDisactivation();

public slots:
	/*!
		\brief Requests the state of the systems check module.
	*/
	void requestICMState();
	/*!
		\brief Request the state fo the system check module (compressed version).
	*/
	void requestCompressedICMState();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};


/*!
	\ingroup StopAndGo
	\brief Device to manage the Stop&Go Plus system.

	This device extends the StopAndGoDevice permitting to open or close
	(sendOpen and sendClose() methods) the
	system and to activate or disactivate (sendTrackingSystemActivation() and
	sendTrackingSystemDisactivation()) the system tracking.

	\sa StopAndGoDevice
*/
class StopAndGoPlusDevice : public StopAndGoDevice
{
Q_OBJECT
public:
	/*!
		\brief Constructor

		Creates a new StopAndGoPlusDevice with the given \a where and
		\a openserver_id.
	*/
	explicit StopAndGoPlusDevice(const QString &where, int openserver_id = 0);

	/*!
		\brief Activates the system tracking.
	*/
	void sendTrackingSystemActivation();
	/*!
		\brief Disactivate the system tracking.
	*/
	void sendTrackingSystemDisactivation();

public slots:
	/*!
		\brief Closes the system.
	*/
	void sendClose();
	/*!
		\brief Opens the system.
	*/
	void sendOpen();
};

/*!
	\ingroup StopAndGo
	\brief Device to manage the Stop&Go BTest system.

	This device extends the StopAndGoDevice permitting to enable or disable
	(sendDiffSelftestActivation() and sendDiffSelftestDisactivation()) the
	automatic test of the differential and to set the frequency of the test to
	be performed at (sendSelftestFreq()).

	\section StopAndGoBTestDevice-dimensions Dimensions
	In addition to the StopAndGoDevice dimensions the StopAndGoPlusDevice has:
	\startdim
	\dim{DIM_AUTOTEST_FREQ,int,1-180,The frequency in days the automatic test fo
	the differential is perferomed at}
	\enddim

	\sa \ref device-dimensions, StopAndGoDevice
*/
class StopAndGoBTestDevice : public StopAndGoDevice
{
friend class TestStopAndGoBTestDevice;

Q_OBJECT
public:
	/*!
		\refdim{StopAndGoBTestDevice}
	*/
	enum Type
	{
		DIM_AUTOTEST_FREQ = 212
	};

	static const int SELF_TEST_FREQ_MIN = 1;
	static const int SELF_TEST_FREQ_MAX = 180;

	/*!
		\brief Constructor

		Creates a new StopAndGoBTestDevice with the given \a where and
		\a openserver_id.
	*/
	explicit StopAndGoBTestDevice(const QString &where, int openserver_id = 0);

	virtual void init();

	/*!
		\brief Activates the automatic test of the differential.
	*/
	void sendDiffSelftestActivation();
	/*!
		\brief Disactivates the automatic test of the differential.
	*/
	void sendDiffSelftestDisactivation();

public slots:
	/*!
		\brief Sets the frequency in days the automatic test for the differential
		is performed at.

		\note Valid values are between 1 and 180.
	*/
	void sendSelftestFreq(int days);
	/*!
		\brief Requests the frequency in days the automatic test for the
		differential is performed at.
	*/
	void requestSelftestFreq();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};

#endif // STOPANDGODEVICE_H
