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

#include "treebrowser.h"

#include <QList>
#include <QDebug>
#include <QStringList>

#define ELEMENTS_DISPLAYED 4

namespace
{
	EntryInfo::Type directoryFileType(const QString &file_path)
	{
		QFileInfo file_info(file_path);

		if (file_info.isDir())
			return EntryInfo::DIRECTORY;

		QString ext = file_info.suffix().toLower();

		foreach (const QString &extension, getFileExtensions(EntryInfo::IMAGE))
			if (ext == extension)
				return EntryInfo::IMAGE;

		foreach (const QString &extension, getFileExtensions(EntryInfo::VIDEO))
			if (ext == extension)
				return EntryInfo::VIDEO;

		foreach (const QString &extension, getFileExtensions(EntryInfo::AUDIO))
			if (ext == extension)
				return EntryInfo::AUDIO;

	#ifdef PDF_EXAMPLE
		foreach (const QString &extension, getFileExtensions(EntryInfo::PDF))
			if (ext == extension)
				return EntryInfo::PDF;
	#endif

		return EntryInfo::UNKNOWN;
	}
}

void TreeBrowser::setFilter(int mask)
{
	filter_mask = mask;
}

DirectoryTreeBrowser::DirectoryTreeBrowser()
{
	level = 0;

	current_dir.setSorting(QDir::DirsFirst | QDir::Name);
	current_dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);
}

void DirectoryTreeBrowser::setRootPath(const QStringList &path)
{
	root_path = "/" + path.join("/");
	current_dir.setPath(root_path);
	level = 0;
	emit isRootChanged();
}

void DirectoryTreeBrowser::reset()
{
	level = 0;
	current_dir.setPath(root_path);
	emit isRootChanged();
}

DirectoryBrowserMemento *DirectoryTreeBrowser::clone()
{
	DirectoryBrowserMemento *m = new DirectoryBrowserMemento;
	m->root_path = getRootPath();
	m->mask = filter_mask;
	// for the context, we need to strip the root_path from the current directory
	// path
	QDir root_dir(root_path);
	QString diff = root_dir.relativeFilePath(current_dir.absolutePath());
	QDir ctx(diff, "", QDir::NoSort, QDir::NoDotAndDotDot);
	m->context = ctx.path().split("/", QString::SkipEmptyParts);
	return m;
}

void DirectoryTreeBrowser::restore(DirectoryBrowserMemento *m)
{
	setRootPath(m->root_path);
	setFilter(m->mask);
	setContext(m->context);
}

QStringList DirectoryTreeBrowser::getRootPath()
{
	return root_path.split("/", QString::SkipEmptyParts);
}

void DirectoryTreeBrowser::enterDirectory(const QString &name)
{
	if (!current_dir.cd(name))
	{
		emit directoryChangeError();
		return;
	}

	++level;
	emit directoryChanged();
	emit isRootChanged();
}

void DirectoryTreeBrowser::exitDirectory()
{
	if (level == 0)
		return;

	if (!current_dir.cdUp())
	{
		emit directoryChangeError();
		return;
	}

	--level;
	if (level == 0)
		emit rootDirectoryEntered();
	emit directoryChanged();
	emit isRootChanged();
}

void DirectoryTreeBrowser::getFileList()
{
	QList<QFileInfo> files_list = current_dir.entryInfoList();
	EntryInfoList result;

	foreach (const QFileInfo &item, files_list)
	{
		EntryInfo::Type file_type = directoryFileType(item.absoluteFilePath());
		if (filter_mask & file_type)
			result.append(EntryInfo(item.fileName(), file_type, item.absoluteFilePath()));
	}

	emit listReceived(result);
}

bool DirectoryTreeBrowser::isRoot()
{
	return level == 0;
}

QString DirectoryTreeBrowser::pathKey()
{
	return QString::number(level);
}

void DirectoryTreeBrowser::setContext(const QStringList &context)
{
	level = 0;
	current_dir.setPath(root_path);
	foreach (const QString &dir, context)
	{
		if (!current_dir.cd(dir))
		{
			emit genericError();
			return;
		}
		++level;
	}

	emit contextChanged();
	emit isRootChanged();
}


UPnpClientBrowser::UPnpClientBrowser(XmlDevice *d)
{
	dev = d;
	starting_element = 1;
	level = 0;

	connect(dev, SIGNAL(responseReceived(XmlResponse)), SLOT(handleResponse(XmlResponse)));
	connect(dev, SIGNAL(error(int,int)), SLOT(handleError(int,int)));
}

void UPnpClientBrowser::reset()
{
	level = 0;
	starting_element = 1;
	emit isRootChanged();
}

void UPnpClientBrowser::enterDirectory(const QString &name)
{
	context.append(name);
	if (level == 0)
		dev->selectServer(name);
	else
		dev->chDir(name);
}

void UPnpClientBrowser::exitDirectory()
{
	if (level == 0)
		return;

	if (level == 1)
	{
		--level;
		starting_element = 1;
		emit rootDirectoryEntered();
		dev->requestUPnPServers();
	}
	else
		dev->browseUp();
}

void UPnpClientBrowser::getFileList()
{
	getFileList(1);
}

void UPnpClientBrowser::getFileList(int s)
{
	Q_ASSERT_X(s > 0, "UPnpClientBrowser::getFileList", "Invalid starting element");
	starting_element = s;

	if (level == 0)
		dev->requestUPnPServers();
	else
		dev->listItems(starting_element, ELEMENTS_DISPLAYED);
}

void UPnpClientBrowser::getPreviousFileList()
{
	if (starting_element - ELEMENTS_DISPLAYED < 1)
		return;

	starting_element -= ELEMENTS_DISPLAYED;

	if (level == 0)
		emit listReceived(cached_elements.mid(starting_element -1, ELEMENTS_DISPLAYED));
	else
		dev->listItems(starting_element, ELEMENTS_DISPLAYED);
}

void UPnpClientBrowser::getNextFileList()
{
	if (starting_element + ELEMENTS_DISPLAYED > num_elements)
		return;

	starting_element += ELEMENTS_DISPLAYED;

	if (level == 0)
		emit listReceived(cached_elements.mid(starting_element -1, ELEMENTS_DISPLAYED));
	else
		dev->listItems(starting_element, ELEMENTS_DISPLAYED);
}

int UPnpClientBrowser::getNumElements()
{
	return num_elements;
}

int UPnpClientBrowser::getStartingElement()
{
	return starting_element;
}

int UPnpClientBrowser::lastQueuedCommand() const
{
	return dev->lastQueuedCommand();
}

int UPnpClientBrowser::lastAnsweredCommand() const
{
	return dev->lastAnsweredCommand();
}

bool UPnpClientBrowser::isRoot()
{
	return level == 0;
}

QString UPnpClientBrowser::pathKey()
{
	return QString::number(level);
}

DirectoryBrowserMemento *UPnpClientBrowser::clone()
{
	DirectoryBrowserMemento *m = new DirectoryBrowserMemento;
	m->root_path = getRootPath();
	m->mask = filter_mask;
	m->context = context;
	return m;
}

void UPnpClientBrowser::restore(DirectoryBrowserMemento *m)
{
	setRootPath(m->root_path);
	setFilter(m->mask);
	setContext(m->context);
}

void UPnpClientBrowser::handleResponse(const XmlResponse &response)
{
	foreach (int key, response.keys())
	{
		switch (key)
		{
		case XmlResponses::WELCOME:
		case XmlResponses::ACK:
			break;
		case XmlResponses::SERVER_LIST:
		{
			cached_elements.clear();
			foreach (const QString &server, response[key].toStringList())
				cached_elements << EntryInfo(server, EntryInfo::DIRECTORY, QString());

			num_elements = cached_elements.size();
			emit isRootChanged();
			EntryInfoList entry_list;

			if (num_elements > 0)
				entry_list = cached_elements.mid(starting_element - 1, ELEMENTS_DISPLAYED);
			emit listReceived(entry_list);
			break;
		}
		case XmlResponses::SERVER_SELECTION:
		case XmlResponses::CHDIR:
			++level;
			emit directoryChanged();
			emit isRootChanged();
			break;
		case XmlResponses::TRACK_SELECTION:
			break;
		case XmlResponses::BROWSE_UP:
			--level;
			while (level < context.size())
				context.removeLast();
			emit directoryChanged();
			emit isRootChanged();
			break;
		case XmlResponses::LIST_ITEMS:
		{
			EntryInfoList infos;
			const UPnpEntryList& list = response[key].value<UPnpEntryList>();
			num_elements = list.total;
			starting_element = list.start;
			foreach (const EntryInfo &entry, list.entries)
			{
				if (filter_mask & entry.type)
					infos << entry;
			}
			emit listReceived(infos);
			break;
		}
		case XmlResponses::SET_CONTEXT:
			level = new_context.size();
			context = new_context;
			new_context.clear();
			emit contextChanged();
			emit isRootChanged();
			break;
		default:
			Q_ASSERT_X(false, "UPnpClientBrowser::handleResponse", "Unhandled response.");
		}
	}
}

void UPnpClientBrowser::handleError(int response, int code)
{
	// Commands (and so, responses) are usually related to the navigation of items
	// (managed by this class) or to playing items (managed by the UPnpListManager).
	// However sometimes happens that a reponse can interest both: in these
	// cases we have to manage it only in one of the XmlDevice users, to avoid
	// multiple actions that can cause problems.
	// This is the case of the SERVER_DOWN error.

	switch (response)
	{
	case XmlResponses::WELCOME:
		emit genericError();
		break;
	case XmlResponses::SERVER_LIST:
	case XmlResponses::LIST_ITEMS:
		emit listRetrieveError();
		break;
	case XmlResponses::SET_CONTEXT:
			emit genericError();
		break;
	case XmlResponses::SERVER_SELECTION:
	case XmlResponses::CHDIR:
		if (code == XmlError::BROWSING)
			emit directoryChangeError();
		else if (code == XmlError::EMPTY_CONTENT)
			emit emptyDirectory();
		else
			emit genericError();
		break;
	case XmlResponses::BROWSE_UP:
		if (code == XmlError::SERVER_DOWN)
			emit genericError();
		else if (level == 1)
		{
			--level;
			while (level < context.size())
				context.removeLast();
			emit directoryChanged();
			emit isRootChanged();
		}
		else
			emit directoryChangeError();
		break;
	case XmlResponses::TRACK_SELECTION:
	case XmlResponses::INVALID:
		// Server down error is managed by the UPnpListManager class, see the comment
		// on the top of this method for more details
		if (code != XmlError::SERVER_DOWN)
			emit genericError();
		break;
	default:
		Q_ASSERT_X(false, "UPnpClientBrowser::handleError", "Unhandled response.");
	}
}

void UPnpClientBrowser::setContext(const QStringList &context)
{
	new_context = context;
	dev->setContext(context[0], context.mid(1));
}

