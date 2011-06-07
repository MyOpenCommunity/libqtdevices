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
#include "itemlist.h"

class QDomNode;
class SlideshowPage;
class VideoPlayerPage;
class AudioPlayerPage;
class NavigationBar;

#ifdef PDF_EXAMPLE
class PdfPage;
#endif



class MultimediaList : public ItemList
{
Q_OBJECT
public:
	MultimediaList(QWidget *parent, int rows_per_page);

protected:
	virtual void addHorizontalBox(QGridLayout *layout, const ItemInfo &item, int id_btn);
};



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
	MultimediaFileListPage(TreeBrowser *browser, int filters = EntryInfo::ALL, bool mount_enabled = true);

	void cleanUp();

protected:
	virtual int currentPage();

protected slots:
	virtual void browseUp();
	virtual void pageUp();
	virtual void pageDown();
	virtual void itemIsClicked(int item);
	virtual void emptyDirectory();
	virtual void directoryChangeError();
	virtual void handleError();

private slots:
	void startPlayback(int item);
	void displayFiles(const EntryInfoList &list);

	void audioPageClosed();
	void loopDetected();
	void handleServerDown();

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
	EntryInfo::Type last_clicked_type;

	int rows_per_page;

	NavigationBar *nav_bar;
	PageTitleWidget *title_widget;

	QStringList navigation_context;

	QHash<QString, unsigned>  playing_pages_indexes;
	QStringList playing_navigation_context;

	void connectAudioPage();

#ifdef PDF_EXAMPLE
	PdfPage *pdfdisplay;
#endif
};


// Used in MultimediaSectionPage to create more instances with the same type and
// filters.
class MultimediaFileListFactory : public FileSelectorFactory
{
public:
	MultimediaFileListFactory(TreeBrowser::Types type, int filters = EntryInfo::ALL, bool mount_enabled = true);
	virtual FileSelector* getFileSelector();

private:
	int filters;
	TreeBrowser::Types type;
	bool mount_enabled;
	static MultimediaFileListPage *upnp_page;
};

#endif // MULTIMEDIA_FILELIST_H
