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
#include "generic_functions.h"

class QDomNode;
class ItemList;
class SlideshowPage;
class VideoPlayerPage;
class AudioPlayerPage;
#ifdef PDF_EXAMPLE
class PdfPage;
#endif

/*!
	\ingroup Multimedia
	\brief Filesystem browser for multimedia files (audio/video/images).
 */
class MultimediaFileListPage : public FileSelector
{
Q_OBJECT
public:
	typedef ItemList ContentType;

	// Leave the filters parameter empty to not apply any filter,
	// pass a QStringList (made with getFileFilter()) to display
	// only wanted file types.
	MultimediaFileListPage(TreeBrowser *browser, int filters = ALL);

	void cleanUp();

signals:
	void displayImages(QList<QString> images, unsigned element);
	void displayVideos(QList<QString> images, unsigned element);
	void playAudioFiles(QList<QString> images, unsigned element);
#ifdef PDF_EXAMPLE
	void displayPdf(QString file);
#endif

private slots:
	void startPlayback(int item);
	void displayFiles(const QList<TreeBrowser::EntryInfo> &list);

private:
	// icons for different file type
	QHash<int,QString> file_icons;

	// button icons for files/directories
	QString play_file, browse_directory;

	// pages to display video/images
	SlideshowPage *slideshow;
	VideoPlayerPage *videoplayer;
	AudioPlayerPage *audioplayer;

	// last clicked item (for files)
	int last_clicked;
	MultimediaFileType last_clicked_type;

#ifdef PDF_EXAMPLE
	PdfPage *pdfdisplay;
#endif
};

#endif // MULTIMEDIA_FILELIST_H
