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
#ifndef XMLCLIENT_H
#define XMLCLIENT_H

#include <QObject>


#ifndef XML_SERVER_ADDRESS
#define XML_SERVER_ADDRESS "127.0.0.1"
#endif

#define XML_SERVER_PORT 12345

class QTcpSocket;



class XmlClient : public QObject
{
friend class TestXmlClient;

Q_OBJECT
public:
	explicit XmlClient(const QString &address = XML_SERVER_ADDRESS,
					   int port = XML_SERVER_PORT, QObject *parent = 0);

	void connectToHost();
	void disconnectFromHost();

	bool isConnected() const;

	void sendCommand(const QString &command);

signals:
	void connectionUp();
	void connectionDown();
	void dataReceived(const QString &data);

private slots:
	void receiveData();
	void socketError();

private:
	void parseData();

	QTcpSocket *socket;
	QString xml_addr;
	int xml_port;
	QString buffer;
};

#endif // XMLCLIENT_H
