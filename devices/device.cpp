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

// Inizialization of static member
QHash<int, OpenServerManager*> device::openservers;


int OpenServerManager::reconnection_time = 30;


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

void OpenServerManager::timerEvent(QTimerEvent*)
{
	if (!monitor->isConnected() && !monitor->isConnecting())
		monitor->connectToHost();
	if (!command->isConnected() && !command->isConnecting())
		command->connectToHost();
	if (!request->isConnected() && !request->isConnecting())
		request->connectToHost();
	if (supervisor && !supervisor->isConnected() && !supervisor->isConnecting())
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
		else
		{
			// restart the connection timer to handle the case when we get
			// a connectionDown signal followed by a connectionUp, so the
			// timer is stopped even if not all sockets are connected
			connection_timer.start(reconnection_time * 1000, this);
		}
	}
}

bool OpenServerManager::isConnected()
{
	return is_connected;
}


FrameCompressor::FrameCompressor()
{
	connect(&compressor_mapper, SIGNAL(mapped(int)), SLOT(emitCompressedFrame(int)));
}

void FrameCompressor::sendCompressedFrame(QString frame, int compression_timeout) const
{
	OpenMsg msg(frame.toStdString());

	int what = msg.what();
	if (compressed_frames.contains(what))
	{
		// replaces the old frame and restarts the timer
		compressed_frames[what].first->start(compression_timeout);
		compressed_frames[what].second = frame;
	}
	else
	{
		// creates a new timer for the what and sets up the signal mapper
		QTimer *timeout = new QTimer(const_cast<FrameCompressor*>(this));
		timeout->setSingleShot(true);
		timeout->start(compression_timeout);

		connect(timeout, SIGNAL(timeout()), &compressor_mapper, SLOT(map()));
		compressor_mapper.setMapping(timeout, what);
		compressed_frames[what] = qMakePair(timeout, frame);
	}
}

void FrameCompressor::emitCompressedFrame(int what)
{
	qDebug() << "Emitting compressed frame for" << what;

	Q_ASSERT_X(compressed_frames.contains(what), "FrameCompressor::emitCompressedFrame", "tried to emit a frame twice");

	compressed_frames[what].first->deleteLater();
	emit sendFrame(compressed_frames[what].second);

	compressed_frames.remove(what);
}

void FrameCompressor::flushCompressedFrames()
{
	foreach (int what, compressed_frames.keys())
		emitCompressedFrame(what);
}


device::device(QString _who, QString _where, int oid) : FrameReceiver(oid), FrameSender(oid)
{
	who = _who;
	where = _where;
	openserver_id = oid;
	subscribeMonitor(who.toInt());

	supported_init_mode = NORMAL_INIT;
	init_request_done = false;

	OpenServerManager *manager = getManager(openserver_id);

	connect(manager, SIGNAL(connectionDown()), this, SLOT(handleConnectionDown(void)));
	connect(manager, SIGNAL(connectionUp()), this, SLOT(handleConnectionUp(void)));

	connect(&frame_compressor, SIGNAL(sendFrame(QString)), SLOT(slotSendFrame(QString)));
	connect(&request_compressor, SIGNAL(sendFrame(QString)), SLOT(slotSendInit(QString)));

	// Clear the attribut indicator set
	initialized_attrid.clear();
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


bool device::smartInit(SupportedInitMode cur_init_mode)
{
	bool retCode=true;

	if (supported_init_mode == DISABLED_INIT)
		return true;

	if(cur_init_mode == DEFERRED_INIT)
	{
		if((!init_request_done) && !isInitialized())
		{
			init();
			init_request_done = true;
		}
	}
	else
	{
		switch(supported_init_mode)
		{
		case  DEFERRED_INIT:
			if(!isInitialized())
			{
				init_request_done = false;
				retCode = false;
			}
			else
			{
				qDebug() << "device["<<get_key()<<"] already initialized ... deferred init skipped ";
				init_request_done = true;
				retCode = true;
			}
			break;

		case  NORMAL_INIT:
			init();
			init_request_done = true;
			break;

		default:
			qWarning("device::smartInit Unsupported INIT mode");
		}
	}
	return retCode;
}

device::SupportedInitMode device::getSupportedInitMode()	{
	return supported_init_mode;
}

void device::setSupportedInitMode(SupportedInitMode the_mode)	{
	supported_init_mode = the_mode;
}

int device::openserverId()
{
	return openserver_id;
}

void device::slotSendFrame(QString frame) const
{
	sendFrame(frame);
}

void device::sendCompressedFrame(QString frame, int compression_timeout) const
{
	frame_compressor.sendCompressedFrame(frame, compression_timeout);
}

void device::sendCompressedInit(QString frame, int compression_timeout) const
{
	request_compressor.sendCompressedFrame(frame, compression_timeout);
}

void device::slotSendInit(QString frame) const
{
	sendInit(frame);
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

QString device::get_key()
{
	return QString("%4-%1#%2*%3").arg(openserver_id).arg(who).arg(where).arg(metaObject()->className());
}

RawDevice::RawDevice(int openserver_id) :
	device("0", "0", openserver_id)
{
}

void RawDevice::sendCommand(QString frame)
{
	sendFrame(frame);
}

void RawDevice::sendRequest(QString frame)
{
	sendInit(frame);
}

bool device::isInitialized(void)
{
	return(false);
}

void device::setReceivedAttributes(const DeviceValues& values_list )
{
	// Look for keys into the received attribute list
	QList<int> the_keys = values_list.keys();

	// Set the received attribute ID
	initialized_attrid |= (QSet<int>::fromList(the_keys));

	qDebug() << "Received attribute list for device ["<<get_key()<<"] values: "<<values_list;
}

void device::handleConnectionDown()
{
	// reset the attribute set indicator
	qDebug() << "Connection down for device ["<<get_key()<<"]";
	initialized_attrid.clear();

	// forward the signal
	emit connectionDown();
}

void device::handleConnectionUp()
{
	// reset the attribute set indicator
	qDebug() << "Connection up for device ["<<get_key()<<"]";
	initialized_attrid.clear();

	// forward the signal
	emit connectionUp();
}
