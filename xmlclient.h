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


/*!
	\ingroup Multimedia
	\brief Manages the comunication between the application and the OpenXml server.

	This class wraps a socket connected to an OpenXml server. The connection is
	performed calling the connectToHost() method and the disconnection calling
	the disconnectFromHost() method. To know the state of the client you can
	use the isConnected() method. When the client ends the connection procedure
	emits the connectionUp() signal, and if there is an error that makes the
	client to disconnect, the connectionDown() signal is emitted.

	To send a command to the server you can use the sendCommand() method.

	Every time a chunk of xml comes to the client, it is appended to a buffer
	and parsed. If a valid response is found, the signal dataReceived() signal
	is emitted with the response as argument.
*/
class XmlClient : public QObject
{
friend class TestXmlClient;

Q_OBJECT
public:
	/*!
		\brief Constructor

		Creates a new XmlClient with the given \a address, \a port and \a parent.
	*/
	explicit XmlClient(const QString &address = XML_SERVER_ADDRESS,
					   int port = XML_SERVER_PORT, QObject *parent = 0);

	/*!
		\brief Connects to the OpenXml server.
	*/
	void connectToHost();

	/*!
		\brief Disconnects from the OpenXml server.
	*/
	void disconnectFromHost();

	/*!
		\brief Returns true if the client is connected to the OpenXml server,
		false otherwise.
	*/
	bool isConnected() const;

	/*!
		\brief Sends \a command to the OpenXml server.
	*/
	void sendCommand(const QString &command);

signals:
	/*!
		\brief Emitted when the client is connected to the OpenXml server and
		ready to comunicate.
	*/
	void connectionUp();

	/*!
		\brief Emitted when the client is disconnected from the OpenXml server.

		\note It is emitted both when the client is explicitelly disconnected
		thought the disconnectFromHost() method and if a fatal error occours.
	*/
	void connectionDown();

	/*!
		\brief Emitted when a single chunk of xml (representing a response) is
		extracted from the internal buffer.
	*/
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
