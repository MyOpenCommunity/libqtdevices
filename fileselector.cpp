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


FileSelector::FileSelector(unsigned rows_per_page, QString start_path)
{
	setRootPath(start_path);
	working = NULL;

	current_dir.setSorting(QDir::DirsFirst | QDir::Name);
	current_dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);
}

QString FileSelector::getRootPath() const
{
	return root_path;
}

void FileSelector::browse(const QString &dir)
{
	setRootPath(dir);
	showPage();
}

const QList<QFileInfo> &FileSelector::getFiles() const
{
	return files_list;
}

void FileSelector::showPageNoReload()
{
	Selector::showPage();
}

void FileSelector::showPage()
{
	Selector::showPage();

	QLabel *l = createWaitDialog();
	QTime time_counter = startTimeCounter();

	// refresh QDir information
	current_dir.refresh();

	if (!browseFiles(current_dir, files_list))
	{
		waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
		destroyWaitDialog(l);
		emit notifyExit();
		return;
	}

	waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
	destroyWaitDialog(l);
}

void FileSelector::itemIsClicked(int item)
{
	QLabel *l = createWaitDialog();
	QTime time_counter = startTimeCounter();

	const QFileInfo& clicked_element = files_list[item];
	qDebug() << "[AUDIO] FileSelector::itemIsClicked " << item << "-> " << clicked_element.fileName();

	if (!clicked_element.exists())
		qWarning() << "[AUDIO] Error retrieving file: " << clicked_element.absoluteFilePath();

	if (clicked_element.isDir())
	{
		++level;
		if (!browseDirectory(clicked_element.absoluteFilePath()))
		{
			destroyWaitDialog(l);
			emit notifyExit();
			return;
		}
	}
	else
	{
		// save the info of old directory
		pages_indexes[current_dir.absolutePath()] = currentPage();

		emit fileClicked(item);
	}

	waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
	destroyWaitDialog(l);
}

void FileSelector::browseUp()
{
	if (level > 0)
	{
		--level;
		QLabel *l = createWaitDialog();
		QTime time_counter = startTimeCounter();

		if (!browseDirectory(QFileInfo(current_dir, "..").absoluteFilePath()))
		{
			destroyWaitDialog(l);
			emit notifyExit();
			return;
		}
		waitTimeCounter(time_counter, MEDIASERVER_MSEC_WAIT_TIME);
		destroyWaitDialog(l);
	}
	else
		emit notifyExit();
}

void FileSelector::setRootPath(const QString &start_path)
{
	root_path = start_path;
	current_dir.setPath(start_path);
	level = 0;
	pages_indexes.clear();
}

bool FileSelector::browseDirectory(QString new_path)
{
	QString old_path = current_dir.absolutePath();
	if (changePath(new_path))
	{
		if (!browseFiles(current_dir, files_list))
		{
			qDebug() << "[AUDIO] empty directory: "<< new_path;
			changePath(old_path);
			--level;
		}
		return true;
	}
	else
	{
		qDebug() << "[AUDIO] browseFiles(): path '" << new_path << "%s' doesn't exist";
		changePath(old_path);
		--level;
	}

	return browseFiles(current_dir, files_list);
}

bool FileSelector::changePath(QString new_path)
{
	// if new_path is valid changes the path and run browseFiles()
	if (QFileInfo(new_path).exists())
	{
		// save the info of old directory
		pages_indexes[current_dir.absolutePath()] = currentPage();

		QString new_path_string = QFileInfo(new_path).absoluteFilePath();
		// change path
		current_dir.setPath(new_path_string);
		return true;
	}

	return false;
}

int FileSelector::displayedPage(const QDir &directory)
{
	if (pages_indexes.contains(directory.absolutePath()))
		return pages_indexes[directory.absolutePath()];
	else
		return 0;
}

void FileSelector::destroyWaitDialog(QLabel *l)
{
	l->hide();
	l->deleteLater();
}

QLabel *FileSelector::createWaitDialog()
{
	QLabel* l = new QLabel(this);
	l->setProperty("Loading", true);
	l->setGeometry(geometry());

	l->show();
	qApp->processEvents();

	return l;
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
