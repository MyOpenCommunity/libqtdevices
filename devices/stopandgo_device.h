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


class StopAndGoDevice : public device
{
friend class TestStopAndGoDevice;

Q_OBJECT
public:

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
