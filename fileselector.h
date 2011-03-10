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


#ifndef FILE_SELECTOR_H
#define FILE_SELECTOR_H

#include "scrollablepage.h"
#include "treebrowser.h"

#include <QMap>
#include <QList>
#include <QLabel>
#include <QTime>

class FileSelectorWaitDialog;
class QShowEvent;
class QHideEvent;

/*!
	\ingroup Core
	\brief File navigation base class

	Provides basic support for directory navigation.

	Handling clicks on files and displaying file list is delegated to subclasses,
	that must handle the TreeBrowser::listReceived() signal emitted by the TreeBrowser.
 */
class FileSelector : public ScrollablePage
{
Q_OBJECT
public slots:
	virtual void showPage();
	virtual void cleanUp();

#ifdef BT_HARDWARE_TS_10
	// only meaningful for physical file systems
	void unmount();
#endif

public:
	void setRootPath(const QString &start_path);

signals:
	void fileClicked(int item);

protected:
	FileSelector(TreeBrowser *browser);
	QString getRootPath();

	/// returns the list of currently displayed files
	const EntryInfoList &getFiles() const;

	/// Must be called by subclasses before displaying the file/directory list
	void setFiles(const EntryInfoList &files);

	/// returns the page that should be displayed for the given directory
	int displayedPage(const QString &directory);

	void startOperation();
	void operationCompleted();

	virtual int currentPage();

	TreeBrowser *browser;
	QHash<QString, unsigned>  pages_indexes;

	void resetDisplayedPage();

protected slots:
	virtual void itemIsClicked(int item);
	virtual void emptyDirectory();

	// Subclasses should call this method when the user clicks on the navbar.
	virtual void browseUp();
	virtual void pageUp();
	virtual void pageDown();

private slots:
	void directoryChanged();
	void handleError();
	void directoryChangeError();
	void screenSaverStarted(Page *curr);

#ifdef BT_HARDWARE_TS_10
	void unmounted(const QString &dir);
#endif

private:
	/// Change the current dir, return false in case of error.
	bool changePath(QString new_path);

	FileSelectorWaitDialog *working;
	EntryInfoList files_list;
};


class FileSelectorWaitDialog : public QLabel
{
Q_OBJECT
public:
	FileSelectorWaitDialog(Page *parent, int timeout);

	void waitForTimeout();

public slots:
	void abort();

private:
	QTime elapsed;
	int timeout;
};


// A factory class that can be used to generate new instances of a FileSelector
// (or one of its children) class.
class FileSelectorFactory
{
public:
	virtual FileSelector* getFileSelector() = 0;
};


#endif // FILE_SELECTOR_H

