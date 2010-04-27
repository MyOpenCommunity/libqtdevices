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



#ifndef IMAGESELECTIONHANDLER_H
#define IMAGESELECTIONHANDLER_H

#include <QString>
#include <QStringList>
#include <QSet>
#include <QFileInfo>

/**
 * Handles image selection/removal and saving to file
 */
class ImageSelectionHandler
{
friend class TestImageSelection;
public:
	/*
	 * \param file_path Path of the file to save/load the image list.
	 * TODO: Can be a QIODevice?
	 */
	ImageSelectionHandler(const QString &file_path);

	/*
	 * Set a filter for operations that need file filtering, eg. removeItem() and compactDirectory()
	 */
	void setFileFilter(const QStringList &filters);

	/*
	 * Check if an item is selected.
	 * By design we don't query the interface but rely only on the QSets below (it may well be that the interface
	 * doesn't hold as many buttons as items in the directory).
	 * Use it sparingly since it involves 1 lookup for each level of depth of the given path. For example the path:
	 * /media/disk/photos/img001.jpg
	 * will require 4 lookups if the file and any directory below it are not selected.
	 *
	 * TODO: compact a directory if dirty == true
	 */
	bool isItemSelected(const QString &abs_path);

	/*
	 * Check if an item is explicitly selected, don't check for parent directories.
	 */
	bool isItemExplicitlySelected(const QString &abs_path);

	void insertItem(const QString &path);

	/*
	 * Remove a file from currently selected set.
	 * Assumes that either path or one of its containing directories is selected.
	 * It operates by looking for the containing directory, deselects it, then adds all files and directories
	 * that do NOT contain the path; then recurse into the not selected directory.
	 * Eg. path = /usr/local/images/slideshow/photo.jpg. If /usr/local/images is selected:
	 * 1. remove /usr/local/images
	 * 2. add /usr/local/images/ *.jpg and directories (except slideshow/)
	 * 3. add /usr/local/images/slideshow/ *.jpg and directories (except photo.jpg)
	 */
	void removeItem(const QString &path);

	void saveSlideshowToFile();
	void loadSlideshowFromFile();

	QSet<QString> getSelectedImages();

private:
	/*
	 * Compact the given directory.
	 *
	 * Get a fileinfo list from the filesystem; if every entry is selected, substitute them all with dir.
	 * \param dir Directory to be compacted
	 * \return True if the directory is compacted, false otherwise.
	 */
	bool compactDirectory(const QString &dir);

	/*
	 * Remove a path from selected_images and set dirty to true
	 */
	void removePath(const QString &path);

	/*
	 * Extract parent directory path from parameter.
	 * Eg. /usr/local/file.txt will return /usr/local (without final separator). file_path will be file.txt
	 *
	 * \param path Path you want the parent directory of
	 * \param file_path If not NULL, name of the file or directory stripped away
	 * \return Empty string if no parent is found, the parent directory otherwise.
	 */
	QString getParentDirectory(const QString &path, QString *file_path = 0);

	// selected_images: contains full path names for files. Full path names for directories if all files and dir
	//                  below are selected.
	QSet<QString> selected_images;

	QStringList file_filter;
	QString save_file_path;

	// set whenever the user inserts or removes an item, used to check if a directory should be compacted
	bool dirty;
};

#endif // IMAGESELECTIONHANDLER_H
