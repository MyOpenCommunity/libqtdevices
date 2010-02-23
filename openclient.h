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


#ifndef  OPENCLIENT_H
#define OPENCLIENT_H

#include <openwebnet.h>

#include <QTimer>
#include <QTcpSocket>
#include <QByteArray>
#include <QRegExp>
#include <QHash>

#ifndef OPENSERVER_ADDR
#define OPENSERVER_ADDR "127.0.0.1"
#endif

#define OPENSERVER_PORT 20000

/*!
  \class Client
  \brief This class manages the socket communication throught the application and the \a openserver.

  \date lug 2005
*/

class Client : public QObject
{
friend class OpenServerMock;
Q_OBJECT
public:

	enum Type
	{
		MONITOR = 0,
		RICHIESTE,
		COMANDI
	};

	Client(Type t, const QString &_host=OPENSERVER_ADDR, unsigned _port=OPENSERVER_PORT);
	void ApriInviaFrameChiudi(const char *);
	void installFrameCompressor(int timeout, const QString &regex);
	void flush() { socket->flush(); }
	~Client();

private slots:
	void connetti();
	/// Reads messages from the socket
	int socketFrameRead();

	void socketConnected();
	void socketConnectionClosed();
	void socketError(QAbstractSocket::SocketError e);

	/// Send an \a Open \aFrame through the socket and wait for ack
	void ApriInviaFrameChiudiw(char*);
	void richStato(QString richiesta);
	void ackReceived();
	void sendFrameOpen(const QString &frame_open);

	void clear_last_msg_open_read();

private:
	QTcpSocket *socket;
	Type type;
	QString host;
	unsigned port;
	QByteArray data_read;
	QTimer Open_read;
	openwebnet last_msg_open_read;
	openwebnet last_msg_open_write;
	bool ackRx;

	void socketStateRead(char*);
	void manageFrame(QByteArray frame);
	QByteArray readFromServer();

	//! Wait for ack (returns 0 on ack, -1 on nak or when socket is a monitor socket)
	int socketWaitForAck();

signals:
	void frameIn(char*);
	void rispStato(char*);
	void monitorSu();
	void frameToAutoread(char*);
	//! Openwebnet ack received
	void openAckRx();
	//! Openwebnet nak received
	void openNakRx();
};

#endif
