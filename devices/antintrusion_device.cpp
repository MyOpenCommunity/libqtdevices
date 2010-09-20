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


#include "antintrusion_device.h"
#include "frame_functions.h"

#include <openmsg.h>

#include <QDebug>


enum RequestDimension
{
	REQ_TOGGLE_ACTIVATION = 36,
	REQ_PARTIALIZATION = 50,
	DIM_SYSTEM_ON = 8,
	DIM_SYSTEM_OFF = 9,
};

#define WHERE_CENTRAL "0"


namespace
{
	inline int zoneNumber(std::string where)
	{
		return QString::fromStdString(where).mid(1).toInt();
	}
}

AntintrusionDevice::AntintrusionDevice(int openserver_id) : device("5", WHERE_CENTRAL, openserver_id)
{
	for (int i = 0; i < NUM_ZONES; ++i)
		zones[i] = false;
}

void AntintrusionDevice::toggleActivation(const QString &password)
{
	sendCommand(QString::number(REQ_TOGGLE_ACTIVATION) + "#" + password);
}

void AntintrusionDevice::setPartialization(const QString &password)
{
	QString zones_mask;
	for (int i = 0; i < NUM_ZONES; ++i)
		zones_mask += zones[i] ? "1" : "0";

	sendCommand(QString::number(REQ_PARTIALIZATION) + "#" + password + "#" + zones_mask);
}

void AntintrusionDevice::partializeZone(int num_zone, bool partialize)
{
	if (num_zone > 0 && num_zone <= NUM_ZONES)
		zones[num_zone - 1] = partialize;
	else
		qWarning() << "Zone number " << num_zone << "out of range";
}

void AntintrusionDevice::requestStatus()
{
	sendRequest(QString());
}

bool AntintrusionDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (!isCommandFrame(msg))
		return false;

	int what = msg.what();

	if (what == DIM_SYSTEM_ON || what == DIM_SYSTEM_OFF)
		values_list[DIM_SYSTEM_INSERTED] = (what == DIM_SYSTEM_ON);
	else if (what == DIM_ZONE_PARTIALIZED || what == DIM_ZONE_INSERTED)
	{
		int zone = zoneNumber(msg.whereFull());
		if (zone >= 1 && zone <= NUM_ZONES)
			values_list[what] = zone;
	}
	else // alarms
	{
		int zone = zoneNumber(msg.whereFull());
		if ((what == DIM_ANTIPANIC_ALARM && zone == 9) || // the antipanic can arrive only on the 9th zone
			(what == DIM_INTRUSION_ALARM && zone >= 1 && zone <= NUM_ZONES) || // normal zones
			(what == DIM_MANOMISSION_ALARM && zone >= 0 && zone < 16) || // zones and special zones
			((what == DIM_TECHNICAL_ALARM || what == DIM_RESET_TECHNICAL_ALARM) && zone >= 1 && zone < 16)) // auxiliaries
			values_list[what] = zone;
	}

	if (values_list.isEmpty())
		return false;

	return true;
}
