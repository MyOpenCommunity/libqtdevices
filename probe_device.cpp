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

#include <QtDebug>

enum what_t
{
	INTERNAL_TEMPERATURE = 0,
	EXTERNAL_TEMPERATURE = 15
};

NonControlledProbeDevice::NonControlledProbeDevice(QString where, ProbeType t)
	: device(QString("4"), where),
	type(t)
{
}

void NonControlledProbeDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));
	if (who.toInt() == msg.who())
		manageFrame(msg);
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
		StatusList sl;

		sl[DIM_TEMPERATURE] = msg.whatArgN(1);
		emit status_changed(sl);
	}
	else if (type == INTERNAL && what_t(msg.what()) == INTERNAL_TEMPERATURE)
	{
		StatusList sl;

		sl[DIM_TEMPERATURE] = msg.whatArgN(0);
		emit status_changed(sl);
	}
	else
		qDebug() << "Unhandled frame" << msg.frame_open;
}
