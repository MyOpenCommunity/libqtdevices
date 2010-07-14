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

#define FRAME_TIMEOUT 10


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


bool Client::delay_frames = false;

Client::Client(Type t, const QString &_host, unsigned _port) : type(t), host(_host)
{
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
	frame_timer.setInterval(FRAME_TIMEOUT);

	connect(&delayed_frame_timer, SIGNAL(timeout()), SLOT(sendDelayedFrames()));
	delayed_frame_timer.setSingleShot(true);
	delayed_frame_timer.setInterval(FRAME_TIMEOUT + (*bt_global::config)[TS_NUMBER].toInt() * TS_NUMBER_FRAME_DELAY);
}

bool Client::isConnected()
{
	// A client of type REQUEST or COMMAND can treat as connected (after the
	// first connection is done) until an error occurs, ignoring the disconnect
	// signal of the underlying socket.
	return is_connected;
}

void Client::socketConnected()
{
	is_connected = true;
	emit connectionUp();

	QMetaEnum e = staticMetaObject.enumerator(0);
	qDebug("Client::socketConnected()[%s]", e.key(type));
	if (type == MONITOR)
		socket->write(SOCKET_MONITOR);
	else if (type == REQUEST)
		socket->write(SOCKET_REQUEST);
	else if (type == SUPERVISOR)
		socket->write(SOCKET_SUPERVISOR);
	else
		socket->write(SOCKET_COMMAND);
}

void Client::delayFrames(bool delay)
{
	delay_frames = delay;
}

void Client::sendFrames()
{
	sendFrames(list_frames);
	list_frames.clear();
}

void Client::sendDelayedFrames()
{
	sendFrames(delayed_list_frames);
	delayed_list_frames.clear();
}

void Client::sendFrames(const QList<QByteArray> &to_send)
{
	// The openserver closes the connection with sockets of type REQUEST/COMMAND
	// after 30 seconds of inactivity, while it doesn't close connections of
	// type MONITOR/SUPERVISOR. So, a client of type COMMAND or REQUEST should
	// treat as connected even if the read underlying socket is disconnected
	// without errors.
	if (!is_connected)
		return;

	if (socket->state() == QAbstractSocket::UnconnectedState || socket->state() == QAbstractSocket::ClosingState)
		connectToHost();

	// We assume that 100 milliseconds are a reasonable time to connect without problems.
	if (!socket->waitForConnected(100))
	{
		is_connected = false;
		emit connectionDown();
		return;
	}

	QSet<QByteArray> discard_duplicates;

	foreach (const QByteArray &frame, to_send)
	{
		if (discard_duplicates.contains(frame))
			continue;
		discard_duplicates.insert(frame);

		int written = socket->write(frame);
		if (written == -1)
		{
			if (host != OPENSERVER_ADDR)
				qWarning() << "Unable to send the frame:" << frame << QString("to [%1:%2]").arg(host).arg(port);
			else
				qWarning() << "Unable to send the frame:" << frame;
			continue;
		}

		if (host != OPENSERVER_ADDR)
			qDebug() << qPrintable(QString("Client::sendFrameOpen()[%1:%2]").arg(host).arg(port)) << "sent:" << frame;
		else
			qDebug() << "Client::sendFrameOpen() sent:" << frame;
	}
}

void Client::sendFrameOpen(const QString &frame_open)
{
	QByteArray frame = frame_open.toLatin1();
	// queue the frames to be sent later, but avoid delaying frames indefinitely
	QList<QByteArray> &list = delay_frames ? delayed_list_frames : list_frames;
	QTimer &timer = delay_frames ? delayed_frame_timer : frame_timer;

	list.append(frame);
	if (!timer.isActive())
		timer.start();
}

void Client::disconnectFromHost()
{
	qDebug() << "Client::disconnectFromHost()";
	socket->abort();
}

void Client::connectToHost()
{
	qDebug() << "Client::connectToHost(), host: " << host << ", port: " << port;
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
		if (host != OPENSERVER_ADDR)
			qDebug() << qPrintable(QString("Client::manageFrame()[%1:%2]").arg(host).arg(port)) << "read:" << frame;
		else
			qDebug() << "Client::manageFrame() read:" << frame;

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

void Client::subscribe(FrameReceiver *obj, int who)
{
	subscribe_list[who].append(obj);
}

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
	qDebug("Client::socketFrameRead()");

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

// Aspetta ack
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
		QMetaEnum e = staticMetaObject.enumerator(0);

		qWarning() << qPrintable(QString("OpenClient [%1:%2]: error").arg(host).arg(port))
			<< socket->errorString() << "occurred on client" << e.key(type);

		is_connected = false;
		emit connectionDown();
	}
}

