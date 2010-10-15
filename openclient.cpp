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


#include "openclient.h"
#include "frame_receiver.h"
#include "main.h" // bt_global::config, TS_NUMBER_FRAME_DELAY

#include <openmsg.h>

#include <QDebug>
#include <QMetaEnum>
#include <QMetaObject>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define SOCKET_MONITOR "*99*1##"
#define SOCKET_SUPERVISOR "*99*10##"
#define SOCKET_COMMAND "*99*9##"
#define SOCKET_REQUEST "*99*0##"

#define FRAME_TIMEOUT_MSECS 10

#define CONNECTION_TIMEOUT_SECS 25

namespace
{
	// TODO: make this function cross platform and move in hardware functions!
	bool setTcpKeepaliveParams(int s, bool enable = true)
	{
		int idle = 3; // seconds from the setsockopt call
		int interval = 10; // seconds
		int count = 2; // the number of times after that the socket is not alive.
		int optval = enable ? 1 : 0;
		if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0)
			return false;

		if (enable)
		{
			if (setsockopt(s, SOL_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)) < 0 ||
				setsockopt(s, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) < 0 ||
				setsockopt(s, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count)) < 0)
				return false;
		}
		return true;
	}
}

/*!
	\ingroup Core
	\class Client
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

/*!
	\enum Client::Type
	Defines the type of the client.
*/
/*!
	\var Client::Type Client::MONITOR
	Used to receive frames from almost all systems.
*/
/*!
	\var Client::Type Client::COMMAND
	Used to send frames to the openserver.
*/
/*!
	\var Client::Type Client::REQUEST
	Used to send frames to the openserver.
	\note the frames sent during the programmation of a scenario module are not
	delivered.
*/
/*!
	\var Client::Type Client::SUPERVISOR
	Used to receive frames that came from the socket NOTIFY.
*/

/*!
	\enum Client::FrameDelay
	Defines the type of the delay that can used while sent frames.
*/
/*!
	\var Client::FrameDelay Client::DELAY_NONE
	No delay allowed
*/
/*!
	\var Client::FrameDelay Client::DELAY_IF_REQUESTED
	Delay if the requested from delayFrames()
*/


bool Client::delay_frames = false;

/*!
	\brief Constructor

	It builds a new Client of Client::Type \a t and connect it to the openserver
	having the given \a host and \a port.
*/
Client::Client(Type t, const QString &_host, unsigned _port) : type(t), host(_host)
{
	QMetaEnum e = staticMetaObject.enumerator(0);
	if (host != OPENSERVER_ADDR)
		description = QString("%1 [%2:%3]").arg(e.key(type)).arg(host).arg(port);
	else
		description = e.key(type);

	port = !_port ? OPENSERVER_PORT : _port;
	is_connected = false;

	to_forward = 0;

	socket = new QTcpSocket(this);
	connect(socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(socket, SIGNAL(readyRead()), SLOT(socketFrameRead()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));

	// connect to the server
	connectToHost();

	// set up the timer for normal and delayed frame sending
	connect(&frame_timer, SIGNAL(timeout()), SLOT(sendFrames()));
	frame_timer.setSingleShot(true);
	frame_timer.setInterval(FRAME_TIMEOUT_MSECS);

	connect(&delayed_frame_timer, SIGNAL(timeout()), SLOT(sendDelayedFrames()));
	delayed_frame_timer.setSingleShot(true);
	delayed_frame_timer.setInterval(FRAME_TIMEOUT_MSECS + (*bt_global::config)[TS_NUMBER].toInt() * TS_NUMBER_FRAME_DELAY);
}

/*!
	\brief Check if the client is connected
*/
bool Client::isConnected()
{
	// The openserver closes the connection for sockets of type REQUEST/COMMAND
	// after 30 seconds of inactivity, while it doesn't close connections of
	// type MONITOR/SUPERVISOR. So, a client of type COMMAND or REQUEST should
	// treat as connected even if the underlying socket is disconnected without
	// errors.
	return is_connected;
}

void Client::socketConnected()
{
	if (!is_connected)
	{
		// Signals in the same thread are dispatched immediately, so we have
		// to set the is_connected variable before the emission of the signal
		// connectionUp (in order to have a right result for the isConnected
		// method).
		is_connected = true;
		emit connectionUp();
	}
	else
		is_connected = true;

	// Sometimes happens that the QTCPSocket thinks that the connection is still
	// up while the openserver has already closes its side of the socket.
	// To prevent losing frames we use a time counter and manually disconnect the
	// connection after CONNECTION_TIMEOUT_SECS of inactivity.
	// NOTE: we cannot use a QTimer because at the startup sometimes the timeout
	// signal has dispatched too late.
	if (type == COMMAND || type == REQUEST)
		inactivity_time.start();

	qDebug() << "Client::socketConnected()" << qPrintable(description);

	if (type == MONITOR)
		socket->write(SOCKET_MONITOR);
	else if (type == REQUEST)
		socket->write(SOCKET_REQUEST);
	else if (type == SUPERVISOR)
		socket->write(SOCKET_SUPERVISOR);
	else
		socket->write(SOCKET_COMMAND);

	if (!send_on_connected.isEmpty() && sendFrames(send_on_connected))
		send_on_connected.clear();
}

/*!
	\brief delay the frames that are allowed to \sa sendFrameOpen()
*/
void Client::delayFrames(bool delay)
{
	delay_frames = delay;
}

void Client::sendFrames()
{
	if (!sendFrames(list_frames))
		send_on_connected.append(list_frames);

	list_frames.clear();
}

void Client::sendDelayedFrames()
{
	if (!sendFrames(delayed_list_frames))
		send_on_connected.append(delayed_list_frames);

	delayed_list_frames.clear();
}

bool Client::sendFrames(const QList<QByteArray> &to_send)
{
	if (socket->state() == QAbstractSocket::ConnectedState && (type == COMMAND || type == REQUEST) &&
		inactivity_time.elapsed() > CONNECTION_TIMEOUT_SECS * 1000)
	{
		// We can safely call abort because if we aren't sending frames for
		// CONNECTION_TIMEOUT_SECS we won't lose frame. The abort call drop immediately
		// the connection, so after the call the socket is unconnected.
		socket->abort();
		qDebug() << "Disconnect for inactivity of" << inactivity_time.elapsed() / 1000
			<< "seconds on client" << qPrintable(description);
	}

	QAbstractSocket::SocketState state = socket->state();
	if (state == QAbstractSocket::UnconnectedState || state == QAbstractSocket::ClosingState ||
		state == QAbstractSocket::HostLookupState || state == QAbstractSocket::ConnectingState)
	{
		if (state == QAbstractSocket::UnconnectedState || state == QAbstractSocket::ClosingState)
			connectToHost();
		return false;
	}

	QSet<QByteArray> discard_duplicates;

	foreach (const QByteArray &frame, to_send)
	{
		if (discard_duplicates.contains(frame))
			continue;
		discard_duplicates.insert(frame);

		int written = socket->write(frame);
		if (written == -1)
			qWarning() << "Unable to send the frame" << frame << "to" << qPrintable(description);
		else
			qDebug() << "Client::sendFrameOpen()" << qPrintable(description) << "sent:" << frame;
	}

	if (type == COMMAND || type == REQUEST)
		inactivity_time.start();

	return true;
}

/*!
	\brief Send a frame to the openserver.

	Send a \a frame_open. The frame is sent immediately if \a delay is FrameDelay::DELAY_NONE,
	othewise can be delayed depending on the status set via delayFrames()
*/
void Client::sendFrameOpen(const QString &frame_open, FrameDelay delay)
{
	QByteArray frame = frame_open.toLatin1();

	if (!is_connected)
	{
		qWarning() << "Client::sendFrameOpen try to send the frame" << frame_open
			<< "in unconnected state for client" << qPrintable(description);
		send_on_connected.append(frame);
		return;
	}

	// queue the frames to be sent later, but avoid delaying frames indefinitely
	QList<QByteArray> &list = delay_frames && delay == DELAY_IF_REQUESTED ? delayed_list_frames : list_frames;
	QTimer &timer = delay_frames && delay == DELAY_IF_REQUESTED ? delayed_frame_timer : frame_timer;

	list.append(frame);
	if (!timer.isActive())
		timer.start();
}

/*!
	\fn Client::connectionDown()

	\brief Notifies when the connection is lost.
*/

/*!
	\brief Disconnect from the openserver
*/
void Client::disconnectFromHost()
{
	qDebug() << "Client::disconnectFromHost()" << qPrintable(description);
	socket->disconnectFromHost();
	is_connected = false;
}

/*!
	\fn Client::connectionUp()

	\brief Notifies when the connection is established.
*/

/*!
	\brief Connect to the openserver
*/
void Client::connectToHost()
{
	qDebug() << "Client::connectToHost()" << qPrintable(description);
	socket->connectToHost(host, port);
	if (socket->socketDescriptor() != -1)
		setTcpKeepaliveParams(socket->socketDescriptor());
}

QByteArray Client::readFromServer()
{
	data_read += socket->readAll();
	int pos = data_read.indexOf("##");
	if (pos == -1)
		return QByteArray();
	else
	{
		QByteArray buf = data_read.left(pos + 2);
		data_read.remove(0, pos + 2);
		return buf;
	}
}

void Client::manageFrame(QByteArray frame)
{
	if (type == MONITOR || type == SUPERVISOR)
	{
		qDebug() << "Client::manageFrame()" << qPrintable(description) << "read:" << frame;

		if (frame == "*#*1##")
			qWarning("ERROR - ack received");
		else if (frame == "*#*0##")
			qWarning("ERROR - nak received");
		else
			dispatchFrame(frame);
	}
	else
	{
		if (frame == "*#*1##")
		{
			qDebug("ack received");
			emit openAckRx();
		}
		else if (frame == "*#*0##")
		{
			qDebug("nak received");
			emit openNakRx();
		}
	}
}

/*!
	\brief Forward the frames received from a Client object to another one.
*/
void Client::forwardFrame(Client *c)
{
	to_forward = c;
}

void Client::dispatchFrame(QString frame)
{
	if (to_forward)
	{
		to_forward->dispatchFrame(frame);
		return;
	}

	OpenMsg msg;
	msg.CreateMsgOpen(frame.toAscii().data(), frame.length());
	delay_frames = true;
	if (subscribe_list.contains(msg.who()))
	{
		QList<FrameReceiver*> &l = subscribe_list[msg.who()];
		for (int i = 0; i < l.size(); ++i)
			l[i]->manageFrame(msg);
	}
	delay_frames = false;
}

/*!
	\brief Request to receive frames having \a who.

	Note that a FrameReceiver can ask to receive frames for one or more who.
*/
void Client::subscribe(FrameReceiver *obj, int who)
{
	subscribe_list[who].append(obj);
}

/*!
	\brief Request to not receive frames anymore.
*/
void Client::unsubscribe(FrameReceiver *obj)
{
	// A frame receiver can be subscribed for one or more "who".
	QMutableHashIterator<int, QList<FrameReceiver*> > it(subscribe_list);
	while (it.hasNext())
	{
		it.next();
		QMutableListIterator<FrameReceiver*> it_list(it.value());
		while (it_list.hasNext())
		{
			it_list.next();
			if (it_list.value() == obj)
				it_list.remove();
		}
	}
}

int Client::socketFrameRead()
{
	while (true)
	{
		QByteArray frame = readFromServer();

		while (frame.endsWith('\n') || frame.endsWith('\r'))
			frame.chop(1);

		if (frame.isEmpty())
			break;

		manageFrame(frame);
	}
	return 0;
}

int Client::socketWaitForAck()
{
	qDebug("Client::socketWaitForAck()");

	if (type == MONITOR || type == SUPERVISOR)
		return -1;
	ackRx = false;
	connect(this, SIGNAL(openAckRx()), this, SLOT(ackReceived()));
	int stat;
	if ((stat = socketFrameRead()) < 0)
		return stat;
	disconnect(this, SIGNAL(openAckRx()), this, SLOT(ackReceived()));
	return ackRx ? 0 : -1;
}

void Client::ackReceived()
{
	ackRx = true;
}

void Client::socketError(QAbstractSocket::SocketError e)
{
	if (!is_connected)
		return;

	if (e != QAbstractSocket::RemoteHostClosedError || type == MONITOR || type == SUPERVISOR)
	{
		qWarning() << "Client: error" << socket->errorString() << "occurred on client" << qPrintable(description);
		is_connected = false;
		emit connectionDown();
	}
}

