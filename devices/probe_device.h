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


#ifndef PROBE_DEVICE_H
#define PROBE_DEVICE_H

#include "device.h"


class NonControlledProbeDevice : public device
{
Q_OBJECT
public:
	enum ProbeType
	{
		INTERNAL = 0,
		EXTERNAL = 1
	};

	enum Type
	{
		DIM_TEMPERATURE = 1,
	};

	NonControlledProbeDevice(QString where, ProbeType type, int openserver_id = 0);

	void requestStatus();
	virtual void init() { requestStatus(); }

protected:
	virtual void manageFrame(OpenMsg &msg);

private:
	ProbeType type;
};


class ControlledProbeDevice : public device
{
Q_OBJECT
public:
	enum ProbeType
	{
		NORMAL = 0,
		FANCOIL = 1,
	};

	enum CentralType
	{
		CENTRAL_99ZONES = 0,
		CENTRAL_4ZONES = 1,
	};

	enum Type
	{
		DIM_FANCOIL_STATUS = 1,
		DIM_TEMPERATURE = 2,
		DIM_STATUS = 3,
		DIM_SETPOINT = 4,
		DIM_OFFSET = 5,
		DIM_LOCAL_STATUS = 6,
	};

	enum ProbeStatus
	{
		ST_NONE = 0,
		ST_NORMAL = 0,
		ST_MANUAL = 1,
		ST_AUTO = 2,
		ST_OFF = 3,
		ST_PROTECTION = 4,
	};

	ControlledProbeDevice(QString where, QString central, QString simple_where, CentralType central_type, ProbeType type, int openserver_id = 0);

	virtual void init() { requestStatus(); }
	void setManual(unsigned setpoint);
	void setAutomatic();

	void setFancoilSpeed(int speed);
	void requestFancoilStatus();

	void requestStatus();

protected:
	virtual void manageFrame(OpenMsg &msg);

private slots:
	void timeoutElapsed();

private:
	QString simple_where;
	ProbeType type;
	CentralType central_type;
	bool has_central_info, new_request_allowed;
	QTimer new_request_timer;

	// device state
	ProbeStatus status;
	ProbeStatus local_status;
	int local_offset, set_point;

private:
	static const int TIMEOUT_TIME = 10000;
};

#endif // PROBE_DEVICE_H
