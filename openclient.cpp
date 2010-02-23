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
#include "generic_functions.h" // rearmWDT

#include <QDebug>

#define SOCKET_MONITOR "*99*1##"
#define SOCKET_COMANDI "*99*9##"
#define SOCKET_RICHIESTE "*99*0##"


Client::Client(Type t, const QString &_host, unsigned _port) : type(t), host(_host), port(_port)
{
	qDebug("Client::Client()");

	socket = new QTcpSocket(this);

	connect(socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(socket, SIGNAL(disconnected()), SLOT(socketConnectionClosed()));
	connect(socket, SIGNAL(readyRead()), SLOT(socketFrameRead()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));

	// connect to the server
	connetti();

	// azzero la variabile last_msg_open_read
	last_msg_open_read.CreateNullMsgOpen();

	connect(&Open_read, SIGNAL(timeout()), this, SLOT(clear_last_msg_open_read()));
}

Client::~Client()
{
}

void Client::socketConnected()
{
	qDebug("Client::socketConnected()");
	if (type == MONITOR)
	{
		qDebug("TRY TO START monitor session");
		socket->write(SOCKET_MONITOR);
		emit monitorSu();
	}
	else if (type == RICHIESTE)
	{
		qDebug("TRY TO START request");
		socket->write(SOCKET_RICHIESTE);
	}
	else
	{
		qDebug("TRY TO START command");
		socket->write(SOCKET_COMANDI);
	}
}

void Client::ApriInviaFrameChiudi(const char* frame)
{
	sendFrameOpen(frame);

	// TODO: questa funzione dovra' gestire anche i Nak e ack (e la sua versione "w"
	// dovra' sparire), attendendo che arrivi o un ack o un nack con un ciclo tipo:
	// while (socketWaitForAck() < 0 || socketWaitForNak() < 0);
	// restituendo quindi un booleano che vale true se e' un ack, false altrimenti.
}

void Client::sendFrameOpen(const QString &frame_open)
{
	QByteArray frame = frame_open.toLatin1();
	if (socket->state() == QAbstractSocket::UnconnectedState || socket->state() == QAbstractSocket::ClosingState)
	{
		connetti();
		if (type == RICHIESTE)
			socket->write(SOCKET_RICHIESTE);
		else
			socket->write(SOCKET_COMANDI); //lo metto qui else mando prima frame di questo!
	}
	socket->write(frame);
	qDebug("Client::ApriInviaFrameChiudi() invio: %s",frame.data());
}

void Client::ApriInviaFrameChiudiw(char *frame)
{
	qDebug("Client::ApriInviaFrameChiudiw()");
	ApriInviaFrameChiudi(frame);
	qDebug("Frame sent waiting for ack");
	while (socketWaitForAck() < 0);
	qDebug("Ack received");
}

// richiesta stato all'openserver
void Client::richStato(QString richiesta)
{
	qDebug("Client::richStato()");
	if (socket->state() == QAbstractSocket::UnconnectedState)
		connetti();
	socket->write(richiesta.toAscii());
}

void Client::connetti()
{
	qDebug("Client::connetti()");
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
	if (type == MONITOR)
	{
		qDebug() << "frame read: " << frame;
		if (frame == "*#*1##")
			qWarning("ERROR - ack received");
		else if (frame == "*#*0##")
			qWarning("ERROR - nak received");
		else if (frame != last_msg_open_read.frame_open)
		{
			Open_read.stop();
			last_msg_open_read.CreateMsgOpen(frame.data(),frame.size());
			Open_read.setSingleShot(true);
			Open_read.start(1000);
			emit frameIn(frame.data());
		}
		else
			qDebug("Frame Open duplicated");
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

int Client::socketFrameRead()
{
	qDebug("Client::socketFrameRead()");
	//riarmo il WD
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

void Client::clear_last_msg_open_read()
{
	qDebug("Delete last Frame Open read");
	last_msg_open_read.CreateNullMsgOpen();
}

// Aspetta ack
int Client::socketWaitForAck()
{
	qDebug("Client::socketWaitForAck()");

	if (type == MONITOR)
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
	if (type == MONITOR)
		connetti();
}

void Client::socketError(QAbstractSocket::SocketError e)
{
	if (e != QAbstractSocket::RemoteHostClosedError || type == MONITOR)
		qWarning() << "OpenClient: error " << e << "occurred " << socket->errorString()
			<< "on client" << type;

	if (type == MONITOR)
		QTimer::singleShot(500, this, SLOT(connetti()));
}
