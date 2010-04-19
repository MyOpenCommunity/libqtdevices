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


#include "frame_receiver.h"
#include "openclient.h" // client_monitor

// Inizialization of static member
QHash<int, Client*> FrameReceiver::clients_monitor;


FrameReceiver::FrameReceiver(int oid)
{
	subscribed = false;
	openserver_id = oid;
}

FrameReceiver::~FrameReceiver()
{
	if (subscribed)
		clients_monitor[openserver_id]->unsubscribe(this);
}

void FrameReceiver::setClientsMonitor(const QHash<int, Client*> &monitors)
{
	clients_monitor = monitors;
}

void FrameReceiver::subscribe_monitor(int who)
{
	Q_ASSERT_X(clients_monitor.contains(openserver_id), "FrameReceiver::subscribe_monitor",
		qPrintable(QString("Client monitor not set for id: %1!").arg(openserver_id)));
	clients_monitor[openserver_id]->subscribe(this, who);
	subscribed = true;
}

