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
#include "bttime.h"

#include <openmsg.h>

#include <QDebug>
#include <QStringList>
#include <QDate>

// the request delay in milliseconds
#define STATUS_REQUEST_DELAY 1000


PlatformDevice::PlatformDevice() : device(QString("13"), QString(""))
{
}

void PlatformDevice::setTime(const BtTime &t)
{
	QString f;
	f.sprintf("*#13**#0*%02u*%02u*%02u**##", t.hour(), t.minute(), t.second());
	sendFrame(f);
}

void PlatformDevice::setDate(const QDate &d)
{
	QString f;
	f.sprintf("*#13**#1*00*%02d*%02d*%04d##", d.day(), d.month(), d.year());
	sendFrame(f);
}

void PlatformDevice::enableLan(bool enable)
{
	int val = enable ? 1 : 0;
	int what = DIM_STATUS;
	sendFrame(QString("*#%1**#%2*%3##").arg(who).arg(what).arg(val));
	// We need to perform a status request after the command to toggle the status,
	// but if we put the requestStatus call immediately after the command frame
	// the result is that the status request is received by the openserver before
	// the command. To preserve the order, we have to use a singleshot timer with
	// some delay.
	QTimer::singleShot(STATUS_REQUEST_DELAY, this, SLOT(requestStatus()));
}

void PlatformDevice::requestStatus() const
{
//	sendRequest(QString::number(DIM_STATUS));
}

void PlatformDevice::requestIp() const
{
//	sendRequest(QString::number(DIM_IP));
}

void PlatformDevice::requestNetmask() const
{
//	sendRequest(QString::number(DIM_NETMASK));
}

void PlatformDevice::requestMacAddress() const
{
//	sendRequest(QString::number(DIM_MACADDR));
}

void PlatformDevice::requestGateway() const
{
//	sendRequest(QString::number(DIM_GATEWAY));
}

void PlatformDevice::requestDNS1() const
{
//	sendRequest(QString::number(DIM_DNS1));
}

void PlatformDevice::requestDNS2() const
{
//	sendRequest(QString::number(DIM_DNS2));
}

void PlatformDevice::requestKernelVersion() const
{
//	sendRequest(QString::number(DIM_KERN_VERS));
}

void PlatformDevice::requestFirmwareVersion() const
{
//	sendRequest(QString::number(DIM_FW_VERS));
}

void PlatformDevice::manageFrame(OpenMsg &msg)
{

	int what = msg.what();
	int what_args = msg.whatArgCnt();

	StatusList status_list;
	QVariant v;

	if (what == DIM_MACADDR || what == DIM_IP || what == DIM_NETMASK ||
		what == DIM_GATEWAY || what == DIM_DNS1 || what == DIM_DNS2 ||
		what == DIM_STATUS || what == DIM_KERN_VERS || what == DIM_FW_VERS)
	{
		qDebug("PlatformDevice::manageFrame -> frame read:%s", msg.frame_open);

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

		status_list[what] = v;
		emit status_changed(status_list);
	}
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
