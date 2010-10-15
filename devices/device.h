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


#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "frame_receiver.h"

#include <QVariant>
#include <QHash>
#include <QList>
#include <QTimer>
#include <QSignalMapper>

class device_status;
class frame_interpreter;
class Client;
class OpenMsg;

typedef QHash<int, QVariant> DeviceValues;


struct Clients
{
	Client *command;
	Client *request;
	Client *supervisor;
	Clients() { command = request = supervisor = 0; }
};

/*!
	\ingroup Core
	\brief Manages the logic of connection to the openserver.

	Most of the time when you communicate with an openserver you have 3 socket
	connections open:
	\li the Client::MONITOR monitor;
	\li the Client::COMMAND command;
	\li the Client::REQUEST request.

	When one of these fall down all the objects connected with that openserver
	must to be notify with the signal connectionDown().

	In this case we have to retry the connection after OpenServerManager::reconnection_time
	seconds.

	Also in the case of a successfully reconnection the application must to be
	notified about the event in order to resyncronize to the home systems.
*/
class OpenServerManager : public QObject
{
Q_OBJECT
public:
	/*!
		\brief Constructor

		Constructs a new OpenServerManager giving the openserver id, and the
		Client instances for the monitor \a m, supervisor \a s (optional),
		command \a c and request \a r sockets.
	*/

	OpenServerManager(int oid, Client *monitor, Client *supervisor, Client *command, Client *request);

	/*!
		\brief Returns true if the Client instances are connected, false
		otherwise.
	*/
	bool isConnected();

	/*!
		\brief The interval (in seconds) to retry the connection with the openserver.
	*/
	static int reconnection_time;

signals:
	/*!
		\brief emitted when all the Client are successfully connected.
	*/
	void connectionUp();

	/*!
		\brief emitted when all the Client are disconnected.
	*/
	void connectionDown();

protected:
	/*!
		\brief Reconnect the Client instances.

		Try to reconnect all instances of Client every OpenServerManager::reconnection_time
		if connection is down.
	*/
	void timerEvent(QTimerEvent*);

private slots:
	void handleConnectionUp();
	void handleConnectionDown();

private:
	Client *monitor, *supervisor, *command, *request;
	int openserver_id;
	bool is_connected;
	QBasicTimer connection_timer;
};


/*!
	\ingroup Core
	\class FrameCompressor
	\brief Incapsulate the logic of compressed frames.

	To avoid to flood the openserver, queues the frame to be emitted after a
	time interval; if another compressed frame with the same "what" is sent
	before the timeout, the first frame is discarded and the timeout restarted.
*/
class FrameCompressor : public QObject
{
friend class TestDevice;
Q_OBJECT
public:
	FrameCompressor();

	/*!
		\brief Sends only one frame of the same \a what into a time interval.

		Queues the frame to be emitted after a time interval; if another compressed
		frame with the same \a what is sent before the timeout, the first frame is
		discarded and the timeout restarted.
	*/
	void sendCompressedFrame(QString frame, int compression_timeout) const;

signals:
	/*!
		\brief emitted when it is time to send the frame.
	*/
	void sendFrame(QString frame);

private slots:
	void emitCompressedFrame(int what);

private:
	// used by the tests
	void flushCompressedFrames();

private:
	mutable QHash<int, QPair<QTimer*, QString> > compressed_frames;
	mutable QSignalMapper compressor_mapper;
};


/*!
	\ingroup Core
	\brief Base abstract class for all devices.

	In order to create a new device you must subclass this class.

	\section handling Frame handling

	To parse incoming frames, there are two methods that you can overload:
	\li manageFrame()
	\li parseFrame()

	The latter is the prefered one because can work as the former, and permits
	a fine grain management of frame handling making the creation of device
	hierarchies verry easy.
	When a new frame is received (and handled if you are overloading the
	parseFrame() method) the valueReceived() signal is emitted.

	\section device-dimensions Device dimensions
	The argument of the valueReceived signal is a DeviceValues, which is a
	typedef for \a QHash<int, QVariant>.

	Each item of this data structure is a "dimension" and indicates a
	different value produced by the device.

	\section sending Frame sending

	There are many methods to send frames. Depending on what you want to do, you
	can use one of the following methods:

	\li sendFrame() to send a frame on command port
	\li sendInit() to send a frame on the request port

	These frame are sent immediatelly (after a ~10 ms in truth) unless you
	called delayFrames() with true as parameter.
	However there are the send-immediatelly versions of these methods
	(sendFrameNow() and sendInitNow()) that sends the frames without delay
	although was setted the dalay.

	Could happen that for some reasons is requested to send many frames
	with the same "what". To avoid to flood the openserver, you can use the method
	sendCompressedFrame() which sends only the last frame of the same "what"
	in a time interval.

	\sa manageFrame(), parseFrame(), sendFrame(), sendFrameNow(), sendInit(),
	sendInitNow(), sendCompressedFrame(), delayFrames(), valueReceived()
*/
class device : public QObject, FrameReceiver
{
friend class TestDevice;
friend class BtMain;
Q_OBJECT

public:
	/*!
		\brief default timeout value for compressed frames.
	*/
	static const int COMPRESSION_TIMEOUT = 1000;

	/*!
		\brief Send messages to initialize data.

		Every device should re-implement this method, in order to syncronize the
		device status with the related graphic object one.
	*/
	virtual void init() { }

	/*!
		\brief Returns cache key
		\sa DevicesCache
	*/
	virtual QString get_key();
	virtual ~device() {}

	/*!
		\brief Sets the Client
	*/
	static void setClients(const QHash<int, Clients> &c);

	/*!
		\brief Manages a frame

		Can be reimplemented in order to parse the incoming frames (from the client
		monitor). However, if the specific device can be subclassed, reimplement the
		parseFrame() method in order to avoid a double valueReceived() signal.

		If the frame is handled the valueReceived() signal is emitted.

		\sa parseFrame()
	*/
	virtual void manageFrame(OpenMsg &msg);

	/*!
		\brief Returns true if is connected to the OpenServer.
	*/
	bool isConnected();

	/*!
		\brief Returns the Openserver id.
	*/
	int openserverId();

	/*!
		\brief Calls the init() of all devices.

		\sa init()
	*/
	static void initDevices();

	/*!
		\brief Send (without delay) a \a frame to the Openserver with the given \a id.
	 */
	static void sendCommandFrame(int openserver_id, const QString &frame);

	/*!
		\brief Set this flag to delay the frames sent using the sendFrame()
		sendInit() methods.

		\sa sendFrame(), sendFrameNow(), sendInit(), sendInitNow()
	*/
	static void delayFrames(bool delay);

signals:
	/*!
		\brief Old status changed

		\warning use valueReceived() instead.

		\sa valueReceived()
	 */
	void status_changed(QList<device_status*>);

	/*!
		\brief Signal used to notify that a dimension of device has changed or a
		new command has received.

		For some devices, more than one dimension can change at the same time,
		so the int is used as an enum to recognize the dimensions.

		\note  Using an int is a design choice. In this way the signal is
		generic (so the connections can be made in a generic way) and the enum
		can be specific for a device, avoiding the coupling between abstract
		and concrete device class.

		\sa \ref device-dimensions
	 */
	void valueReceived(const DeviceValues &values_list);

	/*!
		\brief signal emitted when the connection comes up.

		\sa OpenServerManager::connectionUp()
	*/
	void connectionUp();

	/*!
		\brief signal emitted when the connection goes down.

		\sa OpenServerManager::connectionDown()
	*/
	void connectionDown();

public slots:
	/*!
		\brief Sends frames to the openserver, using the Client::COMMAND channel.

		The frame can be really sent after a delay depending on the delayFrames()
		setting.

		\sa delayFrames(), sendFrameNow()
	*/
	void sendFrame(QString frame) const;

	/*!
		\brief Sends frames to the openserver, using the Client::REQUEST channel.

		The frame can be really sent after a delay depending on the delayFrames()
		setting.

		\sa delayFrames(), sendInitNow()
	*/
	void sendInit(QString frame) const;

	/*!
		\brief Sends frames to the openserver, using the Client::COMMAND channel.

		The frame is sent immediatelly not taking care of the delayFrames()
		setting.

		\sa delayFrames(), sendFrame()
	 */
	void sendFrameNow(QString frame) const;

	/*!
		\brief Sends frames to the openserver, using the Client::REQUEST channel.

		The frame is sent immediatelly not taking care of the delayFrames()
		setting.

		\sa delayFrames(), sendFrame()
	 */
	void sendInitNow(QString frame) const;

	/*!
		\brief Sends only one frame of the same "what" into a time interval on
		Client::COMMAND socket.

		\sa sendFrame(), sendCompressedInit(), FrameCompressor
	*/
	void sendCompressedFrame(QString frame, int compression_timeout = COMPRESSION_TIMEOUT) const;

	/*!
		\brief Sends only one frame of the same "what" into a time interval on
		Client::REQUEST socket.

		\sa sendInit(), sendCompressedFrame(), FrameCompressor
	*/
	void sendCompressedInit(QString frame, int compression_timeout = COMPRESSION_TIMEOUT) const;

protected:

	/*!
		\brief Constructor

		Construct a new device with the given \a who, \a where and Openserver id.

		\note The costructor is protected only to make device abstract.
		\note The default openserver id should be keep in sync with the define
		MAIN_OPENSERVER.
	*/
	device(QString who, QString where, int openserver_id = 0);

	/*!
		\brief The (physical) system of the device.
	*/
	QString who;

	/*!
		\brief The address of the device.
	*/
	QString where;

	/*!
		\brief The id of the OpenServer which the device is connected.
	*/
	int openserver_id;

	/*!
		\brief Sends a frame on the Client::COMMAND socket using the given \a what and \a where.
	*/
	void sendCommand(QString what, QString _where) const;

	/*!
		\brief Sends a frame on the Client::COMMAND socket using the given \a what
		and the \a where specified in the device constuctor.
	*/
	void sendCommand(QString what) const;

	/*!
		\brief Sends a frame on the Client::COMMAND socket using the given \a what
		and the \a where specified in the device constuctor.
	*/
	void sendCommand(int what) const;

	/*!
		\brief Sends a frame on the Client::REQUEST socket with the given \a what.
	*/
	void sendRequest(QString what) const;

	/*!
		\brief Sends a frame on the Client::REQUEST socket with the given \a what.
	*/
	void sendRequest(int what) const;

	/*!
		\brief Preferred way to parse incoming frames.

		This method must be reimplemented to handle incoming frames.
		It must return true if the argument frame was recognized and processed,
		false otherwise.
		The default implementation simply returns false.

		\sa manageFrame()
	*/
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list) { return false; }

private:
	static OpenServerManager *getManager(int openserver_id);

private:
	static QHash<int, Clients> clients;
	static QHash<int, OpenServerManager*> openservers;

	FrameCompressor frame_compressor, request_compressor;
};

#endif //__DEVICE_H__
