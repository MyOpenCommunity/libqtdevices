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


#ifndef DEV_AUTOMATION_H
#define DEV_AUTOMATION_H

#include "pulldevice.h"

class OpenMsg;

#define PULL_DELAY_AUTOMATION   6000


/*!
	\ingroup Automation
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
class AutomationDevice : public PullDevice
{
friend class TestAutomationDevice;
public:
	enum Type
	{
		DIM_STOP = 0,
		DIM_UP = 1,
		DIM_DOWN = 2,
	};

	AutomationDevice(QString where, PullMode mode = PULL_UNKNOWN, int openserver_id = 0, int pull_delay = PULL_DELAY_AUTOMATION);
	virtual void init();

	/*!
		\brief Start moving the actuator upwards.
	*/
	void goUp();

	/*!
		\brief Start moving the actuator downwards.
	*/
	void goDown();

	/*!
		\brief Stops the actuator.
	*/
	void stop();

	/*!
		\brief Requests a status update.

		It should never be necessary to call this function explicitly.
	*/
	void requestStatus();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
	virtual void requestPullStatus();
};


/*!
	\ingroup Automation
	\brief Actuator status device (TODO).

	Allows reading the actuator status (TODO).

	\section dimensions Dimensions
	\startdim
	\dim{DIM_STATUS,bool,,The actuator (TODO) status.}
	\enddim
*/
class PPTStatDevice : public device
{
Q_OBJECT
public:
	PPTStatDevice(QString address, int openserver_id = 0);
	virtual void init();

	/*!
		\brief Requests a status update.

		It should never be necessary to call this function explicitly.
	*/
	void requestStatus() const;

	// This enum is required to pass the information about the status with the valueReceived signal
	enum Type
	{
		DIM_STATUS
	};

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
};

#endif // DEV_AUTOMATION_H
