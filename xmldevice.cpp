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

#include <QTextDocument> // Qt::escape
#include <QDomDocument>
#include <QStringList>
#include <QUuid>
#include <QDebug>
#include <QDir>
#include <QTime>

// Set 1 to this define to dump the content of the responses from openxmlserver.
#define DUMP_OPENXML 0
#define DUMP_DIR "cfg/extra/0"


const char *command_template =
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<OWNxml xmlns=\"http://www.bticino.it/xopen/v1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n"
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


namespace
{
	EntryInfo::Metadata getMetadata(const QDomNode &item)
	{
		EntryInfo::Metadata metadata;

		foreach (const QString &tag, QStringList() << "title" << "artist" << "album")
		{
			QString value = getElement(item, QString("DIDL-Lite/item/%1:%2").arg(tag == "title" ? "dc" : "upnp").arg(tag)).text();
			if (!value.isEmpty())
				metadata[tag] = value;
		}

		QString duration = getElement(item, "DIDL-Lite/item/res").attribute("duration");
		if (!duration.isEmpty())
			metadata["total_time"] = duration;

		return metadata;
	}

	EntryInfo::Type getFileType(const QDomNode &item)
	{
		EntryInfo::Type file_type = EntryInfo::UNKNOWN;
		QString upnp_class = getElement(item, "DIDL-Lite/item/upnp:class").text();

		if (upnp_class.contains("audioItem"))
			file_type = EntryInfo::AUDIO;
		else if (upnp_class.contains("videoItem"))
			file_type = EntryInfo::VIDEO;
		else if (upnp_class.contains("imageItem"))
			file_type = EntryInfo::IMAGE;
		else
			file_type = EntryInfo::UNKNOWN;

		return file_type;
	}

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
			qWarning() << "handle_upnp_server_list: server node not found";
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
			{
				qWarning() << "handle_selection: current_server not found";
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::SERVER_SELECTION, XmlError::PARSE));
			}
			else if (current_server == "no_answer_has_been_received" || current_server == "server_down")
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::SERVER_SELECTION, XmlError::SERVER_DOWN));
			else
				result[XmlResponses::SERVER_SELECTION] = current_server;
		}
		// NOTE: the command RW26C2 (and the answer AW26C2) is the same to select a directory,
		// a server or a file to play. But BTouch has to do different things, so we represent it with:
		// - a selection of a directory with the value CHDIR;
		// - a selection of a file with TRACK_SELECTION;
		// - a selection of a server with SERVER_SELECTION.
		// Unfortunately the answers for the first two cases can be the same if the server is offline,
		// so can happen that we request to select a file and the xmldevice responses with a CHDIR answer.
		else if (tag_name == "status_browse")
		{
			QString status_browse = getTextChild(node, "status_browse");
			if (status_browse.isEmpty())
			{
				qWarning() << "handle_selection: status_browse not found";
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::CHDIR, XmlError::PARSE));
			}
			else if (status_browse == "browse_okay")
				result[XmlResponses::CHDIR] = true;
			else if (status_browse == "server_down")
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::CHDIR, XmlError::SERVER_DOWN));
			else
			{
				qWarning() << "handle_selection: status_browse unknown";
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::CHDIR, XmlError::BROWSING));
			}
		}
		else if (tag_name == "DIDL-Lite")
		{
			QString track_url = getElement(node, "DIDL-Lite/item/res").text();
			if (track_url.isEmpty())
			{
				qWarning() << "handle_selection: didl tags not found";
				result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::TRACK_SELECTION, XmlError::PARSE));
			}
			else
			{
				EntryInfo::Type file_type = getFileType(node);

				EntryInfo::Metadata metadata;
				if (file_type == EntryInfo::AUDIO)
					metadata = getMetadata(node);

				EntryInfo entry(getElement(node, "DIDL-Lite/item/dc:title").text(),
					file_type, getElement(node, "DIDL-Lite/item/res").text(), metadata);

				QVariant value;
				value.setValue(entry);
				result[XmlResponses::TRACK_SELECTION] = value;
			}
		}

		return result;
	}

	QHash<int,QVariant> handle_browsing(const QDomNode &node, const QString cmdname, XmlResponses::Type cmdtype)
	{
		QHash<int,QVariant> result;

		QString status_browse = getTextChild(node, "status_browse");
		if (status_browse.isEmpty())
		{
			qWarning() << QString("%1: status_browse not found").arg(cmdname);
			result[XmlResponses::INVALID].setValue(XmlError(cmdtype, XmlError::PARSE));
		}
		else if (status_browse == "browse_okay")
			result[cmdtype] = true;
		else if (status_browse == "server_down")
			result[XmlResponses::INVALID].setValue(XmlError(cmdtype, XmlError::SERVER_DOWN));
		else if (status_browse == "no_such_directory" || status_browse == "already_at_root")
			result[XmlResponses::INVALID].setValue(XmlError(cmdtype, XmlError::BROWSING));
		else
		{
			qWarning() << QString("%1: status_browse unknown").arg(cmdname);
			result[XmlResponses::INVALID].setValue(XmlError(cmdtype, XmlError::BROWSING));
		}
		return result;
	}

	QHash<int,QVariant> handle_browseup(const QDomNode &node)
	{
		return handle_browsing(node, "handle_browseup", XmlResponses::BROWSE_UP);
	}

	QHash<int,QVariant> handle_setcontext(const QDomNode &node)
	{
		return handle_browsing(node, "handle_setcontext", XmlResponses::SET_CONTEXT);
	}

	QHash<int,QVariant> handle_listitems(const QDomNode &node)
	{
		QHash<int,QVariant> result;

		QString status_browse = getTextChild(node, "status_browse");
		if (status_browse == "server_down")
		{
			result[XmlResponses::INVALID].setValue(XmlError(XmlResponses::LIST_ITEMS, XmlError::SERVER_DOWN));
			return result;
		}

		UPnpEntryList list;

		list.total = getTextChild(node, "total").toUInt();
		list.start = getTextChild(node, "rank").toUInt();

		QDomNode directories = getChildWithName(node, "directories");
		foreach (const QDomNode &item, getChildren(directories, "name"))
			list.entries << EntryInfo(item.toElement().text(), EntryInfo::DIRECTORY, QString());

		QDomNode tracks = getChildWithName(node, "tracks");
		foreach (const QDomNode &item, getChildren(tracks, "file"))
		{
			EntryInfo::Type file_type = getFileType(item);

			EntryInfo::Metadata metadata;
			if (file_type == EntryInfo::AUDIO) // Maybe video, too?
				metadata = getMetadata(item);

			list.entries << EntryInfo(getElement(item, "DIDL-Lite/item/dc:title").text(),
				file_type, getElement(item, "DIDL-Lite/item/res").text(), metadata);
		}

		QVariant value;
		value.setValue(list);
		result[XmlResponses::LIST_ITEMS] = value;

		return result;
	}

	QString uuidhex(uint data, int digits)
	{
		return QString::number(data, 16).rightJustified(digits, QLatin1Char('0'));
	}

	// Workaround for the lack of Quuid::toString()
	QString uuid2str(const QUuid &uuid)
	{
		QString result;
		QChar dash = QLatin1Char('-');
		result = uuidhex(uuid.data1,8);
		result += dash;
		result += uuidhex(uuid.data2,4);
		result += dash;
		result += uuidhex(uuid.data3,4);
		result += dash;
		result += uuidhex(uuid.data4[0],2);
		result += uuidhex(uuid.data4[1],2);
		result += dash;
		for (int i = 2; i < 8; i++)
			result += uuidhex(uuid.data4[i],2);
		return result;
	}

	// An utility function to dump the xml into a file
	void dump_xml(const QString &xml, const QString &sid, int pid, bool is_request)
	{
		QDir dump_dir(DUMP_DIR);
		if (!dump_dir.exists())
			dump_dir.mkpath(DUMP_DIR);


		QString filename(QTime::currentTime().toString("hh.mm.ss.z_") + sid.left(7) +
			(is_request ? "_REQ_": "_RESP_") + QString::number(pid) + ".xml");
		QFile file(QString(DUMP_DIR) + "/" + filename);
		file.open(QIODevice::WriteOnly);
		file.write(xml.toUtf8());
		file.close();
	}
}


XmlDevice *bt_global::xml_device = 0;


XmlDevice::XmlDevice()
{
	xml_client = new XmlClient;
	connect(xml_client, SIGNAL(dataReceived(QString)), SLOT(handleData(QString)));
	connect(xml_client, SIGNAL(connectionUp()), SLOT(sendMessageQueue()));
	connect(xml_client, SIGNAL(connectionDown()), SLOT(cleanSessionInfo()));
	connect(xml_client, SIGNAL(connectionDown()), SLOT(handleClientError()));

	welcome_received = false;

	xml_handlers["WMsg"] = handle_welcome_message;
	xml_handlers["AW26C1"] = handle_upnp_server_list;
	xml_handlers["AW26C2"] = handle_selection;
	xml_handlers["AW26C7"] = handle_browseup;
	xml_handlers["AW26C15"] = handle_listitems;
	xml_handlers["AW26C11"] = handle_selection;
	xml_handlers["AW26C10"] = handle_selection;
	xml_handlers["AW26C16"] = handle_setcontext;
}

XmlDevice::~XmlDevice()
{
	xml_client->deleteLater();
}

void XmlDevice::reset()
{
	// TODO: implement
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

void XmlDevice::previousFile()
{
	sendCommand("CW26C11");
}

void XmlDevice::nextFile()
{
	sendCommand("CW26C10");
}

void XmlDevice::browseUp()
{
	sendCommand("CW26C7");
}

void XmlDevice::setContext(const QString &server, const QStringList &path)
{
	XmlArguments arg;
	arg["server"] = server;
	arg["path"] = path.join("/");
	sendCommand("CW26C16", arg);
}

void XmlDevice::listItems(unsigned int starting_element, unsigned int max_elements)
{
	XmlArguments arg;
	arg["rank"] = QString::number(starting_element);
	arg["delta"] = QString::number(max_elements);
	sendCommand("RW26C15", arg);
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

void XmlDevice::handleClientError()
{
	qWarning() << "XmlDevice::handleClientError: connection lost";
	emit error(XmlResponses::INVALID, XmlError::CLIENT);
}

void XmlDevice::sendMessageQueue()
{
	if (!welcome_received)
		return;

	while (!message_queue.isEmpty())
	{
		QPair<QString,XmlArguments> command = message_queue.takeFirst();
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

void XmlDevice::sendCommand(const QString &message, const XmlArguments &arguments)
{
	if (!xml_client->isConnected())
	{
		// Saves the message and the argument.
		// An alternative implementation could be to generate only the command
		// body, and save it. Then when the connection comes up prepend the
		// generated header. For that implementation the problem is the PID
		// which changes with the responses.
		message_queue << qMakePair<QString, XmlArguments>(message, arguments);
		xml_client->connectToHost();
	}
	else {
		++pid;
		xml_client->sendCommand(buildCommand(message, arguments));
	}
}

void XmlDevice::select(const QString &name)
{
	XmlArguments arg;
	arg["id"] = name;
	sendCommand("RW26C2", arg);
}

XmlResponse XmlDevice::parseXml(const QString &xml)
{
	XmlResponse response;
	QDomDocument doc;
	if (!doc.setContent(xml))
	{
		qWarning() << "XmlDevice::parseXml: invalid xml";
		response[XmlResponses::INVALID].setValue(XmlError(XmlResponses::INVALID, XmlError::PARSE));
		return response;
	}

	QDomElement root = doc.documentElement();

	if (root.tagName() == "OWNxml" && parseHeader(getChildWithName(root, "Hdr")))
	{

#if DUMP_OPENXML
		dump_xml(xml, sid, pid, false);
#endif

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

				if (command_name == "NACK")
					qWarning() << "XmlDevice::parseXml: nack received";
				else if (command_name == "ACK")
					qWarning() << "XmlDevice::parseXml: ack invalid";
				else
					qWarning() << "XmlDevice::parseXml: unknown command name:" << command_name;
			}
			else
				qWarning() << "XmlDevice::parseXml: command name not found";
		}
	}
	else
		qWarning() << "XmlDevice::parseXml: bad xml header";

	response[XmlResponses::INVALID].setValue(XmlError(XmlResponses::INVALID, XmlError::PARSE));
	return response;
}

bool XmlDevice::parseHeader(const QDomNode &header_node)
{
	QDomNode message_id = getChildWithName(header_node, "MsgID");
	if (message_id.isNull())
		return false;

	sid = getTextChild(message_id, "SID");
	bool ok;
	pid = getTextChild(message_id, "PID").toInt(&ok);

	if (sid.isEmpty() || !ok)
		return false;


	QDomNode dest_address = getChildWithName(header_node, "Dst");
	if (dest_address.isNull())
		return false;
	local_addr = getTextChild(dest_address, "IP");

	QDomNode src_address = getChildWithName(header_node, "Src");
	if (src_address.isNull())
		return false;
	server_addr = getTextChild(src_address, "IP");

	if (local_addr.isEmpty() || server_addr.isEmpty())
		return false;

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
		sid = uuid2str(QUuid::createUuid());
	}

	return true;
}

QString XmlDevice::buildCommand(const QString &command, const XmlArguments &arguments)
{
	QString cmd;

	if (!arguments.isEmpty())
	{
		cmd = QString("		<%1>\n").arg(command);
		QHashIterator<QString, QString> it(arguments);
		while (it.hasNext())
		{
			it.next();
			cmd += QString("			<%1>%2</%1>\n").arg(it.key()).arg(Qt::escape(it.value()));
		}
		cmd += QString("		</%1>").arg(command);
	}
	else
		cmd = QString("\t\t<%1/>").arg(command);

	QString xml = QString(command_template).arg(sid).arg(pid).arg(server_addr).arg(local_addr).arg(cmd);

#if DUMP_OPENXML
	dump_xml(xml, sid, pid, true);
#endif
	return xml;
}
