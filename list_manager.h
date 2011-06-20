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


#ifndef LIST_MANAGER_H
#define LIST_MANAGER_H

#include "generic_functions.h" // EntryInfoList
#include "xmldevice.h" // XmlResponse

#include <QObject>



// The interface for the manager of a list of 'files'.
class ListManager : public QObject
{
Q_OBJECT
public:
	virtual QString currentFilePath() = 0;

	virtual void nextFile() = 0;
	virtual void previousFile() = 0;

	virtual int currentIndex() = 0;
	virtual int totalFiles() = 0;

	virtual EntryInfo::Metadata currentMeta() = 0;

signals:
	void currentFileChanged();
};


// Implements the ListManager interface for files or virtual radio ip item.
// All the items in a directory/album are loaded at the beginning, without regarding
// the number of item displayed.
class FileListManager : public ListManager
{
public:
	FileListManager();
	virtual QString currentFilePath();

	virtual void nextFile();
	virtual void previousFile();

	virtual int currentIndex();
	virtual int totalFiles();

	virtual EntryInfo::Metadata currentMeta();

	// FileListManager specific methods
	void setCurrentIndex(int i);
	void setList(const EntryInfoList &files);

private:
	int index, total_files;
	EntryInfoList files_list;
};


// Implements the ListManager interface for files retrieved from upnp.
// The items in a directory/album are loaded in using the XmlDevice, and
// only the items to display are requested to the upnp server, in order to
// improve the performance.
class UPnpListManager : public ListManager
{
Q_OBJECT
public:
	UPnpListManager(XmlDevice *dev);
	virtual QString currentFilePath();

	virtual void nextFile();
	virtual void previousFile();

	virtual int currentIndex();
	virtual int totalFiles();

	virtual EntryInfo::Metadata currentMeta();

	// UPnpListManager specific methods
	void setStartingFile(EntryInfo starting_file);
	void setCurrentIndex(int i);
	void setTotalFiles(int n);

signals:
	void serverDown();

private slots:
	void handleResponse(const XmlResponse &response);
	void handleError(int response, int code);

private:
	int index, total_files;
	EntryInfo current_file;
	XmlDevice *dev;
};


#endif // LIST_MANAGER_H
