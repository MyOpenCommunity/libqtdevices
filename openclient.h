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

#ifndef OPENCLIENT_H
#define OPENCLIENT_H

#include <QTcpSocket> // QAbstractSocket
#include <QByteArray>
#include <QHash>
#include <QTimer>
#include <QTime>

#ifndef OPENSERVER_ADDR
#define OPENSERVER_ADDR "127.0.0.1"
#endif

#define OPENSERVER_PORT 20000

// The id for the main openserver
#define MAIN_OPENSERVER 0

class FrameReceiver;
class FrameSender;

class QTcpSocket;

/*!
	\ingroup Core
	\brief Manages the communication between the application and a specified openserver.

	This abstract class has two specializations:
	- the ClientWriter, that can be used to send frames to the openserver
	- the ClientReader, to receive frames from the openserver.

	Each client when created tries to connect itself to the specified openserver,
	but you can connect or disconnect in every moment using the connectToHost()
	and disconnectFromHost() methods. The signal connectionUp() notify when
	the connection is established, while the connectionDown() is emitted when
	the connection is lost (but not after the call of the disconnectFromHost
	method).
	\sa ClientReader, ClientWriter
*/
class Client : public QObject
{
friend class OpenServerMock;
Q_OBJECT
Q_ENUMS(Type)

public:
	// the enum Type should be the first (or you have to modify the use of QMetaEnum)
	/*!
		\brief Defines the type of the client.
	*/
	enum Type
	{
		MONITOR = 0, /*!< Used to receive frames from almost all systems. */
		SUPERVISOR,  /*!< Used to receive frames from the socket NOTIFY. */
		REQUEST,     /*!< Used to send frames to the openserver.
		                  \note The frames sent during the programmation of a scenario module are not delivered. */
		COMMAND      /*!< Used to send frames to the openserver. */
	};

	/*!
		\brief Check if the client is connected
	*/
	bool isConnected();

#if DEBUG
	int bytesAvailable() { return socket->bytesAvailable(); }
	virtual void flush() { socket->flush(); }
#endif

public slots:
	/*!
		\brief Connect to the openserver
	*/
	void connectToHost();

	/*!
		\brief Disconnect from the openserver
	*/
	void disconnectFromHost();

signals:
	/*!
		\brief Notifies when the connection is established.
	*/
	void connectionUp();

	/*!
		\brief Notifies when the connection is lost.
	*/
	void connectionDown();

protected:
	Client(Type t, const QString &host, unsigned port);

	virtual void sendChannelId() = 0;
	virtual void manageFrame(QByteArray frame) = 0;

	// The channel description
	QString description;

	// The actual socket instance
	QTcpSocket *socket;

	// The channel type
	Type type;

protected slots:
	virtual void socketConnected();

private slots:
	// Reads messages from the socket
	void socketFrameRead();

	void socketError(QAbstractSocket::SocketError e);

private:
	// The address of the openserver
	QString host;
	// The port of the openserver
	unsigned port;

	// The buffer that store the data read from the server
	QByteArray data_read;

	// This flag marks if the client is logically connected or not.
	bool is_connected;

	QByteArray readFromServer();
};



/*!
	\ingroup Core
	\brief Manages the read-only part of the communication with the openserver.

	To receive a frame inherit from the FrameReceiver class and subscribe the
	object for a who. The forwardFrame method can be used to receive frames from
	an object to anothe object.

	\sa Client
*/
class ClientReader : public Client
{
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:
	/*!
		\brief Constructor

		It builds a new ClientReader of Client::Type \a t and connect it to the openserver
		having the given \a host and \a port.
	*/
	ClientReader(Type t, const QString &host = OPENSERVER_ADDR, unsigned port = 0);

	/*!
		\brief Forward the frames received from a ClientReader object to another one.
	*/
	void forwardFrame(ClientReader *c);

	/*!
		\brief Request to receive frames having \a who.

		When a frame is received from the openserver, the FrameReceiver::manageFrame()
		method is called for every object that has subscribed itself.
		Note that a FrameReceiver can ask to receive frames for one or more who.
	*/
	void subscribe(FrameReceiver *obj, int who);

	/*!
		\brief Request to not receive frames anymore.
	*/
	void unsubscribe(FrameReceiver *obj);

protected:
	virtual void sendChannelId();
	virtual void manageFrame(QByteArray frame);

private:
	// The client where forwards the frames received.
	ClientReader *to_forward;

	// The list of the FrameReceivers that will receive the incoming frames.
	QHash<int, QList<FrameReceiver*> > subscribe_list;
};


/*!
	\ingroup Core
	\brief Manages the write-only part of the communication with the openserver.

	To send a frame use the sendFrameOpen() method, using the optional argument
	delay to force the %ClientWriter to send the frame immediately.
*/
class ClientWriter : public Client
{
Q_OBJECT
public:
	/*!
		\brief Constructor

		It builds a new ClientReader of Client::Type \a t and connect it to the openserver
		having the given \a host and \a port.
	*/
	ClientWriter(Type t, const QString &host = OPENSERVER_ADDR, unsigned port = 0);

	/*!
		\brief Defines the type of the delay that can used while sent frames.
	*/
	enum FrameDelay
	{
		DELAY_NONE,          /*!< No delay allowed */
		DELAY_IF_REQUESTED   /*!< Delay if the requested from delayFrames() */
	};

	/*!
		\brief Delay all the frames marked as DELAY_IF_REQUESTED are delayed.
		\sa FrameDelay, sendFrameOpen(), setDelay()
	*/
	static void delayFrames(bool delay);

	/*!
		\brief Set the delay used when the frame are delayed
		\sa FrameDelay, delayFrames(), sendFrameOpen()
	*/
	static void setDelay(int msecs);

	/*!
		\brief Send a frame to the openserver.

		Send a \a frame_open. The frame is sent immediately if \a delay is FrameDelay::DELAY_NONE,
		othewise can be delayed depending on the status set via delayFrames()
		\sa delayFrames(), setDelay()
	*/
	void sendFrameOpen(const QString &frame_open, FrameDelay delay = DELAY_IF_REQUESTED);

#if DEBUG
	virtual void flush() { sendDelayedFrames(); sendFrames(); Client::flush(); }
#endif

	/*!
		\brief Request to receive notifications for ack/nack.
		When a ack, which indicates that a command is successfully processed, or its opposite nak
		is received from the openserver, the FrameSender::manageAck() or FrameSender::manageNak()
		is called for every object that has subscribed itself.

		Note that a FrameSender can ask to receive notifications for one or more who.
	*/
	void subscribeAck(FrameSender *obj, int who);

	/*!
		\brief Request to not receive notifications for ack/nack anymore.
	*/
	void unsubscribeAck(FrameSender *obj);

protected:
	virtual void sendChannelId();
	virtual void manageFrame(QByteArray frame);

protected slots:
	virtual void socketConnected();

private slots:
	// sends queued frames, removing duplicates
	void sendDelayedFrames();
	void sendFrames();

private:
	// gather sent frames to be sent later; using a list because frame order is important
	QList<QByteArray> list_frames, delayed_list_frames;
	QTimer frame_timer, delayed_frame_timer;

	// The list of the frames to send as soon as the connection is established.
	QList<QByteArray> send_on_connected;

	// The time since the last frame sent (only for REQUEST and COMMAND)
	QTime inactivity_time;

	QList<QByteArray> ack_source_list;

	QHash<int, QList<FrameSender*> > ack_receivers;

	// True if we need to delay the frames
	static bool delay_frames;

	// The msecs of delay used when sending frames (if delay_frames is true)
	static int delay_msecs;

	// try to send the argument frames and return true on success.
	bool sendFrames(const QList<QByteArray> &to_send);
};


#endif
