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

#include <errno.h> // errno

ImageSelectionHandler::ImageSelectionHandler(const QString &file_path) :
	save_file_path(file_path)
{
	loadSlideshowFromFile();
}

void ImageSelectionHandler::compactDirectory(const QString &dir, const QFileInfoList &items_in_dir)
{
	qDebug() << "compacting directory " << dir;
	foreach (const QFileInfo &fi, items_in_dir)
		removeItem(fi.absoluteFilePath());
	insertItem(dir);
	qDebug() << "Result of compacting the directory: " << selected_images;
}

void ImageSelectionHandler::saveSlideshowToFile()
{
	QTemporaryFile f("./temp_slideshowXXXXXX.txt");
	if (!f.open())
	{
		qWarning() << "Error creating temporary file for slideshow images";
		return;
	}

	foreach (QString path, selected_images)
	{
		path = path.simplified();
		f.write(path.toLocal8Bit());
		f.write("\n");
	}
	f.close();
	QFileInfo fi(f);
	qDebug() << "Saved file to " << fi.absoluteFilePath();

	if (::rename(qPrintable(fi.absoluteFilePath()), qPrintable(save_file_path)))
		qWarning() << "Could not correctly save slideshow file, error code = " << errno;
}

void ImageSelectionHandler::loadSlideshowFromFile()
{
	QFile f(save_file_path);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "Could not read slideshow images file: " << QDir::currentPath() + save_file_path;
		return;
	}

	while (!f.atEnd())
	{
		QString path = f.readLine().simplified();
		selected_images.insert(path);
	}
	qDebug() << "loadSlideshowFromFile, result: " << selected_images;
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
	Q_ASSERT_X(isItemSelected(path), "SlideshowSelectionPage::removeCurrentFile", "Given path is not selected!");
	// simple case: path is a file and is selected explicitly
	if (selected_images.contains(path))
	{
		removePath(path);
		qDebug() << "Removing explicitly selected file: " << path;
		return;
	}

	QString basename;
	QString parent = getParentDirectory(path, &basename);
	while (!parent.isEmpty())
	{
		QFileInfoList items_in_dir = QDir(parent).entryInfoList(file_filter);
		Q_ASSERT_X(isItemSelected(parent), "SlideshowSelectionPage::removeCurrentFile", "Parent is not selected.");
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
			qDebug() << "Outermost directory: " << parent;
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

bool ImageSelectionHandler::isItemSelected(QString abs_path)
{
	while (!abs_path.isEmpty())
	{
		if (selected_images.contains(abs_path))
		{
			qDebug() << "Item " << abs_path << " is selected";
			return true;
		}

		abs_path = getParentDirectory(abs_path);
	}
	qDebug() << "Item " << abs_path << " is NOT selected";
	return false;
}

bool ImageSelectionHandler::isItemExplicitlySelected(const QString &abs_path)
{
	// TODO: this should also check if abs_path is in removed_images
	return selected_images.contains(abs_path);
}

QSet<QString> ImageSelectionHandler::getSelectedImages()
{
	return selected_images;
}
