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

const char *start_tag = "<OWNxml";
const char *end_tag = "</OWNxml>";


XmlClient::XmlClient(QObject *parent) :
	QObject(parent), socket(new QTcpSocket(this))
{
	connect(socket, SIGNAL(readyRead()), SLOT(parseData()));
}

void XmlClient::connectToHost(const QString &address, int port)
{
	socket->connectToHost(address, port);
}

void XmlClient::disconnectFromHost()
{
	socket->disconnectFromHost();
}

void XmlClient::sendCommand(const QString &command)
{
	socket->write(command.toUtf8());
}

void XmlClient::parseData()
{
	if (socket->bytesAvailable() <= 0)
		return;

	buffer.append(socket->readAll());

	// Look for start and end tags.
	int start = buffer.indexOf(start_tag, 0, Qt::CaseInsensitive);
	int end = buffer.indexOf(end_tag, start, Qt::CaseInsensitive);

	// Get the data between the tags comprending them.
	QString data = buffer.mid(start, end + ::strlen(end_tag) - start);

	if (!data.isEmpty()) {
		// Remove the useless data.
		buffer = buffer.right(buffer.length() - end);
		emit dataReceived(data.simplified());
	}
}
