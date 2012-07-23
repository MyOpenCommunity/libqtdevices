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


#include "list_manager.h"
#ifndef BT_EXPERIENCE_TODO_REVIEW_ME
#include "displaycontrol.h" // bt_global::display
#endif


FileListManager::FileListManager()
{
	index = -1;
	total_files = -1;
}

void FileListManager::setList(const EntryInfoList &files)
{
	files_list = files;
	total_files = files.size();
	index = 0;
}

QString FileListManager::currentFilePath()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	return files_list[index].path;
}

void FileListManager::nextFile()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	++index;
	if (index >= total_files)
		index = 0;
	emit currentFileChanged();
}

void FileListManager::previousFile()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	--index;
	if (index < 0)
		index = total_files - 1;
	emit currentFileChanged();
}

int FileListManager::currentIndex()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	return index;
}

void FileListManager::setCurrentIndex(int i)
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	Q_ASSERT_X(index >= 0 && index < total_files, "FileListManager::setCurrentIndex", "index out of range");
	index = i;
}

int FileListManager::totalFiles()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	return total_files;
}

EntryInfo::Metadata FileListManager::currentMeta()
{
	return EntryInfo::Metadata();
}


UPnpListManager::UPnpListManager(XmlDevice *d)
{
	dev = d;
	connect(dev, SIGNAL(responseReceived(XmlResponse)), SLOT(handleResponse(XmlResponse)));
	connect(dev, SIGNAL(error(int,int)), SLOT(handleError(int,int)));
}

QString UPnpListManager::currentFilePath()
{
	Q_ASSERT_X(!current_file.isNull(), "UPnpListManager::currentFilePath", "Called with current_file not initialized!");
	return current_file.path;
}

void UPnpListManager::handleResponse(const XmlResponse &response)
{
	if (response.contains(XmlResponses::TRACK_SELECTION))
	{
		current_file = response[XmlResponses::TRACK_SELECTION].value<EntryInfo>();
		emit currentFileChanged();
	}
}

void UPnpListManager::handleError(int response, int code)
{
	// NOTE: See the comment on managing errors in UPnpClientBrowser::handleError
	if ((response == XmlResponses::TRACK_SELECTION || response == XmlResponses::INVALID) &&
			code == XmlError::SERVER_DOWN)
		emit serverDown();

	// Because the serverDown error can change the current page and hide the screensaver,
	// we have to restore the normal status of the display.
#ifndef BT_EXPERIENCE_TODO_REVIEW_ME
	bt_global::display->makeActive();
#endif
}

void UPnpListManager::nextFile()
{
	if (++index >= total_files)
		index = 0;
	dev->nextFile();
}

void UPnpListManager::previousFile()
{
	if (--index < 0)
		index = total_files - 1;
	dev->previousFile();
}

int UPnpListManager::currentIndex()
{
	return index;
}

int UPnpListManager::totalFiles()
{
	return total_files;
}

void UPnpListManager::setCurrentIndex(int i)
{
	index = i;
}

void UPnpListManager::setTotalFiles(int n)
{
	total_files = n;
}

void UPnpListManager::setStartingFile(EntryInfo starting_file)
{
	dev->selectFile(starting_file.name);
	current_file = starting_file;
}

EntryInfo::Metadata UPnpListManager::currentMeta()
{
	return current_file.metadata;
}

