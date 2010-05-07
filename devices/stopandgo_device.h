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


enum StatusBits
{
	OPENED              = 0x1000,
	FAULT               = 0x0800,
	BLOCK               = 0x0400,
	OPENED_LE_N         = 0x0200,
	OPENED_GROUND       = 0x0100,
	OPENED_VMAX         = 0x0080,
	AUTOTEST_DISACTIVE  = 0x0040,
	AUTORESET_DISACTIVE = 0x0020,
	TRACKING_DISACTIVE  = 0x0010,
	WAITING_RECLOSE     = 0x0008,
	OPENED_FIRST_DAY    = 0x0004,
	NO_VOLTAGE_OUTPUT   = 0x0002,
	NO_VOLTAGE_INPUT    = 0x0001
};

// Function to verify if the fiven fields are setted in status. Returns true
// if they are setted, false otherwise.
//
// Valid fields are the masks enumerated in StatusBits.
//
// It's possible to combine masks with a bitwise OR to check multiple fields
// at once, in which case the function returns true ONLY IF ALL the fields
// passed are setted in status.
bool getStatusValue(int status, int fields);

class StopAndGoDevice : public device
{
friend class TestStopAndGoDevice;

Q_OBJECT
public:

	enum Type
	{
		DIM_ICM_STATE = 250
	};

	explicit StopAndGoDevice(const QString &where, int openserver_id = 0);

	void sendAutoResetActivation();
	void sendAutoResetDisactivation();

public slots:
	void requestICMState();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};

class StopAndGoPlusDevice : public StopAndGoDevice
{
friend class TestStopAndGoPlusDevice;

Q_OBJECT
public:
	explicit StopAndGoPlusDevice(const QString &where, int openserver_id = 0);

	void sendClose();
	void sendOpen();
	void sendTrackingSystemActivation();
	void sendTrackingSystemDisactivation();
};

class StopAndGoBTestDevice : public StopAndGoDevice
{
friend class TestStopAndGoBTestDevice;

Q_OBJECT
public:

	enum Type
	{
		DIM_AUTOTEST_FREQ = 212
	};

	explicit StopAndGoBTestDevice(const QString &where, int openserver_id = 0);

	void sendDiffSelftestActivation();
	void sendDiffSelftestDisactivation();
	void sendSelftestFreq(int days);

public slots:
	void requestSelftestFreq();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};

#endif // STOPANDGODEVICE_H
