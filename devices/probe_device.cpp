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


#include "probe_device.h"
#include "openmsg.h"
#include "generic_functions.h" // createCommandFrame

#include <QVariant>
#include <QtDebug>

enum what_t
{
	// probe
	INTERNAL_TEMPERATURE = 0,
	FANCOIL_STATUS = 11,
	SETPOINT_ADJUSTED = 12,
	LOCAL_OFFSET = 13,
	SETPOINT_TEMPERATURE = 14,
	EXTERNAL_TEMPERATURE = 15,

	CONDITIONING = 0,
	HEATING = 1,

	// central
	WIN_MANUAL = 110,
	SUM_MANUAL = 210,
	GEN_MANUAL = 310,

	WIN_AUTOMATIC = 111,
	SUM_AUTOMATIC = 211,
	GEN_AUTOMATIC = 311,

	PROT_ANTIFREEZE = 102,
	PROT_THERMAL = 202,
	PROT_GENERIC = 302,

	WIN_OFF = 103,
	SUM_OFF = 203,
	GEN_OFF = 303,
};


// NonControlledProbeDevice implementation

NonControlledProbeDevice::NonControlledProbeDevice(QString where, ProbeType t, int openserver_id)
	: device(QString("4"), where, openserver_id), type(t)
{
}

void NonControlledProbeDevice::requestStatus()
{
	sendRequest(type == EXTERNAL ? "15#1" : "0");
}

void NonControlledProbeDevice::manageFrame(OpenMsg &msg)
{
	if (where.toInt() != msg.where())
		return;

	if (type == EXTERNAL && what_t(msg.what()) == EXTERNAL_TEMPERATURE)
	{
		DeviceValues values_list;

		values_list[DIM_TEMPERATURE] = msg.whatArgN(1);
		emit valueReceived(values_list);
	}
	else if (type == INTERNAL && what_t(msg.what()) == INTERNAL_TEMPERATURE)
	{
		DeviceValues values_list;

		values_list[DIM_TEMPERATURE] = msg.whatArgN(0);
		emit valueReceived(values_list);
	}
	else
		qDebug() << "Unhandled frame" << msg.frame_open;
}


// ControlledProbeDevice implementation

ControlledProbeDevice::ControlledProbeDevice(QString where, QString central, QString _simple_where,
						 CentralType _central_type, ProbeType _type, int openserver_id)
	: device(QString("4"), QString("#") + where, openserver_id)
{
	simple_where = _simple_where;
	type = _type;
	central_type = _central_type;
	has_central_info = false;
	new_request_allowed = true;
	local_offset = 0;
	local_status = ST_NORMAL;
	status = ST_NONE;
	set_point = -1;

	connect(&new_request_timer, SIGNAL(timeout()), SLOT(timeoutElapsed()));
}

void ControlledProbeDevice::setManual(unsigned setpoint)
{
	QString what = QString("#14*%1*3").arg(setpoint, 4, 10, QChar('0'));
	QString frame = "*#" + who + "*" + where + "*" + what + "##";

	set_point = -1;
	sendCompressedFrame(frame);
}

void ControlledProbeDevice::setAutomatic()
{
	set_point = -1;
	sendFrame(createCommandFrame(who, "311", where));
}

void ControlledProbeDevice::setFancoilSpeed(int speed)
{
	if (type != FANCOIL)
		return;

	QString msg = QString("*#") + who + "*" + simple_where + "*#11*" +
		QString::number(speed) + "##";
	sendFrame(msg);
}

void ControlledProbeDevice::requestFancoilStatus()
{
	if (type != FANCOIL)
		return;

	QString msg = QString("*#") + who + "*" + simple_where + "*11##";
	sendInit(msg);
}

void ControlledProbeDevice::requestStatus()
{
	if (central_type == CENTRAL_99ZONES)
		sendRequest(QString());

	sendInit("*#" + who + "*" + simple_where + "##");

	if (type == FANCOIL)
		requestFancoilStatus();
}

void ControlledProbeDevice::manageFrame(OpenMsg &msg)
{
	QString where_full = msg.whereFull().c_str();

	if (central_type == CENTRAL_99ZONES && where_full == "#0" && !has_central_info)
	{
		new_request_allowed = true;
		has_central_info = true;
		sendRequest(QString());

		return;
	}

	if (simple_where.toInt() != msg.where())
		return;

	int what = msg.what();
	DeviceValues values_list;

	qDebug() << "Full where" << where_full << "what" << what;

	switch (what)
	{
	case WIN_MANUAL:
	case SUM_MANUAL:
	case GEN_MANUAL:
		if (local_status != ST_OFF && local_status != ST_PROTECTION)
			values_list[DIM_STATUS] = status = ST_MANUAL;
		if (new_request_allowed)
		{
			sendFrame("*#4*" + where_full.mid(1) + "##");
			new_request_timer.start(TIMEOUT_TIME);
			new_request_allowed = false;
		}
		has_central_info = false;
		break;
	case WIN_AUTOMATIC:
	case SUM_AUTOMATIC:
	case GEN_AUTOMATIC:
		if (local_status != ST_OFF && local_status != ST_PROTECTION)
			values_list[DIM_STATUS] = status = ST_AUTO;
		if (new_request_allowed)
		{
			sendFrame("*#4*" + where_full.mid(1) + "##");
			new_request_timer.start(TIMEOUT_TIME);
			new_request_allowed = false;
		}
		has_central_info = false;
		break;
	case PROT_ANTIFREEZE:
		if (where_full[0] == '#')
			break;
		if (local_status != ST_OFF && local_status != ST_PROTECTION)
			values_list[DIM_STATUS] = status = ST_PROTECTION;
		break;
	case PROT_THERMAL:
		values_list[DIM_STATUS] = status = ST_PROTECTION;
		break;
	case PROT_GENERIC:
		if (local_status != ST_OFF && local_status != ST_PROTECTION)
			values_list[DIM_STATUS] = status = ST_PROTECTION;
		break;
	case WIN_OFF:
	case SUM_OFF:
	case GEN_OFF:
		values_list[DIM_STATUS] = status = ST_OFF;
		break;
	case CONDITIONING:
	case HEATING:
		if (!msg.IsNormalFrame())
			break;
		if (status != ST_MANUAL && status != ST_AUTO)
		{
			values_list[DIM_STATUS] = status = ST_AUTO;
			if (!has_central_info)
			{
				has_central_info = true;
				sendFrame("*#" + who + "*" + where_full);
			}
		}
	}

	if (values_list.count() > 0)
	{
		values_list[DIM_LOCAL_STATUS] = local_status;
		values_list[DIM_OFFSET] = local_offset;
		if (set_point >= 0)
			values_list[DIM_SETPOINT] = set_point;

		emit valueReceived(values_list);
		return;
	}

	if (msg.IsNormalFrame())
		return;

	switch (what)
	{
	case INTERNAL_TEMPERATURE:
		values_list[DIM_TEMPERATURE] = msg.whatArgN(0);
		break;
	case FANCOIL_STATUS:
		values_list[DIM_FANCOIL_STATUS] = msg.whatArgN(0);
		break;
	case SETPOINT_TEMPERATURE:
		values_list[DIM_SETPOINT] = set_point = msg.whatArgN(0);
		break;
	case SETPOINT_ADJUSTED:
		if (local_status == ST_NORMAL)
			values_list[DIM_SETPOINT] = set_point = msg.whatArgN(0) - local_offset * 10;
		break;
	case LOCAL_OFFSET:
		int st = msg.whatArgN(0);

		if (st == 4)
		{
			values_list[DIM_LOCAL_STATUS] = local_status = ST_OFF;
		}
		else if (st == 5)
		{
			values_list[DIM_LOCAL_STATUS] = local_status = ST_PROTECTION;
		}
		else
		{
			// 0, 1, 2, 3 => 0, 1, 2, 3 degrees offset
			// 11, 12, 13 => -1, -2, -3 degrees offset
			if (st <= 3)
				local_offset = st;
			else
				local_offset = -st + 10;
			values_list[DIM_OFFSET] = local_offset;
			values_list[DIM_LOCAL_STATUS] = local_status = ST_NORMAL;
		}

		break;
	};

	if (values_list.count() > 0)
	{
		values_list[DIM_LOCAL_STATUS] = local_status;
		values_list[DIM_OFFSET] = local_offset;
		if (set_point >= 0)
			values_list[DIM_SETPOINT] = set_point;
		values_list[DIM_STATUS] = status;

		emit valueReceived(values_list);
	}
}

void ControlledProbeDevice::timeoutElapsed()
{
	new_request_allowed = true;
	new_request_timer.stop();
}
