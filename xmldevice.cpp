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

#include <QDomDocument>
#include <QStringList>
#include <QDebug>

#include "xmlclient.h"
#include "xml_functions.h"

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
	QPair<int,QVariant> handle_welcome_message(const QDomNode &node)
	{
		return qMakePair<int,QVariant>(XmlDevice::RESP_WELCOME, QVariant());
	}

	QPair<int,QVariant> handle_upnp_server_list(const QDomNode &node)
	{
		QStringList value;
		QDomNodeList servers = node.childNodes();
		for (uint i = 0; i < servers.length(); ++i)
			value.append(getTextChild(servers.item(i), "name"));

		return qMakePair<int,QVariant>(XmlDevice::RESP_SERVERLIST, value);
	}

	QPair<int,QVariant> handle_selection(const QDomNode &node)
	{
		QDomElement element = node.childNodes().at(0).toElement();
		QString tag_name = element.tagName();
		int key = -1;
		QVariant value;

		if (tag_name == "current_server")
		{
			key = XmlDevice::RESP_SERVERSEL;
			value = getTextChild(node, "current_server");
		}
		else if (tag_name == "status_browse")
		{
			key = XmlDevice::RESP_CHDIR;
			value = getTextChild(node, "status_browse") == "browse_ok";
		}

		return qMakePair<int,QVariant>(key, value);
	}

	QPair<int,QVariant> handle_browseup(const QDomNode &node)
	{
		bool value = getTextChild(node, "status_browse") == "browse_ok";

		return qMakePair<int,QVariant>(XmlDevice::RESP_BROWSEUP, value);
	}

	QPair<int,QVariant> handle_listitems(const QDomNode &node)
	{
		XmlDevice::FilesystemEntries entries;
		QDomNodeList nodes = node.childNodes();

		for (int i = 0; i < nodes.size(); ++i)
		{
			QDomNode item = nodes.at(i);
			XmlDevice::FilesystemEntry entry;
			QDomElement element = item.toElement();

			if (element.isNull())
				continue;
			if (element.tagName() == "directory")
				entry.type = XmlDevice::DIRECTORY;
			else if (element.tagName() == "track")
				entry.type = XmlDevice::TRACK;
			entry.name = getTextChild(item, "name");

			entries.append(entry);
		}
		QVariant value;
		value.setValue(entries);

		return qMakePair<int,QVariant>(XmlDevice::RESP_LISTITEMS, value);
	}
}


XmlDevice::XmlDevice()
{
	xml_client = new XmlClient;
	connect(xml_client, SIGNAL(dataReceived(QString)), SLOT(handleData(QString)));

	xml_handlers["WMsg"] = handle_welcome_message;
	xml_handlers["AW26C1"] = handle_upnp_server_list;
	xml_handlers["AW26C2"] = handle_selection;
	xml_handlers["AW26C7"] = handle_browseup;
	xml_handlers["AW26C6"] = handle_listitems;
}

XmlDevice::~XmlDevice()
{
	xml_client->deleteLater();
}

void XmlDevice::requestUPnPServers()
{
	xml_client->sendCommand(buildCommand("RW26C1"));
}

void XmlDevice::select(const QString &name)
{
	xml_client->sendCommand(buildCommand("RW26C2", name));
}

void XmlDevice::browseUp()
{
	xml_client->sendCommand(buildCommand("CW26C7"));
}

void XmlDevice::listItems(int max_results)
{
	xml_client->sendCommand(buildCommand("CW26C6", QString::number(max_results)));
}

void XmlDevice::handleData(const QString &data)
{
	XmlResponse response = parseXml(data);

	if (!response.isEmpty())
		emit responseReceived(response);
}

XmlResponse XmlDevice::parseXml(const QString &xml)
{
	XmlResponse response;
	QDomDocument doc;
	doc.setContent(xml);

	QDomElement root = doc.documentElement();
	if (root.tagName() != "OWNxml")
		return response;

	/*
		Parse the header if the sid or addresses are not set.
		This should append only for the first message (the welcome).
	*/
	if ((sid.isEmpty() || local_addr.isEmpty() || server_addr.isEmpty()) &&
		!parseHeader(getChildWithName(root, "Hdr")))
		return response;

	QDomNode command_container = getChildWithName(root, "Cmd");
	if (command_container.isNull())
		return response;

	QDomNode command = command_container.childNodes().at(0);
	QString command_name = command.toElement().tagName();

	if (xml_handlers.contains(command_name))
	{
		QPair<int, QVariant> result = xml_handlers[command_name](command);
		if (result.first != XmlDevice::RESP_INVALID)
			response[result.first] = result.second;
	}

	return response;
}

bool XmlDevice::parseHeader(const QDomNode &header_node)
{
	QDomNode message_id = getChildWithName(header_node, "MsgID");
	if (message_id.isNull())
		return false;
	sid = getTextChild(message_id, "SID");

	QDomNode dest_address = getChildWithName(header_node, "Dst");
	if (dest_address.isNull())
		return false;
	local_addr = getTextChild(dest_address, "IP");

	QDomNode src_address = getChildWithName(header_node, "Src");
	if (src_address.isNull())
		return false;
	server_addr = getTextChild(src_address, "IP");

	return true;
}

QString XmlDevice::buildCommand(const QString &command, const QString &argument)
{
	QString cmd;

	if (!argument.isEmpty())
		cmd = QString(argument_template).arg(command).arg(argument);
	else
		cmd = QString("\t\t<%1/>").arg(command);

	return QString(command_template).arg(sid)
									.arg(0)
									.arg(server_addr)
									.arg(local_addr)
									.arg(cmd);
}
