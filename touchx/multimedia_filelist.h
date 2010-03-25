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


#ifndef MULTIMEDIA_FILELIST_H
#define MULTIMEDIA_FILELIST_H

#include "fileselector.h"

class QDomNode;
class ItemList;
class SlideshowPage;
class VideoPlayerPage;
class AudioPlayerPage;


class MultimediaFileListPage : public FileSelector
{
Q_OBJECT
public:
	enum Type
	{
		UNKNOWN = -1,
		DIRECTORY = 0,
		AUDIO = 1,
		VIDEO = 2,
		IMAGE = 3
	};

	typedef ItemList ContentType;

	MultimediaFileListPage();

	void browse(const QString &dir);

public slots:
	virtual void nextItem();
	virtual void prevItem();

signals:
	void displayImages(QList<QString> images, unsigned element);
	void displayVideos(QList<QString> images, unsigned element);
	void playAudioFiles(QList<QString> images, unsigned element);

protected:
	virtual bool browseFiles(const QDir &directory, QList<QFileInfo> &files);
	virtual int currentPage();

private:
	static Type fileType(const QFileInfo &file);

	// returns the files in the current directory with the same type as the "item"
	// file; on return, type contains the file type and current is the index of the
	// item in the output list
	QList<QString> filterFileList(int item, Type &type, int &current);

private slots:
	void startPlayback(int item);
	void unmount();
	void unmounted(const QString &path);

private:
	// icons for different file type
	QList<QString> file_icons;

	// button icons for files/directories
	QString play_file, browse_directory;

	// root path, used to unmount the device
	QString root_path;

	// pages to display video/images
	SlideshowPage *slideshow;
	VideoPlayerPage *videoplayer;
	AudioPlayerPage *audioplayer;
};

#endif // MULTIMEDIA_FILELIST_H
