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

#include <openwebnet.h>

#include <QTcpSocket>
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

// TODO: frameDElay!

/*!
	\ingroup Core
	\brief Manages the communication between the application and a specified openserver.

	%Clients of types Client::MONITOR / Client::SUPERVISOR can be used to receive
	frames while clients of types Client::COMMAND or Client::REQUEST can be used
	to send frames to the openserver.

	Each client when created tries to connect itself to the specified openserver,
	but you can connect or disconnect in every moment using the connectToHost()
	and disconnectFromHost() methods. The signal connectionUp() notify when
	the connection is established, while the connectionDown() is emitted when
	the connection is lost (but not after the call of the disconnectFromHost
	method).

	To send a frame use the sendFrameOpen() method.

	To receive a frame you have to inherit from the FrameReceiver class and
	subscribe the derived object for a who.
*/
class Client : public QObject
{
friend class OpenServerMock;
friend class TestScenEvoDevicesCond;
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
		\brief Defines the type of the delay that can used while sent frames.
	*/
	enum FrameDelay
	{
		DELAY_NONE,          /*!< No delay allowed */
		DELAY_IF_REQUESTED   /*!< Delay if the requested from delayFrames() */
	};

	/*!
		\brief Constructor

		It builds a new Client of Client::Type \a t and connect it to the openserver
		having the given \a host and \a port.
	*/
	Client(Type t, const QString &host, unsigned port);

	/*!
		\brief Send a frame to the openserver.

		Send a \a frame_open. The frame is sent immediately if \a delay is FrameDelay::DELAY_NONE,
		othewise can be delayed depending on the status set via delayFrames()
	*/
	virtual void sendFrameOpen(const QString &frame_open, FrameDelay delay = DELAY_IF_REQUESTED) = 0;

	/*!
		\brief Request to receive frames having \a who.

		Note that a FrameReceiver can ask to receive frames for one or more who.
	*/
	virtual void subscribe(FrameReceiver *obj, int who) = 0;

	/*!
		\brief Request to not receive frames anymore.
	*/
	virtual void unsubscribe(FrameReceiver *obj) = 0;

	/*!
		\brief Check if the client is connected
	*/
	bool isConnected();

#if DEBUG
	int bytesAvailable() { return socket->bytesAvailable(); }
	virtual void flush() { socket->flush(); }
#endif

	/*!
		\brief Forward the frames received from a Client object to another one.
	*/
	virtual void forwardFrame(Client *c) = 0;

	/*!
		\brief Delay all the frames marked as DELAY_IF_REQUESTED are delayed.
		\sa FrameDelay, sendFrameOpen()
	*/
	static void delayFrames(bool delay);

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
	virtual void manageFrame(const QByteArray &frame) = 0;
	static bool delay_frames;

	// The channel description
	QString description;

	QTcpSocket *socket;

	// This flag marks if the client is logically connected or not.
	bool is_connected;

protected slots:
	virtual void socketConnected();

private slots:
	// Reads messages from the socket
	void socketFrameRead();

	void socketError(QAbstractSocket::SocketError e);

private:
	// The channel type
	Type type;

	// The address of the openserver
	QString host;
	// The port of the openserver
	unsigned port;

	// The buffer that store the data read from the server
	QByteArray data_read;

	QByteArray readFromServer();

};


class ClientReader : public Client
{
Q_OBJECT
public:
	ClientReader(Type t, const QString &host, unsigned port);
	virtual void forwardFrame(Client *c);

	/*!
		\brief Request to receive frames having \a who.

		Note that a FrameReceiver can ask to receive frames for one or more who.
	*/
	virtual void subscribe(FrameReceiver *obj, int who);

	/*!
		\brief Request to not receive frames anymore.
	*/
	virtual void unsubscribe(FrameReceiver *obj);

	virtual void sendFrameOpen(const QString &frame_open, FrameDelay delay = DELAY_IF_REQUESTED);

protected:
	virtual void manageFrame(const QByteArray &frame);

private:
	// The client where forwards the frames received.
	ClientReader *to_forward;

	// The list of the FrameReceivers that will receive the incoming frames.
	QHash<int, QList<FrameReceiver*> > subscribe_list;

	void dispatchFrame(const QByteArray &frame);
};


class ClientWriter : public Client
{
Q_OBJECT
public:
	ClientWriter(Type t, const QString &host, unsigned port);
	virtual void forwardFrame(Client *c);

	virtual void subscribe(FrameReceiver *obj, int who);
	virtual void unsubscribe(FrameReceiver *obj);

	/*!
		\brief Send a frame to the openserver.

		Send a \a frame_open. The frame is sent immediately if \a delay is FrameDelay::DELAY_NONE,
		othewise can be delayed depending on the status set via delayFrames()
	*/
	virtual void sendFrameOpen(const QString &frame_open, FrameDelay delay = DELAY_IF_REQUESTED);

#if DEBUG
	virtual void flush() { sendDelayedFrames(); sendFrames(); Client::flush(); }
#endif

protected:
	virtual void manageFrame(const QByteArray &frame);

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

	// try to send the argument frames and return true on success.
	bool sendFrames(const QList<QByteArray> &to_send);
};


Client *getClient(Client::Type t, const QString &host = OPENSERVER_ADDR, unsigned port = 0);


#endif
