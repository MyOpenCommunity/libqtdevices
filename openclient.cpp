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
#include "frame_classes.h"

#include <openmsg.h>

#include <QDebug>
#include <QMetaEnum>
#include <QMetaObject>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

// The channels id
#define SOCKET_MONITOR "*99*1##"
#define SOCKET_SUPERVISOR "*99*10##"
#define SOCKET_COMMAND "*99*9##"
#define SOCKET_REQUEST "*99*0##"


// The time after that the connection in the channels COMMAND and REQUEST is
// invalid (see the comment in ClientWriter::socketConnected)
#define CONNECTION_TIMEOUT_SECS 25

// The openserver sent an ack/nak not only for frame, but also on the connection
// and when receiving the channel id. We use the __NONE__ to discard these informations.
#define __NONE__ "NONE"


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

	template <class T> void unsubscribe(QHash<int, QList<T*> > &container, T *obj)
	{
		// An object can be subscribed more times.
		QMutableHashIterator<int, QList<T*> > it(container);
		while (it.hasNext())
		{
			it.next();
			QMutableListIterator<T*> it_list(it.value());
			while (it_list.hasNext())
			{
				it_list.next();
				if (it_list.value() == obj)
					it_list.remove();
			}
		}
	}
}


Client::Client(Type t, const QString &_host, unsigned _port) : type(t), host(_host)
{
	QMetaEnum e = staticMetaObject.enumerator(0);
	if (host != OPENSERVER_ADDR)
		description = QString("%1 [%2:%3]").arg(e.key(type)).arg(host).arg(port);
	else
		description = e.key(type);

	port = !_port ? OPENSERVER_PORT : _port;
	is_connected = false;

	socket = new QTcpSocket(this);
	connect(socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	// The readyRead signal is not emitted recursively: in other words if a slot called by this signal
	// has a local loop or process events, if a frame arrives the signal is not emitted so the frame
	// is not parsed.
	// In the Vct Full Ip, we have exactly this situation: when an alarm arrives during a videocall, we
	// have to end the call and wait for a frame which notifies that the openserver has successfully
	// processed the command. To manage that situation, we break the recursion using a queued slot.
	connect(socket, SIGNAL(readyRead()), SLOT(socketFrameRead()), Qt::QueuedConnection);

	// connect to the server
	connectToHost();
}

bool Client::isConnected()
{
	// The openserver closes the connection for sockets of type REQUEST/COMMAND
	// after 30 seconds of inactivity, while it doesn't close connections of
	// type MONITOR/SUPERVISOR. So, a client of type COMMAND or REQUEST should
	// treat as connected even if the underlying socket is disconnected without
	// errors.
	return is_connected;
}

bool Client::isConnecting()
{
	return socket->state() == QAbstractSocket::HostLookupState ||
	       socket->state() == QAbstractSocket::ConnectingState;
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

	qDebug() << "Client::socketConnected()" << qPrintable(description);

	sendChannelId();
}

void Client::disconnectFromHost()
{
	qDebug() << "Client::disconnectFromHost()" << qPrintable(description);
	socket->disconnectFromHost();
	is_connected = false;
}

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

void Client::socketFrameRead()
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


ClientReader::ClientReader(Type t, const QString &host, unsigned port) : Client(t, host, port)
{
	to_forward = 0;
}

void ClientReader::sendChannelId()
{
	if (type == MONITOR)
		socket->write(SOCKET_MONITOR);
	else if (type == SUPERVISOR)
		socket->write(SOCKET_SUPERVISOR);
	else
		Q_ASSERT_X(false, "ClientReader::sendChannelId", "Unknown type");
}

void ClientReader::manageFrame(QByteArray frame)
{
	qDebug() << "ClientReader::manageFrame()" << qPrintable(description) << "read:" << frame;

	if (frame != ACK_FRAME && frame != NAK_FRAME)
	{
		if (to_forward)
		{
			to_forward->manageFrame(frame);
			return;
		}

		OpenMsg msg;
		msg.CreateMsgOpen(frame.data(), frame.length());
		if (subscribe_list.contains(msg.who()))
		{
			QList<FrameReceiver*> &l = subscribe_list[msg.who()];
			for (int i = 0; i < l.size(); ++i)
				l[i]->manageFrame(msg);
		}
	}
}

void ClientReader::forwardFrame(ClientReader *c)
{
	to_forward = c;
}

void ClientReader::subscribe(FrameReceiver *obj, int who)
{
	subscribe_list[who].append(obj);
}

void ClientReader::unsubscribe(FrameReceiver *obj)
{
	::unsubscribe(subscribe_list, obj);
}



bool ClientWriter::delay_frames = false;
int ClientWriter::delay_msecs = -1;

ClientWriter::ClientWriter(Type t, const QString &host, unsigned port) : Client(t, host, port)
{
	// set up the timer for normal and delayed frame sending
	connect(&frame_timer, SIGNAL(timeout()), SLOT(sendFrames()));
	frame_timer.setSingleShot(true);
	frame_timer.setInterval(STANDARD_FRAME_DELAY);

	connect(&delayed_frame_timer, SIGNAL(timeout()), SLOT(sendDelayedFrames()));
	delayed_frame_timer.setSingleShot(true);
	// The delayed interval is set lazy in the sendFrameOpen method
}

void ClientWriter::delayFrames(bool delay)
{
	delay_frames = delay;
}

void ClientWriter::setDelay(int msecs)
{
	delay_msecs = msecs;
}

void ClientWriter::sendChannelId()
{
	QByteArray frame;
	if (type == REQUEST)
		frame = SOCKET_REQUEST;
	else if (type == COMMAND)
		frame = SOCKET_COMMAND;
	else
		Q_ASSERT_X(false, "ClientWriter::sendChannelId", "Unknown type");
	socket->write(frame);
	ack_source_list.append(__NONE__);
}

void ClientWriter::manageFrame(QByteArray frame)
{
	if (frame == ACK_FRAME || frame == NAK_FRAME)
	{
		bool ack = frame == ACK_FRAME;
		Q_ASSERT_X(ack_source_list.size() > 0, "ClientWriter::manageFrame", "Empty ack source list!");
		QByteArray actual_frame = ack_source_list.takeFirst();
		if (actual_frame != __NONE__)
		{
			qDebug() << "ClientWriter::manageFrame()" << qPrintable(description) << (ack ? "ACK" : "NAK")
				<< "for:" << actual_frame;

			OpenMsg msg;
			msg.CreateMsgOpen(actual_frame.data(), actual_frame.length());
			if (ack_receivers.contains(msg.who()))
			{
				QList<FrameSender*> &l = ack_receivers[msg.who()];
				for (int i = 0; i < l.size(); ++i)
				{
					if (ack)
						l[i]->manageAck(msg);
					else
						l[i]->manageNak(msg);
				}
			}
		}
	}
}

void ClientWriter::sendFrameOpen(const QString &frame_open, FrameDelay delay)
{
	QByteArray frame = frame_open.toLatin1();

	if (!isConnected())
	{
		qWarning() << "Client::sendFrameOpen try to send the frame" << frame_open
			<< "in unconnected state for client" << qPrintable(description);
		send_on_connected.append(frame);
		return;
	}

	if (delay_frames && delayed_frame_timer.interval() != STANDARD_FRAME_DELAY + delay_msecs)
	{
		Q_ASSERT_X(delay_msecs != -1, "ClientWriter::sendFrameOpen", "Trying to send delayed frame with delay not set!");
		delayed_frame_timer.setInterval(STANDARD_FRAME_DELAY + delay_msecs);
	}

	// queue the frames to be sent later, but avoid delaying frames indefinitely
	QList<QByteArray> &list = delay_frames && delay == DELAY_IF_REQUESTED ? delayed_list_frames : list_frames;
	QTimer &timer = delay_frames && delay == DELAY_IF_REQUESTED ? delayed_frame_timer : frame_timer;

	list.append(frame);
	if (!timer.isActive())
		timer.start();
}

void ClientWriter::socketConnected()
{
	ack_source_list.append(__NONE__);
	Client::socketConnected();

	// Sometimes happens that the QTCPSocket thinks that the connection is still
	// up while the openserver has already closes its side of the socket.
	// To prevent losing frames we use a time counter and manually disconnect the
	// connection after CONNECTION_TIMEOUT_SECS of inactivity.
	// NOTE: we cannot use a QTimer because at the startup sometimes the timeout
	// signal has dispatched too late.
	inactivity_time.start();

	qDebug() << "ClientWriter::socketConnected()" << qPrintable(description);

	if (!send_on_connected.isEmpty() && sendFrames(send_on_connected))
		send_on_connected.clear();
}

void ClientWriter::sendFrames()
{
	if (!sendFrames(list_frames))
		send_on_connected.append(list_frames);

	list_frames.clear();
}

void ClientWriter::sendDelayedFrames()
{
	if (!sendFrames(delayed_list_frames))
		send_on_connected.append(delayed_list_frames);

	delayed_list_frames.clear();
}

bool ClientWriter::sendFrames(const QList<QByteArray> &to_send)
{
	if (socket->state() == QAbstractSocket::ConnectedState && inactivity_time.elapsed() > CONNECTION_TIMEOUT_SECS * 1000)
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
		{
			qDebug() << "ClientWriter::sendFrameOpen()" << qPrintable(description) << "sent:" << frame;
			ack_source_list.append(frame);
		}
	}

	inactivity_time.start();
	return true;
}

void ClientWriter::subscribeAck(FrameSender *obj, int who)
{
	ack_receivers[who].append(obj);
}

void ClientWriter::unsubscribeAck(FrameSender *obj)
{
	::unsubscribe(ack_receivers, obj);
}

