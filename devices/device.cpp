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


#include "device.h"
#include "openclient.h"
#include "bttime.h"
#include "generic_functions.h" // createRequestOpen, createMsgOpen

#include <openmsg.h>

#include <QDebug>


// Inizialization of static member
QHash<int, QPair<Client*, Client*> > device::clients;


// Device implementation
device::device(QString _who, QString _where, int oid) : FrameReceiver(oid)
{
	who = _who;
	where = _where;
	openserver_id = oid;
	subscribe_monitor(who.toInt());
}

void device::subscribe_monitor(int who)
{
	FrameReceiver::subscribe_monitor(who);
	connect(clients_monitor[openserver_id], SIGNAL(connectionUp()), SIGNAL(connectionUp()));
	connect(clients_monitor[openserver_id], SIGNAL(connectionDown()), SIGNAL(connectionDown()));
}

bool device::isConnected()
{
	return clients_monitor[openserver_id]->isConnected();
}

void device::sendFrame(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].first, "device::sendFrame",
			   qPrintable(QString("Client comandi not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].first->sendFrameOpen(frame.toAscii().constData());
}

void device::sendInit(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].second, "device::sendInit",
		qPrintable(QString("Client richieste not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].second->sendFrameOpen(frame.toAscii().constData());
}

void device::sendCommand(QString what, QString _where) const
{
	sendFrame(createMsgOpen(who, what, _where));
}

void device::sendCommand(QString what) const
{
	sendCommand(what, where);
}

void device::sendRequest(QString what) const
{
	if (what.isEmpty())
		sendInit(createStatusRequestOpen(who, where));
	else
		sendInit(createRequestOpen(who, what, where));
}

void device::setClients(const QHash<int, QPair<Client*, Client*> > &c)
{
	clients = c;
}

QString device::get_key()
{
	return QString("%1#%2*%3").arg(openserver_id).arg(who).arg(where);
}

