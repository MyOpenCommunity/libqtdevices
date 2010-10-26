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


QList<TreeBrowser::EntryInfo> TreeBrowser::filterEntries(const QList<TreeBrowser::EntryInfo> &entries, QStringList file_filters)
{
	QList<TreeBrowser::EntryInfo> res;

	foreach (const TreeBrowser::EntryInfo &item, entries)
		if (item.is_directory || !file_filters.size() || QDir::match(file_filters, item.name))
			res.append(item);

	return res;
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
	emit directoryChanged();
}

void DirectoryTreeBrowser::getFileUrl(const QString &file)
{
	QFileInfo path(current_dir, file);

	if (!path.exists())
	{
		emit urlRetrieveError();
		return;
	}

	emit urlReceived(path.absoluteFilePath());
}

void DirectoryTreeBrowser::getAllFileUrls(const QStringList &files)
{
	QStringList urls;

	foreach (QString file, files)
	{
		QFileInfo path(current_dir, file);

		if (path.exists())
			urls.append(path.absoluteFilePath());
	}

	emit allUrlsReceived(urls);
}

void DirectoryTreeBrowser::getFileList()
{
	QList<QFileInfo> files_list = current_dir.entryInfoList();
	QList<EntryInfo> result;

	foreach (const QFileInfo &item, files_list)
		result.append(EntryInfo(item.fileName(), item.isDir()));

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


FileSelector::FileSelector(TreeBrowser *_browser)
{
	browser = _browser;
	working = NULL;

	connect(browser, SIGNAL(directoryChanged()), SLOT(directoryChanged()));

	// maybe it's a bit harsh to close the navigation for all errors, but it's
	// probably the safest choice
	connect(browser, SIGNAL(directoryChangeError()), SIGNAL(Closed()));
	connect(browser, SIGNAL(urlRetrieveError()), SIGNAL(Closed()));
	connect(browser, SIGNAL(allUrlsRetrieveError()), SIGNAL(Closed()));
	connect(browser, SIGNAL(listRetrieveError()), SIGNAL(Closed()));

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
		Selector::showPage();

		// refresh directory information if the file list is empty
		startOperation();
		browser->getFileList();
	}
	else
		Selector::showPage();
}

void FileSelector::itemIsClicked(int item)
{
	const TreeBrowser::EntryInfo& clicked_element = files_list[item];
	qDebug() << "[AUDIO] FileSelector::itemIsClicked " << item << "-> " << clicked_element.name;

	if (clicked_element.is_directory)
	{
		startOperation();
		browser->enterDirectory(clicked_element.name);
	}
	else
	{
		// save the info of old directory
		pages_indexes[browser->pathKey()] = currentPage();

		emit fileClicked(item);
	}
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

int FileSelector::displayedPage(const QDir &directory)
{
	if (pages_indexes.contains(directory.absolutePath()))
		return pages_indexes[directory.absolutePath()];
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
