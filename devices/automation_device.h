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

	AutomationDevice(QString where, PullMode mode = PULL_UNKNOWN, int openserver_id = 0);
	virtual void init();
	void goUp();
	void goDown();
	void stop();
	void requestStatus();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
	virtual void requestPullStatus();
};

/**
 * \class PPTStatDevice
 *
 * This class represent a device for managing the PPTStat
 */
class PPTStatDevice : public device
{
Q_OBJECT
public:
	PPTStatDevice(QString address, int openserver_id = 0);
	virtual void init();
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
