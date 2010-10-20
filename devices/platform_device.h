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


/*!
	\ingroup Core
	\brief Device for managing system settings.

	%PlatformDevice has methods to set date setDate()
	and time setTime(). These methods don't set the date and time locally
	only, but syncronize all the devices on the bus, too.

	It has also methods to retrive informations about network settings,
	kernel and firmware versions.
	When a request of information is done, the response is sent through the
	signal valueReceived().

	\section PlatformDevice-dimensions Dimensions
	\startdim
	\dim{DIM_STATUS,bool,,Network status\, true if the network is enabled\, false otherwise.}
	\dim{DIM_IP,QString,,Ip address}
	\dim{DIM_NETMASK,QString,,Network mask}
	\dim{DIM_MACADDR,QString,,MAC address}
	\dim{DIM_GATEWAY,QString,,Gateway}
	\dim{DIM_DNS1,QString,,Primary DNS}
	\dim{DIM_DNS2,QString,,Secondary DNS}
	\dim{DIM_FW_VERS,QString,,Firmware version}
	\dim{DIM_KERN_VERS,QString,,Kernel version}
	\enddim
*/
class PlatformDevice : public device
{
Q_OBJECT
public:
	PlatformDevice();

	virtual void init();

	/*!
		\brief Sets the time

		Sets the time locally and syncronizes the other devices on the bus to the
		same time.
	*/
	void setTime(const BtTime &t);

	/*!
		\brief Sets the time

		Sets the date locally and syncronizes the other devices on the bus to the
		same date.
	*/
	void setDate(const QDate &d);

	/*!
		\brief Enables or disables the LAN settings.
	*/
	void enableLan(bool enable);

	/*!
		\brief Requests the IP address.
	*/
	void requestIp() const;

	/*!
		\brief Requests the network mask.
	*/
	void requestNetmask() const;

	/*!
		\brief Requests the MAC address.
	*/
	void requestMacAddress() const;

	/*!
		\brief Requests the gateway.
	*/
	void requestGateway() const;

	/*!
		\brief Requests the primary DNS
	*/
	void requestDNS1() const;

	/*!
		\brief Requests the secondary DNS
	*/
	void requestDNS2() const;

	/*!
		\brief Requests the kernel version
	*/
	void requestKernelVersion() const;

	/*!
		\brief Requests the firmware version
	*/
	void requestFirmwareVersion() const;

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

	/*!
		\refdim{PlatformDevice}
	*/
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
	void sendRequest(int what) const;
};

#endif // PLATFORM_DEVICE_H
