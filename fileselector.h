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
public:
	FileSelector(TreeBrowser *browser);

public slots:
	void itemIsClicked(int item);
	void browseUp();
	virtual void showPage();
	virtual void browse(const QString &start_path);

#ifdef BT_HARDWARE_TS_10
	// only meaningful for physical file systems
	void unmount();
#endif

signals:
	void fileClicked(int item);

protected:
	void setRootPath(const QString &start_path);
	QString getRootPath();

	/// returns the list of currently displayed files
	const QList<TreeBrowser::EntryInfo> &getFiles() const;

	/// Must be called by subclasses before displaying the file/directory list
	void setFiles(const QList<TreeBrowser::EntryInfo> &files);

	/// returns the page that should be displayed for the given directory
	int displayedPage(const QDir &directory);

protected:
	TreeBrowser *browser;

	void startOperation();
	void operationCompleted();

private:
	/// Change the current dir, return false in case of error.
	bool changePath(QString new_path);

private slots:
	void directoryChanged();
	void handleError();

#ifdef BT_HARDWARE_TS_10
	void unmounted(const QString &dir);
#endif

private:
	FileSelectorWaitDialog *working;
	QList<TreeBrowser::EntryInfo> files_list;
	QMap<QString, unsigned>  pages_indexes;
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

#endif // FILE_SELECTOR_H

