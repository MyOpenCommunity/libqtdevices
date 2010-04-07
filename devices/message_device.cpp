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


#include "message_device.h"

MessageDevice::MessageDevice(QString where, int openserver_id) :
	device("8", where, openserver_id)
{
}

void MessageDevice::manageFrame(OpenMsg &msg)
{
	Q_UNUSED(msg);
}

void MessageDevice::sendReady()
{
	sendCommand("9013", QString("%1#8#00#%2#8").arg(cdp_where).arg(where));
}

void MessageDevice::sendBusy(QString caller_where)
{
	sendCommand("9014", QString("%1#8#00#%2#8").arg(caller_where).arg(where));
}

void MessageDevice::sendWrongChecksum(QString message_id)
{
	sendCommand(QString("9015#%1").arg(message_id), QString("%1#8#00#%2#8").arg(cdp_where).arg(where));
}

void MessageDevice::sendTimeout()
{
	sendCommand(QString("9016#%1").arg(message.size()), QString("%1#8#00#%2#8").arg(cdp_where).arg(where));
}
