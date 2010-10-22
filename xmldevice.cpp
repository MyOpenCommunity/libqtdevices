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

const char *xml_template =
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
		"<OWNxml xmlns=\"http://www.bticino.it/xopen/v1 xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
		"	<Hdr>"
		"		<MsgID>"
		"			<SID>%classid</SID>"
		"			<PID>%seq</PID>"
		"		</MsgID>"
		"		<Dst>"
		"			<IP>%ipaddr_dst</IP>"
		"		</Dst>"
		"		<Src>"
		"			<IP>%ipaddr_src</IP>"
		"		</Src>"
		"	</Hdr>"
		"	<Cmd>"
		"		%basic_command"
		"	</Cmd>"
		"</OWNxml>";

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
}


XmlDevice::XmlDevice() :
	QObject(), xml_client(new XmlClient)
{
	connect(xml_client, SIGNAL(dataReceived(QString)), SLOT(handleData(QString)));

	xml_handlers.insert("WMsg", handle_welcome_message);
	xml_handlers.insert("AW26C1", handle_upnp_server_list);
}

XmlDevice::~XmlDevice()
{
	xml_client->deleteLater();
}

void XmlDevice::requestUPnPServers()
{

}

void XmlDevice::select(const QString &name)
{

}

void XmlDevice::browseUp()
{

}

void XmlDevice::listItems(int max_results)
{

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

	QDomNode command_container = getChildWithName(root, "Cmd");
	if (command_container.isNull())
		return response;

	QDomNode command = command_container.childNodes().at(0);
	QString command_name = command.toElement().tagName();

	if (xml_handlers.contains(command_name))
	{
		QPair<int, QVariant> result = xml_handlers[command_name](command);
		response.insert(result.first, result.second);
	}

	return response;
}
