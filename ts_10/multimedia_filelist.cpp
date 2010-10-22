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


#include "multimedia_filelist.h"
#include "navigation_bar.h"
#include "skinmanager.h"
#include "itemlist.h"
#include "slideshow.h"
#include "videoplayer.h"
#include "audioplayer.h"
#include "mount_watcher.h"
#ifdef PDF_EXAMPLE
#include "examples/pdf/pages/pdfpage.h"
#endif

#include <QLayout>
#include <QDebug>




#define ARRAY_SIZE(x) int(sizeof(x) / sizeof((x)[0]))

// transforms an extension to a pattern (es. "wav" -> "*.[wW][aA][vV]")
void addFilters(QStringList &filters, const char **extensions, int size)
{
	for (int i = 0; i < size; ++i)
	{
		QString pattern = "*.";

		for (const char *c = extensions[i]; *c; ++c)
			pattern += QString("[%1%2]").arg(QChar(*c)).arg(QChar::toUpper((unsigned short)*c));

		filters.append(pattern);
	}
}

MultimediaFileListPage::MultimediaFileListPage(const QStringList &filters)
	: FileSelector(4, "/"), file_filters(filters)
{
	ItemList *item_list = new ItemList(0, 4);
	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(this, SIGNAL(fileClicked(int)), SLOT(startPlayback(int)));

	connect(this, SIGNAL(Closed()), item_list, SLOT(clear()));

	PageTitleWidget *title_widget = new PageTitleWidget(tr("Folder"), SMALL_TITLE_HEIGHT);
	connect(item_list, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));

	NavigationBar *nav_bar = new NavigationBar("eject");

	buildPage(item_list, nav_bar, 0, title_widget);
	layout()->setContentsMargins(0, 5, 25, 10);

	connect(&MountWatcher::getWatcher(), SIGNAL(directoryUnmounted(const QString &, MountType)),
		SLOT(unmounted(const QString &)));
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(unmount()));
	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));
	connect(this, SIGNAL(notifyExit()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(upClick()), item_list, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), item_list, SLOT(nextItem()));
	connect(item_list, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	// order here must match the order in enum Type
	file_icons.append(bt_global::skin->getImage("directory_icon"));
	file_icons.append(bt_global::skin->getImage("audio_icon"));
	file_icons.append(bt_global::skin->getImage("video_icon"));
	file_icons.append(bt_global::skin->getImage("image_icon"));
#ifdef PDF_EXAMPLE
	file_icons.append(bt_global::skin->getImage("pdf_icon"));
#endif

	play_file = bt_global::skin->getImage("play_file");
	browse_directory = bt_global::skin->getImage("browse_directory");

	slideshow = new SlideshowPage;
	connect(this, SIGNAL(displayImages(QList<QString>, unsigned)),
		slideshow, SLOT(displayImages(QList<QString>, unsigned)));
	connect(slideshow, SIGNAL(Closed()), SLOT(showPageNoReload()));

	videoplayer = new VideoPlayerPage;
	connect(this, SIGNAL(displayVideos(QList<QString>, unsigned)),
		videoplayer, SLOT(displayVideos(QList<QString>, unsigned)));

	audioplayer = AudioPlayerPage::getAudioPlayerPage(AudioPlayerPage::LOCAL_FILE);
	connect(this, SIGNAL(playAudioFiles(QList<QString>, unsigned)),
		audioplayer, SLOT(playAudioFiles(QList<QString>, unsigned)));

#ifdef PDF_EXAMPLE
	pdfdisplay = new PdfPage;
	connect(this, SIGNAL(displayPdf(QString)),
		pdfdisplay, SLOT(displayPdf(QString)));
	connect(pdfdisplay, SIGNAL(Closed()), SLOT(showPageNoReload()));
#endif
}

bool MultimediaFileListPage::browseFiles(const QDir &directory, QList<QFileInfo> &files)
{
	// Create fileslist from files
	QList<QFileInfo> temp_files_list = directory.entryInfoList(file_filters);

	if (temp_files_list.empty())
	{
		qDebug() << "[AUDIO] empty directory: " << directory.absolutePath();
		return false;
	}

	files.clear();

	QList<ItemList::ItemInfo> names_list;

	for (int i = 0; i < temp_files_list.size(); ++i)
	{
		const QFileInfo& f = temp_files_list.at(i);
		if (f.fileName() == "." || f.fileName() == "..")
			continue;

		QStringList icons;

		if (f.isFile())
		{
			MultimediaFileType t = fileType(f);
			if (t == UNKNOWN)
				continue;

			icons << file_icons[t];
			icons << play_file;

			ItemList::ItemInfo info(f.fileName(), QString(), icons);

			names_list.append(info);
			files.append(f);
		}
		else if (f.isDir())
		{
			icons << file_icons[DIRECTORY];
			icons << browse_directory;

			ItemList::ItemInfo info(f.fileName(), QString(), icons);

			names_list.append(info);
			files.append(f);
		}
	}

	page_content->setList(names_list, displayedPage(directory));
	page_content->showList();

	return true;
}

MultimediaFileType MultimediaFileListPage::fileType(const QFileInfo &file)
{
	QString ext = file.suffix().toLower();

	foreach (const QString &extension, getFileExtensions(IMAGE))
		if (ext == extension)
			return IMAGE;

	foreach (const QString &extension, getFileExtensions(VIDEO))
		if (ext == extension)
			return VIDEO;

	foreach (const QString &extension, getFileExtensions(AUDIO))
		if (ext == extension)
			return AUDIO;

#ifdef PDF_EXAMPLE
	foreach (const QString &extension, getFileExtensions(PDF))
		if (ext == extension)
			return PDF;
#endif

	return UNKNOWN;
}

QList<QString> MultimediaFileListPage::filterFileList(int item, MultimediaFileType &type, int &current)
{
	const QList<QFileInfo> &files_list = getFiles();
	const QFileInfo &current_file = files_list[item];
	QList<QString> files;

	type = fileType(current_file);
	for (int i = 0; i < files_list.size(); ++i)
	{
		const QFileInfo& fn = files_list[i];
		if (fn.isDir() || fileType(fn) != type)
			continue;
		if (fn == current_file)
			current = files.size();

		files.append(fn.absoluteFilePath());
	}

	return files;
}

void MultimediaFileListPage::startPlayback(int item)
{
	MultimediaFileType type;
	int current;
	QList<QString> files = filterFileList(item, type, current);

	if (type == IMAGE)
		emit displayImages(files, current);
	else if (type == VIDEO)
		emit displayVideos(files, current);
	else if (type == AUDIO)
		emit playAudioFiles(files, current);
	else if (type == PDF)
		emit displayPdf(files[current]);
}

int MultimediaFileListPage::currentPage()
{
	return page_content->getCurrentPage();
}

void MultimediaFileListPage::nextItem()
{
	page_content->nextItem();
}

void MultimediaFileListPage::prevItem()
{
	page_content->prevItem();
}

void MultimediaFileListPage::unmounted(const QString &dir)
{
	if (dir == getRootPath() && isVisible())
		emit Closed();
	setRootPath("");
}

void MultimediaFileListPage::unmount()
{
	MountWatcher::getWatcher().unmount(getRootPath());
}

void MultimediaFileListPage::cleanUp()
{
	page_content->clear();
}

void MultimediaFileListPage::showPage()
{
	if (getRootPath().isEmpty())
		emit Closed();
	// do not reload the file list unless it is empty
	else if (page_content->itemCount() == 0)
		FileSelector::showPage();
	else
		Selector::showPage();
}

// TODO remove showPageNoReload, and only try to reload the file list if it is empty
void MultimediaFileListPage::showPageNoReload()
{
	if (getRootPath().isEmpty())
		emit Closed();
	else
		Selector::showPage();
}
