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

#include <QObject>
//#include <QMetaType>
#include <QHash>
#include <QVariant>

class QDomNode;
class XmlClient;
struct FilesystemEntry;

typedef QHash<int, QVariant> XmlResponse;
typedef QPair<int,QVariant> (*xmlHandler_ptr)(const QDomNode&);
typedef QList<FilesystemEntry> FilesystemEntries;


namespace XmlResponses
{
	enum Type
	{
		INVALID = -1,
		WELCOME,
		SERVER_LIST,
		SERVER_SELECTION,
		CHDIR,
		BROWSE_UP,
		LIST_ITEMS,
	};
}


struct FilesystemEntry
{
	enum Type
	{
		DIRECTORY = 0,
		TRACK
	};

	FilesystemEntry::Type type;
	QString name;
};
Q_DECLARE_METATYPE(FilesystemEntries);


class XmlDevice : public QObject
{
friend class TestXmlDevice;

Q_OBJECT
public:
	XmlDevice();
	~XmlDevice();

	void requestUPnPServers();
	void select(const QString &name);
	void browseUp();
	void listItems(int max_results);

signals:
	void responseReceived(const XmlResponse &response);

private slots:
	void handleData(const QString &data);

private:
	XmlResponse parseXml(const QString &xml);
	bool parseHeader(const QDomNode &header_node);
	QString buildCommand(const QString &command, const QString &argument = QString());

	XmlClient *xml_client;
	QHash<QString, xmlHandler_ptr> xml_handlers;

	QString sid;
	QString pid;
	QString local_addr;
	QString server_addr;
};

#endif // XMLDEVICE_H
