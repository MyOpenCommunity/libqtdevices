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

#include "test_clientwriter.h"
#include "test_functions.h"

#include <openmsg.h>

#include <QDebug>
#include <QApplication>
#include <QTest>
#include <QTcpServer>


FrameSenderMock::FrameSenderMock(ClientWriter *command, ClientWriter *request)
{
	Clients c;
	c.command = command;
	c.request = request;
	QHash<int, Clients> clients;
	clients[0] = c;
	FrameSender::setClients(clients);
}

void FrameSenderMock::manageAck(OpenMsg &msg)
{
	ack_history.append(qMakePair(true, QString(msg.frame_open)));
}

void FrameSenderMock::manageNak(OpenMsg &msg)
{
	ack_history.append(qMakePair(false, QString(msg.frame_open)));
}


TestClientWriter::TestClientWriter()
{
	server = new QTcpServer(this);
	if (!server->listen(QHostAddress(QHostAddress::LocalHost)))
		qFatal("Fatal error: TestClientWriter server cannot listen");
}

ClientWriter *TestClientWriter::createClient(Client::Type t)
{
	return new ClientWriter(t, "127.0.0.1", server->serverPort());
}

QString TestClientWriter::readAll(QTcpSocket *socket)
{
	socket->waitForReadyRead(0);
	return socket->readAll();
}

QTcpSocket *TestClientWriter::newConnection(ClientWriter *client)
{
	server->waitForNewConnection(3000);
	QTcpSocket *socket = server->nextPendingConnection();
	if (!client->socket->waitForConnected(3000))
		qFatal("Fatal error: cannot connect to TestClientWriter server");

	// Discard the id of the channel
	client->flush();
	readAll(socket);
	// And the ack for the connection and for the id
	client->ack_source_list.clear();
	return socket;
}

void TestClientWriter::testSingleFrame()
{
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);

	QString frame("*#13**10##");
	client->sendFrameOpen(frame);
	testSleep(ClientWriter::STANDARD_FRAME_DELAY);
	client->socket->flush();

	QString data = readAll(command);
	QCOMPARE(data, frame);
}

void TestClientWriter::testMultipleFrames()
{
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);

	QStringList frames;
	frames << "*#4*11*0##" << "*#13**20##" << "*8*37#2*176##" << "*#5*0##" << "*#22*3#3#1*12##";

	foreach (QString frame, frames)
		client->sendFrameOpen(frame);

	testSleep(ClientWriter::STANDARD_FRAME_DELAY);
	client->socket->flush();

	QString data = readAll(command);
	QCOMPARE(data, frames.join(""));
}

void TestClientWriter::testDuplicates()
{
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);

	QStringList frames;
	frames << "*#1*82##" << "*#5*#1##" << "*#1*12##" << "*#2*23##" << "*#25*1##";

	foreach (QString frame, frames + QStringList("*#5*#1##"))
		client->sendFrameOpen(frame);

	testSleep(ClientWriter::STANDARD_FRAME_DELAY);
	client->socket->flush();

	QString data = readAll(command);
	QCOMPARE(data, frames.join(""));
}

void TestClientWriter::testDelay()
{
	int delay = 500;
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);
	ClientWriter::setDelay(delay);
	ClientWriter::delayFrames(true);

	QStringList frames;
	frames << "*#4*497*0##" << "*#1*12##" << "*#22*3#1#1*12##" << "*#18*12*250##";

	foreach (QString frame, frames)
		client->sendFrameOpen(frame);

	testSleep(delay + ClientWriter::STANDARD_FRAME_DELAY);
	client->socket->flush();

	QString data = readAll(command);
	QCOMPARE(data, frames.join(""));
}

void TestClientWriter::testAck()
{
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);

	QString frame("*#4*#0##");
	FrameSenderMock frame_sender(client, 0);
	QCOMPARE(frame_sender.ack_history.size(), 0);

	frame_sender.subscribeAck(4);
	client->sendFrameOpen(frame);
	client->flush();

	command->write(ACK_FRAME);
	command->flush();
	client->socket->waitForReadyRead(0);

	QCOMPARE(frame_sender.ack_history.first(), qMakePair(true, frame));
}

void TestClientWriter::testAckNotReceived()
{
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);

	QString frame("*#4*#0##");
	FrameSenderMock frame_sender(client, 0);
	frame_sender.subscribeAck(5);
	client->sendFrameOpen(frame);
	client->flush();

	command->write(ACK_FRAME);
	command->flush();
	client->socket->waitForReadyRead(0);

	QCOMPARE(frame_sender.ack_history.size(), 0);
}

void TestClientWriter::testNak()
{
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);

	QString frame("*#13**16##");
	FrameSenderMock frame_sender(client, 0);

	frame_sender.subscribeAck(13);
	client->sendFrameOpen(frame);
	client->flush();

	command->write(NAK_FRAME);
	command->flush();
	client->socket->waitForReadyRead(0);

	QCOMPARE(frame_sender.ack_history.first(), qMakePair(false, frame));
}

void TestClientWriter::testAckMultipleWho()
{
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);

	FrameSenderMock frame_sender(client, 0);

	frame_sender.subscribeAck(5);
	frame_sender.subscribeAck(13);

	QStringList frames;
	frames << "*#5*#1##" << "*#18*11*250##" << "*#1*11##" << "*#5*#8##" << "*#5*0##";
	frames << "*#13**#9*1##" << "*#22*2#5*13##" << "*#0*01##" << "*5*50#8#00000000*0##";

	foreach (QString frame, frames)
		client->sendFrameOpen(frame);

	client->flush();

	for (int i = 0; i < frames.size(); ++i)
		command->write(i < 3 ? ACK_FRAME : NAK_FRAME);

	command->flush();
	client->socket->waitForReadyRead(0);

	QList<QPair<bool, QString> > expected;
	expected.append(qMakePair(true, QString("*#5*#1##")));
	expected.append(qMakePair(false, QString("*#5*#8##")));
	expected.append(qMakePair(false, QString("*#5*0##")));
	expected.append(qMakePair(false, QString("*#13**#9*1##")));
	expected.append(qMakePair(false, QString("*5*50#8#00000000*0##")));
	QCOMPARE(frame_sender.ack_history, expected);
}

void TestClientWriter::testAckMultipleReceivers()
{
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);

	FrameSenderMock frame_sender1(client, 0);
	FrameSenderMock frame_sender2(client, 0);
	FrameSenderMock frame_sender3(client, 0);

	frame_sender1.subscribeAck(4);
	frame_sender2.subscribeAck(13);
	frame_sender3.subscribeAck(4);

	QStringList frames;
	frames << "*#13**23##" << "*#22*5#3#0#0*1##" << "*#22*5#3#0#0*12##" << "*#4*#01##";
	frames << "*#5*#1##" << "*#4*113*0##" <<"*#18*51*#1200#1*255##" << "*#13**#9*1##";

	foreach (QString frame, frames)
		client->sendFrameOpen(frame);

	client->flush();

	for (int i = 0; i < frames.size(); ++i)
		command->write((i % 2) == 0 ? ACK_FRAME : NAK_FRAME);

	command->flush();
	client->socket->waitForReadyRead(0);

	QList<QPair<bool, QString> > expected4;
	expected4.append(qMakePair(false, QString("*#4*#01##")));
	expected4.append(qMakePair(false, QString("*#4*113*0##")));
	QCOMPARE(frame_sender1.ack_history, expected4);
	QCOMPARE(frame_sender3.ack_history, expected4);

	QList<QPair<bool, QString> > expected13;
	expected13.append(qMakePair(true, QString("*#13**23##")));
	expected13.append(qMakePair(false, QString("*#13**#9*1##")));
	QCOMPARE(frame_sender2.ack_history, expected13);
}

void TestClientWriter::testMultipleAckNak()
{
	ClientWriter *client = createClient(Client::COMMAND);
	QTcpSocket *command = newConnection(client);

	FrameSenderMock frame_sender(client, 0);

	frame_sender.subscribeAck(1);

	QStringList frames;
	frames << "*#18*12*250##" << "*#1*82##" << "*#18*51*#1200#1*255##" << "*#1*12##";
	frames << "*#5*#1##" << "*#2*16##" << "*#1*11##" << "*#0*01##" << "*8*37#1*11##";

	foreach (QString frame, frames)
		client->sendFrameOpen(frame);

	client->flush();

	for (int i = 0; i < frames.size(); ++i)
		command->write(i < 4 ? ACK_FRAME : NAK_FRAME);

	command->flush();
	client->socket->waitForReadyRead(0);

	QList<QPair<bool, QString> > expected;
	expected.append(qMakePair(true, QString("*#1*82##")));
	expected.append(qMakePair(true, QString("*#1*12##")));
	expected.append(qMakePair(false, QString("*#1*11##")));
	QCOMPARE(frame_sender.ack_history, expected);
}

