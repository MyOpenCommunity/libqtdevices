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
#include "xmlclient.h"

#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>


namespace
{
	const char *START_TAG = "<OWNxml";
	const char *END_TAG = "</OWNxml>";
}


XmlClient::XmlClient(const QString &address, int port, QObject *parent) :
	QObject(parent)
{
	socket = new QTcpSocket(this);
	xml_addr = address;
	xml_port = port;
	connect(socket, SIGNAL(connected()), SIGNAL(connectionUp()));
	connect(socket, SIGNAL(disconnected()), SIGNAL(connectionDown()));
	connect(socket, SIGNAL(readyRead()), SLOT(receiveData()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError()));
}

void XmlClient::connectToHost()
{
	socket->connectToHost(xml_addr, xml_port);
}

void XmlClient::disconnectFromHost()
{
	socket->disconnectFromHost();
}

bool XmlClient::isConnected() const
{
	return socket->state() == QAbstractSocket::ConnectedState;
}

void XmlClient::sendCommand(const QString &command)
{
	socket->write(command.toUtf8());
}

void XmlClient::receiveData()
{
	if (socket->bytesAvailable() <= 0)
		return;

	QByteArray ba = socket->readAll();
	// the xml read is utf8, we need to preserve that.
	buffer.append(QString::fromUtf8(ba.constData(), ba.size()));

	parseData();
}

void XmlClient::socketError()
{
	if (!isConnected())
		emit connectionDown();
}

void XmlClient::parseData()
{
	for (;;)
	{
		// Look for start and end tags.
		int start = buffer.indexOf(START_TAG, 0, Qt::CaseInsensitive);
		if (start == -1)
			break;

		int end = buffer.indexOf(END_TAG, start, Qt::CaseInsensitive);
		if (end == -1)
			break;

		// Get the data between the tags comprending them.
		QString data = buffer.mid(start, end + QByteArray(END_TAG).length() - start);

		if (!data.isEmpty())
		{
			// Remove the useless data.
			buffer = buffer.right(buffer.length() - end);
			emit dataReceived(data.simplified());
		}
	}
}
