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
#include "generic_functions.h" // createDimensionFrame, createCommandFrame
#include "devices_cache.h" // bt_global::devices_cache

#include <openmsg.h>

#include <QDebug>

#define COMPRESSION_TIMEOUT 1000

// Inizialization of static member
QHash<int, QPair<Client*, Client*> > device::clients;
QHash<int, OpenServerManager*> device::openservers;


int OpenServerManager::reconnection_time = 30;


OpenServerManager::OpenServerManager(int oid, Client *m, Client *c, Client *r)
{
	openserver_id = oid;
	monitor = m;
	command = c;
	request = r;
	is_connected = monitor->isConnected() && command->isConnected() && request->isConnected();
	connect(monitor, SIGNAL(connectionUp()), SLOT(handleConnectionUp()));
	connect(command, SIGNAL(connectionUp()), SLOT(handleConnectionUp()));
	connect(request, SIGNAL(connectionUp()), SLOT(handleConnectionUp()));
	connect(monitor, SIGNAL(connectionDown()), SLOT(handleConnectionDown()));
	connect(command, SIGNAL(connectionDown()), SLOT(handleConnectionDown()));
	connect(request, SIGNAL(connectionDown()), SLOT(handleConnectionDown()));
	if (!is_connected)
		connection_timer.start(reconnection_time * 1000, this);
}

void OpenServerManager::handleConnectionDown()
{
	if (is_connected)
	{
		qDebug("OpenServerManager::handleConnectionDown()[%d]", openserver_id);
		if (monitor->isConnected())
			monitor->disconnectFromHost();
		if (command->isConnected())
			command->disconnectFromHost();
		if (request->isConnected())
			request->disconnectFromHost();

		is_connected = false;
		emit connectionDown();
		connection_timer.start(reconnection_time * 1000, this);
	}
}

void OpenServerManager::timerEvent(QTimerEvent*)
{
	monitor->connectToHost();
	command->connectToHost();
	request->connectToHost();
	connection_timer.start(reconnection_time * 1000, this);
}

void OpenServerManager::handleConnectionUp()
{
	qDebug("OpenServerManager::handleConnectionUp");
	connection_timer.stop();

	if (!is_connected)
	{
		is_connected = monitor->isConnected() && command->isConnected() && request->isConnected();
		if (is_connected)
		{
			qDebug("OpenServerManager::handleConnectionUp()[%d]", openserver_id);
			bt_global::devices_cache.initOpenserverDevices(openserver_id);
			emit connectionUp();
		}
	}
}

bool OpenServerManager::isConnected()
{
	return is_connected;
}


// Device implementation
device::device(QString _who, QString _where, int oid) : FrameReceiver(oid)
{
	who = _who;
	where = _where;
	openserver_id = oid;
	subscribe_monitor(who.toInt());

	if (!openservers.contains(openserver_id))
	{
		openservers[openserver_id] = new OpenServerManager(openserver_id, clients_monitor[openserver_id],
			clients[openserver_id].first, clients[openserver_id].second);
	}

	connect(openservers[openserver_id], SIGNAL(connectionUp()), SIGNAL(connectionUp()));
	connect(openservers[openserver_id], SIGNAL(connectionDown()), SIGNAL(connectionDown()));
	connect(&compressor_mapper, SIGNAL(mapped(int)), SLOT(emitCompressedFrame(int)));
}

void device::manageFrame(OpenMsg &msg)
{
	DeviceValues values_list;
	parseFrame(msg, values_list);

	// values_list may be empty, avoid emitting a signal in such cases
	if (!values_list.isEmpty())
		emit valueReceived(values_list);
}

bool device::isConnected()
{
	return openservers[openserver_id]->isConnected();
}

void device::initDevices()
{
	foreach (int id, openservers.keys())
		if (openservers[id]->isConnected())
			bt_global::devices_cache.initOpenserverDevices(id);
}

int device::openserverId()
{
	return openserver_id;
}

void device::sendFrame(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].first, "device::sendFrame",
			   qPrintable(QString("Client comandi not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].first->sendFrameOpen(frame);
}

void device::sendCommandFrame(int openserver_id, const QString &frame)
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].first, "device::sendFrame",
			   qPrintable(QString("Client comandi not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].first->sendFrameOpen(frame);
}

void device::sendCompressedFrame(QString frame) const
{
	OpenMsg msg(frame.toStdString());

	int what = msg.what();
	if (compressed_frames.contains(what))
	{
		compressed_frames[what].first->start();
		compressed_frames[what].second = frame;
	}
	else
	{
		QTimer *timeout = new QTimer(const_cast<device*>(this));
		timeout->setSingleShot(true);
		timeout->start(COMPRESSION_TIMEOUT);

		connect(timeout, SIGNAL(timeout()), &compressor_mapper, SLOT(map()));
		compressor_mapper.setMapping(timeout, what);
		compressed_frames[what] = qMakePair(timeout, frame);
	}
}

void device::emitCompressedFrame(int what)
{
	qDebug() << "Emitting compressed frame" << where << what;

	Q_ASSERT_X(compressed_frames.contains(what), "device::emitCompressedFrame", "tried to emit a frame twice");

	compressed_frames[what].first->deleteLater();
	sendFrame(compressed_frames[what].second);

	compressed_frames.remove(what);
}

void device::sendInit(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].second, "device::sendInit",
		qPrintable(QString("Client richieste not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].second->sendFrameOpen(frame);
}

void device::sendCommand(QString what, QString _where) const
{
	sendFrame(createCommandFrame(who, what, _where));
}

void device::sendCommand(QString what) const
{
	sendCommand(what, where);
}

void device::sendCommand(int what) const
{
	sendCommand(QString::number(what), where);
}

void device::sendRequest(QString what) const
{
	if (what.isEmpty())
		sendInit(createStatusRequestFrame(who, where));
	else
		sendInit(createDimensionFrame(who, what, where));
}

void device::sendRequest(int what) const
{
	sendRequest(QString::number(what));
}

void device::setClients(const QHash<int, QPair<Client*, Client*> > &c)
{
	clients = c;
}

QString device::get_key()
{
	return QString("%1#%2*%3").arg(openserver_id).arg(who).arg(where);
}

