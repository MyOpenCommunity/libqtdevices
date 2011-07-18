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
#include "displaycontrol.h" // bt_global::display
#include "labels.h" // WaitLabel

#ifdef BT_HARDWARE_PXA270
#include "mount_watcher.h" // bt_global::mount_watcher
#endif

#include <QTime>
#include <QDebug>
#include <QLabel>
#include <QVariant>

#define MEDIASERVER_MSEC_WAIT_TIME 300


FileSelector::FileSelector(TreeBrowser *_browser)
{
	browser = _browser;

	working = NULL;

	connect(browser, SIGNAL(directoryChanged()), SLOT(directoryChanged()));
	connect(this, SIGNAL(Closed()), this, SLOT(cleanUp()));
	connect(bt_global::display, SIGNAL(startscreensaver(Page*)), SLOT(screenSaverStarted(Page*)));

	connect(browser, SIGNAL(directoryChangeError()), SLOT(directoryChangeError()));
	connect(browser, SIGNAL(listRetrieveError()), SLOT(handleError()));
	connect(browser, SIGNAL(emptyDirectory()), SLOT(emptyDirectory()));
	connect(browser, SIGNAL(genericError()), SLOT(handleError()));

	mounted_filesystem = true;
#ifdef BT_HARDWARE_PXA270
	// since this checks the root file path, it's OK to use it for all instances
	connect(bt_global::mount_watcher, SIGNAL(directoryUnmounted(const QString &, MountType)),
		SLOT(unmounted(const QString &)));
#endif
}

void FileSelector::screenSaverStarted(Page *curr)
{
	if (curr == this && working)
	{
		working->abort();
		working = 0;
	}
}

void FileSelector::cleanUp()
{
	if (working)
	{
		working->abort();
		working = 0;
	}
}

const EntryInfoList &FileSelector::getFiles() const
{
	return files_list;
}

void FileSelector::setFiles(const EntryInfoList &files)
{
	files_list = files;
}

void FileSelector::showPage()
{
	if (!mounted_filesystem)
		emit Closed();
	else
	{
		ScrollablePage::showPage();

		// refresh directory information
		startOperation();
		browser->getFileList();
	}
}

void FileSelector::itemIsClicked(int item)
{
	const EntryInfo& clicked_element = files_list[item];
	qDebug() << "FileSelector::itemIsClicked " << item << "-> " << clicked_element.name;

	// save the info of old directory
	pages_indexes[browser->pathKey()] = currentPage();

	if (clicked_element.type == EntryInfo::DIRECTORY)
	{
		startOperation();
		browser->enterDirectory(clicked_element.name);
	}
	else
		emit fileClicked(item);
}

void FileSelector::resetDisplayedPage()
{
	pages_indexes.remove(browser->pathKey());
}

int FileSelector::currentPage()
{
	return page_content->currentPage();
}

void FileSelector::browseUp()
{
	if (!browser->isRoot())
	{
		startOperation();
		resetDisplayedPage();
		browser->exitDirectory();
	}
	else
		emit Closed();
}

void FileSelector::pageDown()
{
	pages_indexes[browser->pathKey()] = currentPage();
}

void FileSelector::pageUp()
{
	pages_indexes[browser->pathKey()] = currentPage();
}

void FileSelector::directoryChanged()
{
	browser->getFileList();
}

void FileSelector::handleError()
{
	operationCompleted();
	pages_indexes.clear();
	files_list.clear();
	browser->reset();
	emit Closed();
}

void FileSelector::emptyDirectory()
{
	// Because the bt_contropoint process does not change the directory if it
	// is empty we only re-show the page.
	operationCompleted();
}

void FileSelector::directoryChangeError()
{
	browser->getFileList();
}

void FileSelector::setRootPath(const QString &start_path)
{
	mounted_filesystem = !start_path.isEmpty();
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

	working = new WaitLabel(this, MEDIASERVER_MSEC_WAIT_TIME);
	connect(this, SIGNAL(Closed()), working, SLOT(abort()));
}

void FileSelector::operationCompleted()
{
	if (!working)
		return;

	working->waitForTimeout();
	working = NULL;
}

#ifdef BT_HARDWARE_PXA270

// methods for physical file systems
void FileSelector::unmount()
{
	bt_global::mount_watcher->unmount(getRootPath());
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


