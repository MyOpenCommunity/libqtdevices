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
#ifndef XMLDEVICE_H
#define XMLDEVICE_H

#include "generic_functions.h"

#include <QObject>
#include <QHash>
#include <QVariant>

class QDomNode;
class XmlClient;
struct FilesystemEntry;
struct XmlError;

typedef QHash<int, QVariant> XmlResponse;
typedef QHash<int,QVariant> (*xmlHandler_ptr)(const QDomNode&);
typedef QList<FilesystemEntry> FilesystemEntries;

Q_DECLARE_METATYPE(XmlError);
Q_DECLARE_METATYPE(XmlResponse);


namespace XmlResponses
{
	/*!
		\ingroup Multimedia
		\brief Responses of the XmlDevice.

		They are used as index for the XmlResponse in the XmlDevice::responseRecevied()
		signal argument.
	*/
	enum Type
	{
		INVALID = -1,
		ACK,
		WELCOME,
		SERVER_LIST,
		SERVER_SELECTION,
		CHDIR,
		TRACK_SELECTION,
		BROWSE_UP,
		LIST_ITEMS,
	};
}

/*!
	\ingroup Multimedia
	\brief Encapsulates errors of the XmlDevice.

	It has two fields:
	- XmlError::response that refers to the XmlResponses::Type that generates
		the error.
	- XmlError::code that refers to the XmlError::Code which describe the type
		of the error.
*/
struct XmlError
{
	/*!
		\brief Error codes
	*/
	enum Code
	{
		PARSE = 0, /*!< Error during the parsing of the response's XML */
		BROWSING,  /*!< Error during the browsing of the UPnP resource */
		CLIENT,    /*!< Error in XmlClient */
	};

	/*!
		\brief Creates a new XmlError.
	*/
	XmlError() {}

	/*!
		\brief Creates a new XmlError with the given response type \a r and
		error code \a c.
	*/
	XmlError(XmlResponses::Type r, XmlError::Code c) : response(r), code(c) {}

	/*!
		\brief The response that generates the error.
	*/
	XmlResponses::Type response;

	/*!
		\brief The code of the error.
	*/
	XmlError::Code code;
};


/*!
	\ingroup Multimedia
	\brief Abstraction of a single entry of an UPnP resource browsing result.

	It has two fields:
	- FilesystemEntry::name that refers to the name of the entry, which could be
		the name of a directory or the tags of a track.
	- FilesystemEntry::type that refers to the FilesystemEntry::Type of the entry.
*/
struct FilesystemEntry
{
	/*!
		\brief Creates a new FilesystemEntry with the given name \a and the type
		\a t.
	*/
	FilesystemEntry(const QString &n, MultimediaFileType t, const QString &entry_url = QString())
		: name(n), type(t), url(entry_url) {}

	/*!
		\brief The name of the entry.
	*/
	QString name;

	/*!
		\brief The type of the entry.
	*/
	MultimediaFileType type;

	/*!
		\brief The url of the entry
		\note It's used only for FilesystenEntry::TRACK entries.
	*/
	QString url;
};

/*!
	\brief Returns true if \a a and \b are equal, false otherwise.
*/
inline bool operator ==(const FilesystemEntry &a, const FilesystemEntry &b)
{
	return a.name == b.name && a.type == b.type;
}

Q_DECLARE_METATYPE(FilesystemEntries);


/*!
	\ingroup Multimedia
	\brief Incapsulates the protocol to comunicate with the OpenXml server.

	With this class you can send request to the OpenXml server and get responses
	from it.
	It works in a similar way of the \c device class.

	You can get the list of the UPnP servers with the requestUPnPServers() method
	and select one of these with the selectServer() method.

	To request the list of items of a server you can use the listItems() method.

	To enter into a directory you can use the chDir() method, and to come back
	the browseUp() method.

	To get infos about a file you can use the selectFile() method.

	Each of the methods listed above, generate a request. The response is returned
	asynchronously through the responseReceived() signal. The parameter of this
	signal is a XmlResponse, which can contain many values indexed by a
	XmlResponses::Type value.

	A request can generate an error, too, in such case the error() signal is
	emitted with the response that generated the error and the error code associated.

	\section xmldevice-responses Responses
	<TABLE>
		<TR><TH>Response</TH><TH>Type</TH><TH>Description</TH></TR>
		<TR><TD>XmlResponses::WELCOME</TD><TD>QVariant</TD><TD>Empty dummy value</TD></TR>
		<TR><TD>XmlResponses::SERVER_LIST</TD><TD>FilesystemEntries</TD><TD>List of FilesystemEntry each of which of type directory.</TD></TR>
		<TR><TD>XmlResponses::SERVER_SELECTION</TD><TD>QString</TD><TD>The name of the selected server.</TD></TR>
		<TR><TD>XmlResponses::CHDIR</TD><TD>bool</TD><TD>True if no error.</TD></TR>
		<TR><TD>XmlResponses::TRACK_SELECTION</TD><TD>QString</TD><TD>The url of the selected file.</TD></TR>
		<TR><TD>XmlResponses::BROWSE_UP</TD><TD>bool</TD><TD>True if no error.</TD></TR>
		<TR><TD>XmlResponses::LIST_ITEMS</TD><TD>FilesystemEntries</TD><TD>The list of the items into the current directory.</TD></TR>
	</TABLE>

	\sa XmlResponses::Type, XmlError::Code
*/
class XmlDevice : public QObject
{
friend class TestXmlDevice;
friend class XmlDeviceTester;

Q_OBJECT
public:
	/*!
		\brief Constructor.
	*/
	XmlDevice();

	/*!
		\brief Destructor.
	*/
	~XmlDevice();

	void reset();

	/*!
		\brief Requests the list of the UPnP servers.
	*/
	void requestUPnPServers();

	/*!
		\brief Requests to enter into the server \a server_name.
	*/
	void selectServer(const QString &server_name);

	/*!
		\brief Requests to enter into the directory \a dir.
	*/
	void chDir(const QString &dir);

	/*!
		\brief Requests the selection of the file identified by \a file_tags.
	*/
	void selectFile(const QString &file_tags);

	/*!
		\brief Requests to exit from the current directory.
	*/
	void browseUp();

	/*!
		\brief Requests the list of the items of the current directory.
	*/
	void listItems();

signals:
	/*!
		\brief Emitted when a new response is received.
	*/
	void responseReceived(const XmlResponse &response);

	/*!
		\brief Emitted when a request generates an error.

		\sa XmlResponses::Type, XmlError::Code
	*/
	void error(int response, int code);

private slots:
	void handleData(const QString &data);
	void handleClientError();
	void sendMessageQueue();
	void cleanSessionInfo();

private:
	void sendCommand(const QString &command, const QString &argument = QString());
	void select(const QString &name);
	XmlResponse parseXml(const QString &xml);
	bool parseHeader(const QDomNode &header_node);
	bool parseAck(const QDomNode &ack);
	QString buildCommand(const QString &command, const QString &argument = QString());

	XmlClient *xml_client;
	QHash<QString, xmlHandler_ptr> xml_handlers;

	QList<QPair<QString, QString> > message_queue;

	bool welcome_received;
	QString sid;
	int pid;
	QString local_addr;
	QString server_addr;
};

#endif // XMLDEVICE_H
