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


#include "openserver_mock.h"
#include "openclient.h"

#include <QTcpSocket>


OpenServerMock::OpenServerMock() : server(this)
{
	default_timeout = 3000;
	if (!server.listen(QHostAddress(QHostAddress::LocalHost)))
		qFatal("Fatal error: OpenServerMock cannot listen");
}

Client *OpenServerMock::connectMonitor()
{
	Client *client_monitor = new Client(Client::MONITOR, "127.0.0.1", server.serverPort());
	if (!client_monitor->socket->waitForConnected(default_timeout))
		qFatal("Fatal error: client_monitor cannot connect to OpenServerMock");
	server.waitForNewConnection(default_timeout);
	monitor = server.nextPendingConnection();
	return client_monitor;
}

Client *OpenServerMock::connectCommand()
{
	Client *client_command = new Client(Client::COMANDI, "127.0.0.1", server.serverPort());
	if (!client_command->socket->waitForConnected(default_timeout))
		qFatal("Fatal error: client_command cannot connect to OpenServerMock");
	server.waitForNewConnection(default_timeout);
	command = server.nextPendingConnection();
	client_command->flush();
	frameCommand(); // discard all data
	return client_command;
}

Client *OpenServerMock::connectRequest()
{
	Client *client_request = new Client(Client::RICHIESTE, "127.0.0.1", server.serverPort());
	if (!client_request->socket->waitForConnected(default_timeout))
		qFatal("Fatal error: client_request cannot connect to OpenServerMock");
	server.waitForNewConnection(default_timeout);
	request = server.nextPendingConnection();
	client_request->flush();
	frameRequest(); // discard all data
	return client_request;
}

QString OpenServerMock::frameRequest(unsigned int timeout)
{
	if (!timeout)
		timeout = default_timeout;
	request->waitForReadyRead(timeout);
	return request->readAll();
}

QString OpenServerMock::frameCommand(unsigned int timeout)
{
	if (!timeout)
		timeout = default_timeout;
	command->waitForReadyRead(timeout);
	return command->readAll();
}

