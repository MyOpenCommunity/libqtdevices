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
#include "upnpclientbrowser.h"

UPnpClientBrowser::UPnpClientBrowser()
{
	dev = new XmlDevice();
	level = 0;
}

UPnpClientBrowser::~UPnpClientBrowser()
{
	dev->deleteLater();
}

void UPnpClientBrowser::enterDirectory(const QString &name)
{
	if (level == 0)
		dev->selectServer(name);
	else
		dev->chDir(name);
}

void UPnpClientBrowser::exitDirectory()
{
	if (level == 0)
		return;
	else
		dev->browseUp();
}

void UPnpClientBrowser::getFileUrl(const QString &file)
{
	if (!file.isEmpty())
		dev->selectFile(file);
}

void UPnpClientBrowser::getAllFileUrls(const QStringList &files)
{
	// TODO: implement
	return;
}

void UPnpClientBrowser::getFileList()
{
	dev->listItems(-1);
}

bool UPnpClientBrowser::isRoot()
{
	return level == 0;
}

QString UPnpClientBrowser::pathKey()
{
	return QString::number(level);
}

void UPnpClientBrowser::handleResponse(const XmlResponse &response)
{
	foreach (int key, response.keys())
	{
		switch (key)
		{
		case XmlResponses::WELCOME:
			break;
		case XmlResponses::SERVER_LIST:
			{
				QList<TreeBrowser::EntryInfo> infos;
				foreach (const QString &server, response[key].toStringList())
					infos << TreeBrowser::EntryInfo(server, true);
				emit listReceived(infos);
			}
			break;
		case XmlResponses::SERVER_SELECTION:
		case XmlResponses::CHDIR:
			++level;
			emit directoryChanged();
			break;
		case XmlResponses::TRACK_SELECTION:
			{
				QString url = response[key].toString();
				emit urlReceived(url);
			}
			break;
		case XmlResponses::BROWSE_UP:
			--level;
			emit directoryChanged();
			break;
		case XmlResponses::LIST_ITEMS:
			{
				QList<TreeBrowser::EntryInfo> infos;
				foreach (const FilesystemEntry &entry, response[key].value<FilesystemEntries>())
					infos << TreeBrowser::EntryInfo(entry.name, entry.type == FilesystemEntry::DIRECTORY);
				emit listReceived(infos);
			}
			break;
		default:
			Q_ASSERT_X(false, "UPnpClientBrowser::handleResponse", "Unhandled resposne.");
		}
	}
}

void UPnpClientBrowser::handleError(int response, int code)
{
	switch (response)
	{
	case XmlResponses::WELCOME:
		break;
	case XmlResponses::SERVER_LIST:
	case XmlResponses::LIST_ITEMS:
		emit listRetrieveError();
		break;
	case XmlResponses::SERVER_SELECTION:
	case XmlResponses::CHDIR:
	case XmlResponses::BROWSE_UP:
		emit directoryChangeError();
		break;
	case XmlResponses::TRACK_SELECTION:
		emit urlRetrieveError();
		break;
	default:
		Q_ASSERT_X(false, "UPnpClientBrowser::handleResponse", "Unhandled resposne.");
	}
}
