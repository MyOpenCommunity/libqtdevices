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
#include "xmldevice.h"
#include "xmlclient.h"
#include "xml_functions.h"

#include <QDomDocument>
#include <QStringList>
#include <QUuid>
#include <QDebug>

const char *command_template =
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<OWNxml xmlns=\"http://www.bticino.it/xopen/v1 xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
		"	<Hdr>\n"
		"		<MsgID>\n"
		"			<SID>%1</SID>\n"
		"			<PID>%2</PID>\n"
		"		</MsgID>\n"
		"		<Dst>\n"
		"			<IP>%3</IP>\n"
		"		</Dst>\n"
		"		<Src>\n"
		"			<IP>%4</IP>\n"
		"		</Src>\n"
		"	</Hdr>\n"
		"	<Cmd>\n"
		"%5\n"
		"	</Cmd>\n"
		"</OWNxml>\n";

const char *argument_template =
		"		<%1>\n"
		"			<id>%2</id>\n"
		"		</%1>";

namespace
{
	QHash<int,QVariant> handle_welcome_message(const QDomNode &node)
	{
		QHash<int,QVariant> result;
		result[XmlResponses::WELCOME] =  QVariant();

		return result;
	}

	QHash<int,QVariant> handle_upnp_server_list(const QDomNode &node)
	{
		QHash<int,QVariant> result;
		QStringList value;

		QDomElement server_element = getElement(node, "server");
		if (server_element.isNull())
		{
			result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::SERVER_LIST, XmlError::PARSE));
			return result;
		}

		foreach (const QDomNode &server, getChildren(server_element, "name"))
			value.append(server.toElement().text());

		result[XmlResponses::SERVER_LIST] = value;

		return result;
	}

	QHash<int,QVariant> handle_selection(const QDomNode &node)
	{
		QHash<int,QVariant> result;

		QString tag_name = node.childNodes().at(0).toElement().tagName();

		if (tag_name == "current_server")
		{
			QString current_server = getTextChild(node, "current_server");
			if (current_server.isEmpty())
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::SERVER_SELECTION, XmlError::PARSE));
			else
				result[XmlResponses::SERVER_SELECTION] = current_server;
		}
		else if (tag_name == "status_browse")
		{
			QString status_browse = getTextChild(node, "status_browse");
			if (status_browse.isEmpty())
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::CHDIR, XmlError::PARSE));
			else if (status_browse == "browse_ok")
				result[XmlResponses::CHDIR] = true;
			else
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::CHDIR, XmlError::BROWSING));
		}
		else if (tag_name == "DIDL-Lite")
		{
			QString track_url = getElement(node, "DIDL-Lite/item/res").text();
			if (track_url.isEmpty())
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::TRACK_SELECTION, XmlError::PARSE));
			else
				result[XmlResponses::TRACK_SELECTION] = track_url;
		}

		return result;
	}

	QHash<int,QVariant> handle_browseup(const QDomNode &node)
	{
		QHash<int,QVariant> result;

		QString status_browse = getTextChild(node, "status_browse");
		if (status_browse.isEmpty())
			result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::BROWSE_UP, XmlError::PARSE));
		else if (status_browse == "browse_ok")
			result[XmlResponses::BROWSE_UP] = true;
		else
			result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::BROWSE_UP, XmlError::BROWSING));
		return result;
	}

	QHash<int,QVariant> handle_listitems(const QDomNode &node)
	{
		QHash<int,QVariant> result;
		FilesystemEntries entries;

		QDomNode directories = getChildWithName(node, "directories");
		foreach (const QDomNode &item, getChildren(directories, "name"))
			entries << FilesystemEntry(item.toElement().text(), "directory", QString());

		QDomNode tracks = getChildWithName(node, "tracks");
		foreach (const QDomNode &item, getChildren(tracks, "file"))
		{
			entries << FilesystemEntry(getElement(item, "DIDL-Lite/item/dc:title").text(),
									   getElement(item,"DIDL-Lite/item/upnp:class").text(),
									   getElement(item, "DIDL-Lite/item/res").text());
		}

		QVariant value;
		value.setValue(entries);
		result[XmlResponses::LIST_ITEMS] = value;

		return result;
	}
}


XmlDevice::XmlDevice()
{
	xml_client = new XmlClient;
	connect(xml_client, SIGNAL(dataReceived(QString)), SLOT(handleData(QString)));
	connect(xml_client, SIGNAL(connectionUp()), SLOT(sendMessageQueue()));
	connect(xml_client, SIGNAL(connectionDown()), SLOT(cleanSessionInfo()));

	welcome_received = false;

	xml_handlers["WMsg"] = handle_welcome_message;
	xml_handlers["AW26C1"] = handle_upnp_server_list;
	xml_handlers["AW26C2"] = handle_selection;
	xml_handlers["AW26C7"] = handle_browseup;
	xml_handlers["AW26C15"] = handle_listitems;
}

XmlDevice::~XmlDevice()
{
	xml_client->deleteLater();
}

void XmlDevice::requestUPnPServers()
{
	sendCommand("RW26C1");
}

void XmlDevice::selectServer(const QString &server_name)
{
	select(server_name);
}

void XmlDevice::chDir(const QString &dir)
{
	select(dir);
}

void XmlDevice::selectFile(const QString &file_tags)
{
	select(file_tags);
}

void XmlDevice::browseUp()
{
	sendCommand("CW26C7");
}

void XmlDevice::listItems(int max_results)
{
	sendCommand("RW26C15", QString::number(max_results));
}

void XmlDevice::handleData(const QString &data)
{
	XmlResponse response = parseXml(data);

	if (response.contains(XmlResponses::INVALID))
	{
		XmlError e = response[XmlResponses::INVALID].value<XmlError>();
		emit error(e.response, e.code);
	}
	else
		emit responseReceived(response);
}

void XmlDevice::sendMessageQueue()
{
	if (!welcome_received)
		return;

	while (!message_queue.isEmpty())
	{
		QPair<QString,QString> command = message_queue.takeFirst();
		sendCommand(command.first, command.second);
	}
}

void XmlDevice::cleanSessionInfo()
{
	welcome_received = false;
	sid.clear();
	pid = 0;
	local_addr.clear();
	server_addr.clear();
}

void XmlDevice::sendCommand(const QString &message, const QString &argument)
{
	if (!xml_client->isConnected())
	{
		// Saves the message and the argument.
		// An alternative implementation could be to generate only the command
		// body, and save it. Then when the connection comes up prepend the
		// generated header. For that implementation the problem is the PID
		// which changes with the responses.
		message_queue << qMakePair<QString,QString>(message, argument);
		xml_client->connectToHost();
	}
	else {
		++pid;
		xml_client->sendCommand(buildCommand(message, argument));
	}
}

void XmlDevice::select(const QString &name)
{
	sendCommand("RW26C2", name);
}

XmlResponse XmlDevice::parseXml(const QString &xml)
{
	XmlResponse response;
	QDomDocument doc;
	doc.setContent(xml);

	QDomElement root = doc.documentElement();

	if (root.tagName() == "OWNxml" && parseHeader(getChildWithName(root, "Hdr")))
	{
		QDomNode command_container = getChildWithName(root, "Cmd");
		if (!command_container.isNull() && command_container.childNodes().size() == 1)
		{
			QDomNode command = command_container.childNodes().at(0);
			QString command_name = command.toElement().tagName();

			if (!command_name.isEmpty())
			{
				if (command_name == "ACK" && parseAck(command))
				{
					response[XmlResponses::ACK] = true;
					return response;
				}
				else if (xml_handlers.contains(command_name))
				{
					response = xml_handlers[command_name](command);
					return response;
				}
			}
		}
	}

	response[XmlResponses::INVALID].setValue(XmlError(XmlResponses::INVALID, XmlError::PARSE));
	return response;
}

bool XmlDevice::parseHeader(const QDomNode &header_node)
{
	QDomNode message_id = getChildWithName(header_node, "MsgID");
	if (message_id.isNull())
		return false;
	sid = getTextChild(message_id, "SID");
	pid = getTextChild(message_id, "PID").toInt();

	QDomNode dest_address = getChildWithName(header_node, "Dst");
	if (dest_address.isNull())
		return false;
	local_addr = getTextChild(dest_address, "IP");

	QDomNode src_address = getChildWithName(header_node, "Src");
	if (src_address.isNull())
		return false;
	server_addr = getTextChild(src_address, "IP");

	if (!welcome_received)
	{
		welcome_received = true;
		sendMessageQueue();
	}

	return true;
}

/*
	When we receive an ACK with RC == 200 we must regenerate the sid and
	set the pid to 0.
*/
bool XmlDevice::parseAck(const QDomNode &ack)
{
	QString rc = getTextChild(ack, "RC");
	if (rc.isEmpty())
		return false;

	if (rc == "200")
	{
		pid = 0;
		sid = QUuid::createUuid().toString();
	}

	return true;
}

QString XmlDevice::buildCommand(const QString &command, const QString &argument)
{
	QString cmd;

	if (!argument.isEmpty())
		cmd = QString(argument_template).arg(command).arg(argument);
	else
		cmd = QString("\t\t<%1/>").arg(command);

	return QString(command_template).arg(sid).arg(pid).arg(server_addr).arg(local_addr).arg(cmd);
}
