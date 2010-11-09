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


#include "fileselector.h"
#include "icondispatcher.h"
#ifdef BT_HARDWARE_TS_10
#include "mount_watcher.h"
#endif

#include <QTime>
#include <QDebug>
#include <QLabel>
#include <QApplication>
#include <QVariant>


#define MEDIASERVER_MSEC_WAIT_TIME 2000

inline QTime startTimeCounter()
{
	QTime timer;
	timer.start();
	return timer;
}

inline void waitTimeCounter(const QTime& timer, int msec)
{
	int wait_time = msec - timer.elapsed();
	if (wait_time > 0)
		usleep(wait_time * 1000);
}


FileSelector::FileSelector(TreeBrowser *_browser)
{
	browser = _browser;
	working = NULL;

	connect(browser, SIGNAL(directoryChanged()), SLOT(directoryChanged()));
	connect(this, SIGNAL(Closed()), this, SLOT(cleanUp()));

	// maybe it's a bit harsh to close the navigation for all errors, but it's
	// probably the safest choice
	connect(browser, SIGNAL(directoryChangeError()), SLOT(handleError()));
	connect(browser, SIGNAL(listRetrieveError()), SLOT(handleError()));
	connect(browser, SIGNAL(genericError()), SLOT(handleError()));

#ifdef BT_HARDWARE_TS_10
	// since this checks the root file path, it's OK to use it for all instances
	connect(&MountWatcher::getWatcher(), SIGNAL(directoryUnmounted(const QString &, MountType)),
		SLOT(unmounted(const QString &)));
#endif
}

void FileSelector::browse(const QString &dir)
{
	setRootPath(dir);

	// showPage automatically refreshes the file list
	showPage();
}

const QList<TreeBrowser::EntryInfo> &FileSelector::getFiles() const
{
	return files_list;
}

void FileSelector::setFiles(const QList<TreeBrowser::EntryInfo> &files)
{
	files_list = files;
}

void FileSelector::showPage()
{
	if (getRootPath().isEmpty())
	{
		// unmounted file system
		emit Closed();
	}
	else if (files_list.size() == 0)
	{
		ScrollablePage::showPage();

		// refresh directory information if the file list is empty
		startOperation();
		browser->getFileList();
	}
	else
		ScrollablePage::showPage();
}

void FileSelector::itemIsClicked(int item)
{
	const TreeBrowser::EntryInfo& clicked_element = files_list[item];
	qDebug() << "[AUDIO] FileSelector::itemIsClicked " << item << "-> " << clicked_element.name;

	// save the info of old directory
	pages_indexes[browser->pathKey()] = page_content->currentPage();

	if (clicked_element.type == DIRECTORY)
	{
		startOperation();
		browser->enterDirectory(clicked_element.name);
	}
	else
		emit fileClicked(item);
}

void FileSelector::browseUp()
{
	if (!browser->isRoot())
	{
		startOperation();

		browser->exitDirectory();
	}
	else
		emit Closed();
}

void FileSelector::directoryChanged()
{
	browser->getFileList();
}

void FileSelector::handleError()
{
	operationCompleted();
	emit Closed();
}

void FileSelector::setRootPath(const QString &start_path)
{
	browser->setRootPath(start_path.split("/", QString::SkipEmptyParts));
	pages_indexes.clear();
	files_list.clear();
}

QString FileSelector::getRootPath()
{
	return "/" + browser->getRootPath().join("/");
}

int FileSelector::displayedPage(const QString &directory)
{
	if (pages_indexes.contains(directory))
		return pages_indexes[directory];
	else
		return 0;
}

void FileSelector::startOperation()
{
	Q_ASSERT_X(working == NULL, "FileSelector::startOperation", "Multiple operations in progress");

	working = new FileSelectorWaitDialog(this, MEDIASERVER_MSEC_WAIT_TIME);

	connect(this, SIGNAL(Closed()), working, SLOT(abort()));
}

void FileSelector::operationCompleted()
{
	if (!working)
		return;

	working->waitForTimeout();
	working = NULL;
}

#ifdef BT_HARDWARE_TS_10

// methods for physical file systems
void FileSelector::unmount()
{
	MountWatcher::getWatcher().unmount(getRootPath());
}

void FileSelector::unmounted(const QString &dir)
{
	if (dir == getRootPath())
	{
		setRootPath("");
		if (isVisible())
			emit Closed();
	}
}

#endif


FileSelectorWaitDialog::FileSelectorWaitDialog(Page *parent, int _timeout) :
	QLabel(parent), timeout(_timeout)
{
	elapsed = startTimeCounter();

	setProperty("Loading", true);
	setGeometry(parent->geometry());

	show();
	qApp->processEvents();
}

void FileSelectorWaitDialog::waitForTimeout()
{
	waitTimeCounter(elapsed, timeout);

	hide();
	deleteLater();
}

void FileSelectorWaitDialog::abort()
{
	hide();
	deleteLater();
}
