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


#include "frame_classes.h"
#include "openclient.h" // client_monitor

// Inizialization of static member
QHash<int, ClientReader*> FrameReceiver::clients_monitor;
QHash<int, Clients> FrameSender::clients;


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

void FrameReceiver::setClientsMonitor(const QHash<int, ClientReader*> &monitors)
{
	clients_monitor = monitors;
}

void FrameReceiver::subscribeMonitor(int who)
{
	Q_ASSERT_X(clients_monitor.contains(openserver_id), "FrameReceiver::subscribe_monitor",
		qPrintable(QString("Client monitor not set for id: %1!").arg(openserver_id)));
	clients_monitor[openserver_id]->subscribe(this, who);
	subscribed = true;
}


FrameSender::FrameSender(int oid)
{
	subscribed = false;
	openserver_id = oid;
}

void FrameSender::sendFrame(const QString &frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].command, "FrameSender::sendFrame",
			   qPrintable(QString("Client COMMAND not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].command->sendFrameOpen(frame, ClientWriter::DELAY_IF_REQUESTED);
}

void FrameSender::sendFrameNow(const QString &frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].command, "FrameSender::sendFrameNow",
			   qPrintable(QString("Client COMMAND not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].command->sendFrameOpen(frame, ClientWriter::DELAY_NONE);
}

void FrameSender::sendInit(const QString &frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].request, "FrameSender::sendInit",
		qPrintable(QString("Client REQUEST not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].request->sendFrameOpen(frame, ClientWriter::DELAY_IF_REQUESTED);
}

void FrameSender::sendInitNow(const QString &frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].request, "FrameSender::sendInitNow",
		qPrintable(QString("Client REQUEST not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].request->sendFrameOpen(frame, ClientWriter::DELAY_NONE);
}

void FrameSender::sendCommandFrame(int oid, const QString &frame)
{
	Q_ASSERT_X(clients.contains(oid) && clients[oid].command, "FrameSender::sendCommandFrame",
			   qPrintable(QString("Client COMMAND not set for id: %1!").arg(oid)));
	clients[oid].command->sendFrameOpen(frame, ClientWriter::DELAY_NONE);
}

void FrameSender::setClients(const QHash<int, Clients> &c)
{
	clients = c;
}

void FrameSender::subscribeAck(int who)
{
	Q_ASSERT_X(clients.contains(openserver_id), "FrameSender::subscribeAck",
		qPrintable(QString("Clients not set for id: %1!").arg(openserver_id)));

	if (clients[openserver_id].command)
		clients[openserver_id].command->subscribeAck(this, who);

	if (clients[openserver_id].request)
		clients[openserver_id].request->subscribeAck(this, who);
	subscribed = true;
}

void FrameSender::delayFrames(bool delay)
{
	ClientWriter::delayFrames(delay);
}

FrameSender::~FrameSender()
{
	if (subscribed)
	{
		if (clients[openserver_id].command)
			clients[openserver_id].command->unsubscribeAck(this);

		if (clients[openserver_id].request)
			clients[openserver_id].request->unsubscribeAck(this);
	}
}


