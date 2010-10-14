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
#include "frame_functions.h" // isStatusRequestFrame

#include <openmsg.h>

#include <QDebug>
#include <QVariant>


enum RequestDimension
{
	REQ_STATUS_OPEN = 32,
	REQ_STATUS_CLOSE = 31
};


/*!
	\class AutomationDevice
	\brief A device for managing automation actuators.

	The metods allow to move up, move down and stop an automation actuator.

	After sending the up/down commands, the device keeps on moving until it
	reaches the full up/down state or a stop command is received.

	\section dimensions Dimensions
	\startdim
	\dim{DIM_STOP,no value,,The actuator stopped.}
	\dim{DIM_UP,no value,,The actuator started moving upwards.}
	\dim{DIM_DOWN,no value,,The actuator started moving downwards.}
	\enddim
*/

/*!
	\brief Constructor
 */
AutomationDevice::AutomationDevice(QString where, PullMode m, int openserver_id, int pull_delay) :
	PullDevice(QString("2"), where, m, openserver_id, pull_delay)
{
}

void AutomationDevice::init()
{
	requestStatus();
}

/*!
	\brief Start moving the actuator upwards.
 */
void AutomationDevice::goUp()
{
	sendCommand(DIM_UP);
}

/*!
	\brief Start moving the actuator downwards.
 */
void AutomationDevice::goDown()
{
	sendCommand(DIM_DOWN);
}

/*!
	\brief Stops the actuator.
 */
void AutomationDevice::stop()
{
	sendCommand(DIM_STOP);
}

/*!
	\brief Requests a status update.

	It should never be necessary to call this function explicitly.
 */
void AutomationDevice::requestStatus()
{
	sendRequest(QString());
}

void AutomationDevice::requestPullStatus()
{
	requestStatus();
}

bool AutomationDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	int what = msg.what();

	if (what == DIM_UP || what == DIM_DOWN || what == DIM_STOP)
	{
		values_list[what] = true;
		return true;
	}
	return false;
}


/*!
	\class PPTStatDevice
	\brief Actuator status device (TODO).

	Allows reading the actuator status (TODO).

	\section dimensions Dimensions
	\startdim
	\dim{DIM_STATUS,bool,,The actuator (TODO) status.}
	\enddim
*/

PPTStatDevice::PPTStatDevice(QString address, int openserver_id) :
	device(QString("25"), address, openserver_id)
{
}

void PPTStatDevice::init()
{
	requestStatus();
}

/*!
	\brief Requests a status update.

	It should never be necessary to call this function explicitly.
 */
void PPTStatDevice::requestStatus() const
{
	sendRequest(QString());
}

bool PPTStatDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{
	if (where.toInt() != msg.where())
		return false;

	// In some cases (when more than a ts is present in the system)
	// a request frame can arrive from the monitor socket. We have to manage this
	// situation.
	if (isStatusRequestFrame(msg))
		return false;

	int what = msg.what();

	if (what == REQ_STATUS_OPEN || what == REQ_STATUS_CLOSE)
	{
		values_list[DIM_STATUS] = what == REQ_STATUS_CLOSE;
		return true;
	}

	return false;
}

