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

#ifndef OPENSERVER_ADDR
#define OPENSERVER_ADDR "127.0.0.1"
#endif

#define OPENSERVER_PORT 20000

// The id for the main openserver
#define MAIN_OPENSERVER 0

class FrameReceiver;


/**
 * This class manages the socket communication throught the application and the openserver.
 *
 * Clients of type MONITOR can be used to receive frames from openserver while clients
 * of types COMMAND or REQUEST can be used to send frames to the openserver.
 */
class Client : public QObject
{
friend class OpenServerMock;
friend class TestScenEvoDevicesCond;
Q_OBJECT
Q_ENUMS(Type) // the enum Type should be the first (or you have to modify the use of QMetaEnum)

public:
	enum Type
	{
		MONITOR = 0,
		SUPERVISOR,
		REQUEST,
		COMMAND
	};

	Client(Type t, const QString &_host=OPENSERVER_ADDR, unsigned _port=0);
	void sendFrameOpen(const QString &frame_open);

	void subscribe(FrameReceiver *obj, int who);
	void unsubscribe(FrameReceiver *obj);

	bool isConnected();

#if DEBUG
	void flush() { socket->flush(); }
	void forwardFrame(Client *c);
#endif

public slots:
	void connectToHost();
	void disconnectFromHost();

signals:
	void connectionUp();
	void connectionDown();

	//! Openwebnet ack received
	void openAckRx();
	//! Openwebnet nak received
	void openNakRx();

private slots:
	// Reads messages from the socket
	int socketFrameRead();

	void socketConnected();
	void socketError(QAbstractSocket::SocketError e);

	void ackReceived();

private:
	QTcpSocket *socket;
	Type type;
	// The address of the openserver
	QString host;
	// The port of the openserver
	unsigned port;

	// The buffer that store the data read from the server
	QByteArray data_read;

	bool ackRx;

	// The list of the FrameReceivers that will receive the incoming frames.
	QHash<int, QList<FrameReceiver*> > subscribe_list;

	// This flag marks if the client is logically connected or not.
	bool is_connected;

#if DEBUG
	Client *to_forward;
#endif

	void manageFrame(QByteArray frame);
	QByteArray readFromServer();

	//! Wait for ack (returns 0 on ack, -1 on nak or when socket is a monitor socket)
	int socketWaitForAck();
	void dispatchFrame(QString frame);
};

#endif
