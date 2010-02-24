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


#ifndef OPENSERVER_MOCK_H
#define OPENSERVER_MOCK_H

#include <QObject>
#include <QPair> // prevent a bug in Qt 4.5.0
#include <QTcpServer>

class QTcpSocket;
class Client;


class OpenServerMock : public QObject
{
Q_OBJECT
public:
	OpenServerMock();

	Client *connectMonitor();
	Client *connectCommand();
	Client *connectRequest();
	QString frameRequest(unsigned int timeout = 0);
	QString frameCommand(unsigned int timeout = 0);

private:
	QTcpServer server;
	int port;
	int default_timeout;

	QTcpSocket *command, *monitor, *request;
};

#endif // OPENSERVER_MOCK_H
