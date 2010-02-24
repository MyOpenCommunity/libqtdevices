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

#ifndef PLATFORM_DEVICE_H
#define PLATFORM_DEVICE_H

#include "device.h"

#include <QHash>
#include <QVariant>


class OpenMsg;
class BtTime;
class QDate;


/**
 * \class PlatformDevice
 *
 * This class represent a device for managing lan settings. It has a method to
 * activate/deactivate the lan, and some methods to obtain information about
 * actual settings. When a request of information is done, the response is sent
 * through the signal status_changed.
 */
class PlatformDevice : public device
{
Q_OBJECT
public:
	PlatformDevice();

	// date/time methods
	void setTime(const BtTime &t);
	void setDate(const QDate &d);

	// lan management methods
	void enableLan(bool enable);

	// The request methods, used to request an information
	void requestIp() const;
	void requestNetmask() const;
	void requestMacAddress() const;
	void requestGateway() const;
	void requestDNS1() const;
	void requestDNS2() const;
	void requestKernelVersion() const;
	void requestFirmwareVersion() const;

	virtual void manageFrame(OpenMsg &msg);

	enum Type
	{
		DIM_STATUS = 9,
		DIM_IP = 10,
		DIM_NETMASK = 11,
		DIM_MACADDR = 12,
		DIM_FW_VERS = 16,                 // firmware version
		DIM_KERN_VERS = 23,               // kernel version
		DIM_GATEWAY = 50,
		DIM_DNS1 = 51,
		DIM_DNS2 = 52
	};

public slots:
	void requestStatus() const;

private:
	QString extractVersionValues(OpenMsg &msg);
};

#endif // PLATFORM_DEVICE_H
