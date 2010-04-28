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



#include "imageselectionhandler.h"

#include <QTemporaryFile>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

#include <errno.h> // errno

namespace
{
	QStringList loadTextFile(const QString &file_path)
	{
		QFile f(file_path);
		if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qWarning() << "Could not read slideshow images file: " << QDir::currentPath() + file_path;
			return QStringList();
		}

		QStringList li;
		while (!f.atEnd())
		{
			QString path = f.readLine().simplified();
			li << path;
		}
		return li;
	}

	bool saveTextFile(const QStringList &paths, const QString &file_path)
	{
		QTemporaryFile f("./temp_slideshowXXXXXX.txt");
		if (!f.open())
		{
			qWarning() << "Error creating temporary file for slideshow images";
			return false;
		}

		foreach (QString s, paths)
		{
			s = s.simplified();
			f.write(s.toLocal8Bit());
			f.write("\n");
		}
		f.close();
		QFileInfo fi(f);

		// this is not a bug, stlib functions return 0 on success
		return !(::rename(qPrintable(fi.absoluteFilePath()), qPrintable(file_path)));
	}
}


ImageIterator::ImageIterator(const QString &file_path)
{
	foreach (const QString &path, loadTextFile(file_path))
		paths << path;

	list_iter = new QMutableLinkedListIterator<QString>(paths);
	dir_iter = 0;
}

QString ImageIterator::next()
{
	if (dir_iter)
	{
		if (dir_iter->hasNext())
			return dir_iter->next();
		// iterator finished its duty, cleanup
		else
		{
			delete dir_iter;
			dir_iter = 0;
		}
	}

	if (!list_iter->hasNext())
		list_iter->toFront();

	QString path = list_iter->next();
	QFileInfo fi(path);
	if (fi.exists())
	{
		if (fi.isFile())
			return path;
		else
		{
			dir_iter = new QDirIterator(path, file_filters, QDir::Files | QDir::Readable, QDirIterator::Subdirectories);
			return dir_iter->next();
		}
	}
	// remove elements not found
	else
		list_iter->remove();
	return QString();
}

void ImageIterator::setFileFilter(const QStringList &filters)
{
	file_filters = filters;
}

void ImageIterator::saveImagesToFile()
{
	QStringList l;
	foreach (const QString &str, paths)
		l << str;
	if (!saveTextFile(l, SLIDESHOW_FILENAME))
		qWarning() << "ImageIterator::saveImagesToFile, could not save images file";
}

ImageIterator::~ImageIterator()
{
	delete dir_iter;
	delete list_iter;
}



ImageSelectionHandler::ImageSelectionHandler(const QString &file_path) :
	save_file_path(file_path)
{
	loadSlideshowFromFile();
}

bool ImageSelectionHandler::compactDirectory(const QString &dir)
{
	QDir d(dir);
	QStringList deleting_items;

	// check if every item in the directory is really selected
	bool are_all_selected = true;
	foreach (const QFileInfo &fi, d.entryInfoList(file_filter))
	{
		if (selected_images.contains(fi.absoluteFilePath()))
			deleting_items << fi.absoluteFilePath();
		else
		{
			are_all_selected = false;
			break;
		}
	}

	if (are_all_selected)
	{
		foreach (const QString &del, deleting_items)
			removeItem(del);
		insertItem(dir);
		dirty = false;
	}

	return are_all_selected;
}

void ImageSelectionHandler::saveSlideshowToFile()
{
	if (!saveTextFile(selected_images.toList(), save_file_path))
		qWarning() << "ImageSelectionHandler::saveSlideshowToFile, could not correctly save slideshow file";
}

void ImageSelectionHandler::loadSlideshowFromFile()
{
	selected_images = QSet<QString>::fromList(loadTextFile(save_file_path));
}

void ImageSelectionHandler::insertItem(const QString &path)
{
	selected_images.insert(path);
	dirty = true;
}

/*
 * Necessary in case a directory contains other file types in addition to what is displayed to the user.
 * Example:
 * the user is shown only .jpeg files; when he deselects a file from a selected directory, removeItem() must add
 * all files inside the directory (except what is deselected). When the user adds again the file, compactDirectory()
 * will get a filtered list of files and check if every one is selected. If the file list contains other files (eg. mp3)
 * it won't be able to compact the directory.
 */
void ImageSelectionHandler::setFileFilter(const QStringList &filters)
{
	file_filter = filters;
}

void ImageSelectionHandler::removePath(const QString &path)
{
	selected_images.remove(path);
	dirty = true;
}

void ImageSelectionHandler::removeItem(const QString &path)
{
	Q_ASSERT_X(isItemSelected(path), "SlideshowSelectionPage::removeItem", "Given path is not selected!");
	// simple case: path is a file and is selected explicitly
	if (selected_images.contains(path))
	{
		removePath(path);
		return;
	}

	QString basename;
	QString parent = getParentDirectory(path, &basename);
	while (!parent.isEmpty())
	{
		QFileInfoList items_in_dir = QDir(parent).entryInfoList(file_filter);
		Q_ASSERT_X(isItemSelected(parent), "SlideshowSelectionPage::removeItem", "Parent is not selected.");
		// first add all files and directories excluding current basename
		foreach (const QFileInfo &fi, items_in_dir)
		{
			if (fi.fileName() != basename)
				insertItem(fi.absoluteFilePath());
		}

		// then exclude parent directory itself and terminate if we are the outermost selected directory
		if (selected_images.contains(parent))
		{
			removePath(parent);
			break;
		}
		parent = getParentDirectory(parent, &basename);
	}
}

QString ImageSelectionHandler::getParentDirectory(const QString &path, QString *file_path)
{
	// find the previous separator and remove everything from that point
	int pos = path.lastIndexOf(QDir::separator(), -2);
	if (file_path)
		*file_path = path.mid(pos + 1);
	if (pos < 0)
		return QString();
	else
		return path.left(pos);
}

bool ImageSelectionHandler::isItemSelected(const QString &abs_path)
{
	if (selected_images.contains(abs_path))
		return true;
	else
	{
		QString path = abs_path;
		while (!path.isEmpty())
		{
			if (selected_images.contains(path))
				return true;

			path = getParentDirectory(path);
		}
	}

	// try to compact a directory
	if (QFileInfo(abs_path).isDir() && dirty)
	{
		foreach (const QString &item, selected_images)
		{
			if (item.startsWith(abs_path))
				return compactDirectory(abs_path);
		}
	}

	return false;
}

QSet<QString> ImageSelectionHandler::getSelectedImages()
{
	return selected_images;
}
