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
#include "frame_functions.h" // createDimensionFrame, createCommandFrame
#include "devices_cache.h" // bt_global::devices_cache

#include <openmsg.h>

#include <QDebug>

#define COMPRESSION_TIMEOUT 1000


/*!
	\class OpenServerManager
	\brief Class that manages the logic of connection/disconnection/reconnection
	to the openserver.

	Basically, with an openserver you have 3 socket connections open: the monitor,
	the command and the request. When one of these fall down all the objects
	connected with that openserver must to be notify with the signal connectionDown.

	In this case we have to retry the connection after
	\a OpenServerManager::reconnection_time seconds.

	Also in the case of a successfully reconnection the application must to be
	notified about the event.
 */


// Inizialization of static member
QHash<int, Clients> device::clients;
QHash<int, OpenServerManager*> device::openservers;


//! The interval (in seconds) to retry the connection with the openserver.
int OpenServerManager::reconnection_time = 30;

/*!
	\brief Constructor

	Constructs a new \a OpenServerManager giving the openserver id, and the
	\a OpenClient instances for the monitor (\a m), supervisor (\a s, optional),
	command (\a c) and request (\a r) ports.
 */
OpenServerManager::OpenServerManager(int oid, Client *m, Client *s, Client *c, Client *r)
{
	openserver_id = oid;
	monitor = m;
	supervisor = s;
	command = c;
	request = r;
	is_connected = monitor->isConnected() && command->isConnected() && request->isConnected() &&
		(!supervisor || supervisor->isConnected());

	foreach(Client *client, QList<Client*>() << monitor << command << request << supervisor)
	{
		if (client)
		{
			connect(client, SIGNAL(connectionUp()), SLOT(handleConnectionUp()));
			connect(client, SIGNAL(connectionDown()), SLOT(handleConnectionDown()));
		}
	}
	if (!is_connected)
		connection_timer.start(reconnection_time * 1000, this);
}

/*!
	\fn OpenServerManager::connectionUp()
	\brief emitted when all the OpenClient are successfully connected.
 */

/*!
	\fn OpenServerManager::connectionDown()
	\brief emitted when all the OpenClient are successfully disconnected.
 */

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
		if (supervisor && supervisor->isConnected())
			supervisor->disconnectFromHost();

		is_connected = false;
		emit connectionDown();
		connection_timer.start(reconnection_time * 1000, this);
	}
}

/*!
	\brief Reconnect the \a OpenClient instances.

	Try to reconnect all instances of OpenClient after
	\a OpenServerManager::reconnection_time.
 */
void OpenServerManager::timerEvent(QTimerEvent*)
{
	monitor->connectToHost();
	command->connectToHost();
	request->connectToHost();
	if (supervisor)
		supervisor->connectToHost();
	connection_timer.start(reconnection_time * 1000, this);
}

void OpenServerManager::handleConnectionUp()
{
	qDebug("OpenServerManager::handleConnectionUp");
	connection_timer.stop();

	if (!is_connected)
	{
		is_connected = monitor->isConnected() && command->isConnected() && request->isConnected() &&
			(!supervisor || supervisor->isConnected());
		if (is_connected)
		{
			qDebug("OpenServerManager::connectionUp for openserver [%d]", openserver_id);
			bt_global::devices_cache.initOpenserverDevices(openserver_id);
			emit connectionUp();
		}
	}
}

/*!
	\brief Returns true if the \a OpenClient instances are connected, false
	otherwise.
 */
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

	OpenServerManager *manager = getManager(openserver_id);

	connect(manager, SIGNAL(connectionUp()), SIGNAL(connectionUp()));
	connect(manager, SIGNAL(connectionDown()), SIGNAL(connectionDown()));
	connect(&compressor_mapper, SIGNAL(mapped(int)), SLOT(emitCompressedFrame(int)));
}

OpenServerManager *device::getManager(int openserver_id)
{
	if (!openservers.contains(openserver_id))
	{
		openservers[openserver_id] = new OpenServerManager(openserver_id, clients_monitor[openserver_id],
			clients[openserver_id].supervisor, clients[openserver_id].command, clients[openserver_id].request);
	}

	return openservers[openserver_id];
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
	bt_global::devices_cache.devicesCreated();

	foreach (int id, openservers.keys())
		if (openservers[id]->isConnected())
			bt_global::devices_cache.initOpenserverDevices(id);
}

int device::openserverId()
{
	return openserver_id;
}

void device::delayFrames(bool delay)
{
	Client::delayFrames(delay);
}

void device::sendFrame(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].command, "device::sendFrame",
			   qPrintable(QString("Client COMMAND not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].command->sendFrameOpen(frame, Client::DELAY_IF_REQUESTED);
}

void device::sendFrameNow(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].command, "device::sendFrame",
			   qPrintable(QString("Client COMMAND not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].command->sendFrameOpen(frame, Client::DELAY_NONE);
}

void device::sendCommandFrame(int openserver_id, const QString &frame)
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].command, "device::sendFrame",
			   qPrintable(QString("Client COMMAND not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].command->sendFrameOpen(frame, Client::DELAY_NONE);
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
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].request, "device::sendInit",
		qPrintable(QString("Client REQUEST not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].request->sendFrameOpen(frame, Client::DELAY_IF_REQUESTED);
}

void device::sendInitNow(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].request, "device::sendInit",
		qPrintable(QString("Client REQUEST not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].request->sendFrameOpen(frame, Client::DELAY_NONE);
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

void device::setClients(const QHash<int, Clients> &c)
{
	clients = c;
}

QString device::get_key()
{
	return QString("%4-%1#%2*%3").arg(openserver_id).arg(who).arg(where).arg(metaObject()->className());
}

