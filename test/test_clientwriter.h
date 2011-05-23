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


#ifndef TEST_CLIENTWRITER_H
#define TEST_CLIENTWRITER_H

#include <frame_classes.h>
#include <openclient.h>

#include <QObject>
#include <QList>
#include <QPair>
#include <QString>

class QTcpServer;
class QTcpSocket;
class ClientWriter;


class FrameSenderMock : public FrameSender
{
public:
	using FrameSender::subscribeAck;
	FrameSenderMock(ClientWriter *command, ClientWriter *request);

	virtual void manageAck(OpenMsg &msg);
	virtual void manageNak(OpenMsg &msg);

	QList<QPair<bool, QString> > ack_history;
};


class TestClientWriter : public QObject
{
Q_OBJECT
public:
	TestClientWriter();

private slots:
	void testSingleFrame();
	void testMultipleFrames();
	void testDuplicates();
	void testDelay();
	void testAck();
	void testAckNotReceived();
	void testNak();
	void testAckMultipleWho();
	void testAckMultipleReceivers();
	void testMultipleAckNak();

private:
	QTcpServer *server;

	QTcpSocket *newConnection(ClientWriter *client);
	QString readAll(QTcpSocket *socket);
	ClientWriter *createClient(Client::Type t);
	void waitForData(ClientWriter *client);
};

#endif
