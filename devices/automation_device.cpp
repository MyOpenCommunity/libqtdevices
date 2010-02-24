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


#include "automation_device.h"

#include <openmsg.h>

#include <QDebug>
#include <QVariant>


enum RequestDimension
{
	REQ_STATUS_OPEN = 32,
	REQ_STATUS_CLOSE = 31
};


AutomationDevice::AutomationDevice(QString where, PullMode m) :
	PullDevice(QString("2"), where, m)
{
}

void AutomationDevice::goUp()
{
	sendCommand(QString::number(DIM_UP));
}

void AutomationDevice::goDown()
{
	sendCommand(QString::number(DIM_DOWN));
}

void AutomationDevice::stop()
{
	sendCommand(QString::number(DIM_STOP));
}

void AutomationDevice::requestStatus()
{
	sendRequest(QString());
}

void AutomationDevice::requestPullStatus()
{
	requestStatus();
}

void AutomationDevice::parseFrame(OpenMsg &msg, StatusList *sl)
{
	QVariant v;
	int what = msg.what();
	switch (what)
	{
	case DIM_UP:
		v.setValue(true);
		break;
	case DIM_DOWN:
		v.setValue(true);
		break;
	case DIM_STOP:
		v.setValue(true);
		break;
	}
	(*sl)[msg.what()] = v;
}


PPTStatDevice::PPTStatDevice(QString address) : device(QString("25"), address)
{
}

void PPTStatDevice::requestStatus() const
{
	sendRequest(QString());
}

void PPTStatDevice::manageFrame(OpenMsg &msg)
{
	if (where.toInt() != msg.where())
		return;

	int what = msg.what();

	if (what == REQ_STATUS_OPEN || what == REQ_STATUS_CLOSE)
	{
		StatusList status_list;
		status_list[DIM_STATUS] = QVariant(what == REQ_STATUS_CLOSE);
		emit status_changed(status_list);
	}
}

