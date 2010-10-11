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


/*!
	\class OpenServerManager
	\brief Manages the logic of connection/disconnection/reconnection
	to the openserver.

	Basically, with an openserver you have 3 socket connections open: the monitor,
	the command and the request. When one of these fall down all the objects
	connected with that openserver must to be notify with the signal connectionDown.

	In this case we have to retry the connection after
	\a OpenServerManager::reconnection_time seconds.

	Also in the case of a successfully reconnection the application must to be
	notified about the event in order to resyncronize to the home systems.
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

	Try to reconnect all instances of OpenClient every
	\a OpenServerManager::reconnection_time if connection is down.
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


/*!
	\class FrameCompressor
	\brief Incapsulate the logic of compresse frames

	Could happen that for some reasons is requested to send many frames
	with the same "what". To avoid to flood the openserver, you can use the
	this class which sends only the last frame of the same "what"
	in a time interval.
 */

/*!
	\fn FrameCompressor::sendFrame()
	\brief emitted when it is time to send the frame
 */

/*!
	\brief Constructor
 */
FrameCompressor::FrameCompressor()
{
	connect(&compressor_mapper, SIGNAL(mapped(int)), SLOT(emitCompressedFrame(int)));
}

/*!
	\brief Sends only one frame of the same "what" into a time interval

	Queues the frame to be emitted after a time interval; if another compressed
	frame with the same "what" is sent before the timeout, the first frame is
	discarded and the timeout restarted.
 */
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


/*!
	\class device
	\brief Base abstract class for all devices.

	In order to create a new device you must subclass the \device class.

	\section handling Frame handling

	To parse incoming frames, there are two methods that you can overload:
	\li \a manageFrame()
	\li \a parseFrame()

	The latter is the prefered one because can work as the former, and permits
	a fine grain management of frame handling making the creation of device
	hierarchies verry easy.
	When a new frame is received (and handled if you are overloading the
	\a parseFrame() method) the \a valueReceived() signal is emitted.

	\section device-dimensions Device dimensions
	The argument of the \a valueReceived signal is a DeviceValues, which is a
	typedef for \a QHash<int, QVariant>.

	Each item of this data structure is a \a "dimension" and indicates a
	different value produced by the device.

	\section sending Frame sending

	There are many methods to send frames. Depending on what you want to do, you
	can use one of the following methods:

	\li \a sendFrame() to send a frame on command port
	\li \a sendInit() to send a frame on the request port

	These frame are sent immediatelly (after a ~10 ms in truth) unless you
	called \a delayFrames() with true as parameter.
	However there are the send-immediatelly versions of these methods
	(\a sendFrameNow() and \a sendInitNow()) that sends the frames without delay
	although was setted the dalay.

	Could happen that for some reasons is requested to send many frames
	with the same "what". To avoid to flood the openserver, you can use the method
	\a sendCompressedFrame() which sends only the last frame of the same "what"
	in a time interval.

	\sa manageFrame(), parseFrame(), sendFrame(), sendFrameNow(), sendInit(),
	sendInitNow(), sendCompressedFrame(), delayFrames(), valueReceived()
 */

/*!
	\var device::who
	\brief The system of the device
 */

/*!
	\var device::where
	\brief The address of the device
 */

/*!
	\var device::openserver_id
	\brief The Open Server id
 */

/*!
	\var device::COMPRESSION_TIMEOUT
	\brief default timeout value for compressed frames
 */

/*!
	\brief Constructor

	Construct a new device with the given \a who, \a where and \a Open Server id.

	\note The costructor is protected only to make device abstract.
	\note The default openserver id should be keep in sync with the define
	MAIN_OPENSERVER.
 */

device::device(QString _who, QString _where, int oid) : FrameReceiver(oid)
{
	who = _who;
	where = _where;
	openserver_id = oid;
	subscribe_monitor(who.toInt());

	OpenServerManager *manager = getManager(openserver_id);

	connect(manager, SIGNAL(connectionUp()), SIGNAL(connectionUp()));
	connect(manager, SIGNAL(connectionDown()), SIGNAL(connectionDown()));
	connect(&frame_compressor, SIGNAL(sendFrame(QString)), SLOT(sendFrame(QString)));
	connect(&request_compressor, SIGNAL(sendFrame(QString)), SLOT(sendInit(QString)));
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

/*!
	\brief Manages a frame

	Can be reimplemented in order to parse the incoming frames (from the client
	monitor). However, if the specific device can be subclassed, reimplement the
	\a parseFrame() method in order to avoid a double valueReceived signal.

	If the frame is handled the valueReceived() signal is emitted.

	\sa parseFrame()
 */
void device::manageFrame(OpenMsg &msg)
{
	DeviceValues values_list;
	parseFrame(msg, values_list);

	// values_list may be empty, avoid emitting a signal in such cases
	if (!values_list.isEmpty())
		emit valueReceived(values_list);
}

/*!
	\fn device::parseFrame(OpenMsg &msg, DeviceValues &values_list)
	\brief Preferred way to parse incoming frames.

	This method must be reimplemented to handle incoming frames.
	It must return true if the argument frame was recognized and processed,
	false otherwise.
	The default implementation simply returns false.

	\sa manageFrame()
 */

/*!
	\brief Returns true if is connected to the Open Server.
 */
bool device::isConnected()
{
	return openservers[openserver_id]->isConnected();
}

/*!
	\fn device::init()
	\brief Send messages to initialize data.

	Every device should re-implement this method, in order to syncronize the
	device status with the related graphic object one.
*/

/*!
	\brief Calls the \a init() of all devices

	\sa init()
 */
void device::initDevices()
{
	bt_global::devices_cache.devicesCreated();

	foreach (int id, openservers.keys())
		if (openservers[id]->isConnected())
			bt_global::devices_cache.initOpenserverDevices(id);
}

/*!
	\brief Returns the Open Server ID.
 */
int device::openserverId()
{
	return openserver_id;
}

/*!
	\brief Set this flag to delay the frames sent using the \a sendFrame()
	\a sendInit() methods.

	\sa sendFrame(), sendFrameNow(), sendInit(), sendInitNow()
 */
void device::delayFrames(bool delay)
{
	Client::delayFrames(delay);
}

/*!
	\fn device::status_changed(QList< device_status * >)
	\brief Old status changed

	\deprecated use \a valueReceived() instead.

	\sa valueReceived()
 */

/*!
	\fn device::valueReceived(const DeviceValues &values_list)
	\brief Signal used to inform that a dimension of device has changed or a
	new command has received.

	For some devices, more than one dimension can change at the same time,
	so the int is used as an enum to recognize the dimensions.

	\note  Using an int is a design choice. In this way the signal is
	generic (so the connections can be made in a generic way) and the enum
	can be specific for a device, avoiding the coupling between abstract
	and concrete device class.

	\sa \ref device-dimensions
 */

/*!
	\fn device::connectionUp()
	\brief signal emitted when the connection comes up

	\sa OpenServerManager::connectionUp()
 */

/*!
	\fn device::connectionDown()
	\brief signal emitted when the connection goes down

	\sa OpenServerManager::connectionDown()
*/

/*!
	\brief Sends frames to the openserver, using the COMMAND channel.

	The frame can be really sent after a delay depending on the \a delayFrames()
	setting.

	\sa delayFrames(), sendFrameNow()
 */
void device::sendFrame(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].command, "device::sendFrame",
			   qPrintable(QString("Client COMMAND not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].command->sendFrameOpen(frame, Client::DELAY_IF_REQUESTED);
}

/*!
	\brief Sends frames to the openserver, using the COMMAND channel.

	The frame is sent immediatelly not taking care of the \a delayFrames()
	setting.

	\sa delayFrames(), sendFrame()
 */
void device::sendFrameNow(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].command, "device::sendFrame",
			   qPrintable(QString("Client COMMAND not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].command->sendFrameOpen(frame, Client::DELAY_NONE);
}

/*!
	\brief Send (without delay) a frame to the Open Server with the given id.
 */
void device::sendCommandFrame(int openserver_id, const QString &frame)
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].command, "device::sendFrame",
			   qPrintable(QString("Client COMMAND not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].command->sendFrameOpen(frame, Client::DELAY_NONE);
}

/*!
	\brief Sends only one frame of the same "what" into a time interval on
	COMMAND port.

	Queues the frame to be emitted after a time interval; if another compressed
	frame with the same "what" is sent before the timeout, the first frame is
	discarded and the timeout restarted.

	\sa sendFrame(), FrameCompressor
 */
void device::sendCompressedFrame(QString frame, int compression_timeout) const
{
	frame_compressor.sendCompressedFrame(frame, compression_timeout);
}

/*!
	\brief Sends only one frame of the same "what" into a time interval on
	REQUEST port.

	Queues the frame to be emitted after a time interval; if another compressed
	frame with the same "what" is sent before the timeout, the first frame is
	discarded and the timeout restarted.

	\sa sendInit(), FrameCompressor
 */
void device::sendCompressedInit(QString frame, int compression_timeout) const
{
	request_compressor.sendCompressedFrame(frame, compression_timeout);
}

/*!
	\brief Sends frames to the openserver, using the REQUEST channel.

	The frame can be really sent after a delay depending on the \a delayFrames()
	setting.

	\sa delayFrames(), sendInitNow()
 */
void device::sendInit(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].request, "device::sendInit",
		qPrintable(QString("Client REQUEST not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].request->sendFrameOpen(frame, Client::DELAY_IF_REQUESTED);
}

/*!
	\brief Sends frames to the openserver, using the REQUEST channel.

	The frame is sent immediatelly not taking care of the \a delayFrames()
	setting.

	\sa delayFrames(), sendFrame()
 */
void device::sendInitNow(QString frame) const
{
	Q_ASSERT_X(clients.contains(openserver_id) && clients[openserver_id].request, "device::sendInit",
		qPrintable(QString("Client REQUEST not set for id: %1!").arg(openserver_id)));
	clients[openserver_id].request->sendFrameOpen(frame, Client::DELAY_NONE);
}

/*!
	\breif Sends a frame on the COMMAND port with the given \a what and \a where.
 */
void device::sendCommand(QString what, QString _where) const
{
	sendFrame(createCommandFrame(who, what, _where));
}

/*!
	\overload device::sendCommand(QString what) const
 */
void device::sendCommand(QString what) const
{
	sendCommand(what, where);
}

/*!
	\overload device::sendCommand(int what) const
 */
void device::sendCommand(int what) const
{
	sendCommand(QString::number(what), where);
}

/*!
	\breif Sends a frame on the REQUEST port with the given \a what.
 */
void device::sendRequest(QString what) const
{
	if (what.isEmpty())
		sendInit(createStatusRequestFrame(who, where));
	else
		sendInit(createDimensionFrame(who, what, where));
}

/*!
	\overload device::sendRequest(int what) const
 */
void device::sendRequest(int what) const
{
	sendRequest(QString::number(what));
}

/*!
	\brief sets the clients
 */
void device::setClients(const QHash<int, Clients> &c)
{
	clients = c;
}

/*!
	\brief Returns cache key
 */
QString device::get_key()
{
	return QString("%4-%1#%2*%3").arg(openserver_id).arg(who).arg(where).arg(metaObject()->className());
}

