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
#include "hardware_functions.h" // rearmWDT
#include "frame_receiver.h"

#include <openmsg.h>

#include <QTimer>
#include <QDebug>

#define SOCKET_MONITOR "*99*1##"
#define SOCKET_SUPERVISOR "*99*10##"
#define SOCKET_COMANDI "*99*9##"
#define SOCKET_RICHIESTE "*99*0##"


Client::Client(Type t, const QString &_host, unsigned _port) : type(t), host(_host)
{
	port = !_port ? OPENSERVER_PORT : _port;

#if DEBUG
	to_forward = 0;
#endif

	socket = new QTcpSocket(this);

	connect(socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(socket, SIGNAL(disconnected()), SLOT(socketConnectionClosed()));
	connect(socket, SIGNAL(readyRead()), SLOT(socketFrameRead()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));

	// connect to the server
	connectToHost();
	is_connected = false;
}

bool Client::isConnected()
{
	return is_connected;
}

void Client::socketConnected()
{
	qDebug("Client::socketConnected()");
	if (type == MONITOR)
	{
		qDebug("TRY TO START monitor session");
		socket->write(SOCKET_MONITOR);
		is_connected = true;
		emit connectionUp();
	}
	else if (type == RICHIESTE)
	{
		qDebug("TRY TO START request");
		socket->write(SOCKET_RICHIESTE);
	}
	else if (type == SUPERVISOR)
	{
		qDebug("TRY TO START supervisor");
		socket->write(SOCKET_SUPERVISOR);
	}
	else
	{
		qDebug("TRY TO START command");
		socket->write(SOCKET_COMANDI);
	}
}

void Client::sendFrameOpen(const QString &frame_open)
{
	QByteArray frame = frame_open.toLatin1();
	if (socket->state() == QAbstractSocket::UnconnectedState || socket->state() == QAbstractSocket::ClosingState)
		connectToHost();

	if (!socket->waitForConnected(100))
	{
		// For now, discard silently the frame to send.
		return;
	}

	/* E' davvero necessario? Credo che con la waitForConnected venga poi
	   chiamata la socketConnected, che fa anche questa parte.. verificare!!
	if (type == RICHIESTE)
		socket->write(SOCKET_RICHIESTE);
	else
		socket->write(SOCKET_COMANDI); //lo metto qui else mando prima frame di questo!
	*/

	socket->write(frame);

	if (host != OPENSERVER_ADDR)
		qDebug() << qPrintable(QString("Client::sendFrameOpen()[%1:%2]").arg(host).arg(port)) << "sent:" << frame;
	else
		qDebug() << "Client::sendFrameOpen() sent:" << frame;
}

void Client::connectToHost()
{
	qDebug("Client::connectToHost()");
	socket->connectToHost(host, port);
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

#if DEBUG
void Client::forwardFrame(Client *c)
{
	to_forward = c;
}
#endif

void Client::dispatchFrame(QString frame)
{
#if DEBUG
	if (to_forward)
	{
		to_forward->dispatchFrame(frame);
		return;
	}
#endif
	OpenMsg msg;
	msg.CreateMsgOpen(frame.toAscii().data(), frame.length());
	if (subscribe_list.contains(msg.who()))
	{
		QList<FrameReceiver*> &l = subscribe_list[msg.who()];
		for (int i = 0; i < l.size(); ++i)
			l[i]->manageFrame(msg);
	}
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
	rearmWDT();

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

void Client::socketConnectionClosed()
{
	qDebug("Client::socketConnectionClosed()");
	if (type == MONITOR || type == SUPERVISOR)
	{
		if (is_connected)
		{
			is_connected = false;
			emit connectionDown();
		}
		connectToHost();
	}
}

void Client::socketError(QAbstractSocket::SocketError e)
{
	if (type == MONITOR || type == SUPERVISOR)
	{
		qWarning() << qPrintable(QString("OpenClient [%1:%2]: error").arg(host).arg(port))
				<< socket->errorString() << "occurred on client" << type;

		if (is_connected)
		{
			is_connected = false;
			emit connectionDown();
		}
		QTimer::singleShot(500, this, SLOT(connectToHost()));
	}
	else if (e != QAbstractSocket::RemoteHostClosedError)
		qWarning() << qPrintable(QString("OpenClient [%1:%2]: error").arg(host).arg(port))
			<< socket->errorString() << "occurred on client" << type;
}
