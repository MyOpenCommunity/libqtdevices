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


#include "platform_device.h"
#include "frame_functions.h"
#include "bttime.h"

#include <openmsg.h>

#include <QDebug>
#include <QStringList>
#include <QDate>

// the request delay in milliseconds
#define STATUS_REQUEST_DELAY 1000


/*!
	\class PlatformDevice
	\brief This class represent a device for managing system settings.

	\a PlatformDevice has methods to set date (\a setDate())
	and time (\a setTime()). These methods don't set the date and time locally
	only, but syncronize all the devieces on the bus, too.

	It has also methods to retrive informations about network settings,
	kernel and firmware versions.
	When a request of information is done, the response is sent through the
	signal valueReceived.

	\section dimensions Dimensions
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

/*!
	\brief Constructor
 */
PlatformDevice::PlatformDevice() : device(QString("13"), QString(""))
{
}

void PlatformDevice::init()
{
	requestFirmwareVersion();
	requestKernelVersion();
	requestIp();
	requestNetmask();
}

/*!
	\brief Sets the time

	Sets the time locally and syncronize the other devices on the bus to the
	same time.
 */
void PlatformDevice::setTime(const BtTime &t)
{
	QString f;
	f.sprintf("*#13**#0*%02u*%02u*%02u**##", t.hour(), t.minute(), t.second());
	sendFrameNow(f);
}

/*!
	\brief Sets the time

	Sets the date locally and syncronize the other devices on the bus to the
	same date.
 */
void PlatformDevice::setDate(const QDate &d)
{
	QString f;
	f.sprintf("*#13**#1*00*%02d*%02d*%04d##", d.day(), d.month(), d.year());
	sendFrameNow(f);
}

/*!
	\brief Enables and disables the LAN settings
 */
void PlatformDevice::enableLan(bool enable)
{
	int val = enable ? 1 : 0;
	int what = DIM_STATUS;
	sendFrameNow(QString("*#%1**#%2*%3##").arg(who).arg(what).arg(val));
	// We need to perform a status request after the command to toggle the status,
	// but if we put the requestStatus call immediately after the command frame
	// the result is that the status request is received by the openserver before
	// the command. To preserve the order, we have to use a singleshot timer with
	// some delay.
	QTimer::singleShot(STATUS_REQUEST_DELAY, this, SLOT(requestStatus()));
}

void PlatformDevice::sendRequest(int what) const
{
	sendInitNow(createDimensionFrame(who, QString::number(what), where));
}

/*!
	\brief Requests the status
 */
void PlatformDevice::requestStatus() const
{
	sendRequest(DIM_STATUS);
}

/*!
	\brief Requests the IP address
 */
void PlatformDevice::requestIp() const
{
	sendRequest(DIM_IP);
}

/*!
	\brief Requests the network mask
 */
void PlatformDevice::requestNetmask() const
{
	sendRequest(DIM_NETMASK);
}

/*!
	\brief Requests the MAC address
 */
void PlatformDevice::requestMacAddress() const
{
	sendRequest(DIM_MACADDR);
}

/*!
	\brief Requests the gateway
 */
void PlatformDevice::requestGateway() const
{
	sendRequest(DIM_GATEWAY);
}

/*!
	\brief Requests the primary DNS
 */
void PlatformDevice::requestDNS1() const
{
	sendRequest(DIM_DNS1);
}

/*!
	\brief Requests the secondary DNS
 */
void PlatformDevice::requestDNS2() const
{
	sendRequest(DIM_DNS2);
}

/*!
	\brief Requests the kernel version
 */
void PlatformDevice::requestKernelVersion() const
{
	sendRequest(DIM_KERN_VERS);
}

/*!
	\brief Requests the firmware version
 */
void PlatformDevice::requestFirmwareVersion() const
{
	sendRequest(DIM_FW_VERS);
}

bool PlatformDevice::parseFrame(OpenMsg &msg, DeviceValues &values_list)
{

	int what = msg.what();
	int what_args = msg.whatArgCnt();

	QVariant v;

	if (what == DIM_MACADDR || what == DIM_IP || what == DIM_NETMASK ||
		what == DIM_GATEWAY || what == DIM_DNS1 || what == DIM_DNS2 ||
		what == DIM_STATUS || what == DIM_KERN_VERS || what == DIM_FW_VERS)
	{
		qDebug("PlatformDevice::parseFrame -> frame read:%s", msg.frame_open);

		switch (what)
		{
		case DIM_STATUS:
		{
			bool st = msg.whatArgN(0) == 1;
			v.setValue(st);
			break;
		}
		case DIM_MACADDR:
		{
			QStringList parts;
			for (int i = 0; i < what_args; ++i)
				parts << QString::number(msg.whatArgN(i), 16).rightJustified(2, '0');
			v.setValue(parts.join(":"));
			break;
		}
		case DIM_KERN_VERS:
			v.setValue(extractVersionValues(msg));
			break;
		case DIM_FW_VERS:
			v.setValue(extractVersionValues(msg));
			break;
		default:
		{
			Q_ASSERT(what_args == 4); // IPv4 ip are composed by 4 parts
			QStringList parts;
			for (int i = 0; i < what_args; ++i)
				parts << QString::number(msg.whatArgN(i));
			v.setValue(parts.join("."));
			break;
		}
		}

		values_list[what] = v;
		return true;
	}

	return false;
}

QString PlatformDevice::extractVersionValues(OpenMsg &msg)
{
	Q_ASSERT_X(msg.whatArgCnt() == 3, "PlatformDevice::extractVersionValues", "Open message invalid, must have 3 values.");
	QString str;
	// don't remove the following check, in production we won't have the assert above and don't rely on
	// frames to be always well formed
	if (msg.whatArgCnt() == 3)
		str = QString("%1.%2.%3").arg(msg.whatArgN(0)).arg(msg.whatArgN(1)).arg(msg.whatArgN(2));
	return str;
}
