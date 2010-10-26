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

MultimediaFileListPage::MultimediaFileListPage(const QStringList &filters) :
	FileSelector(new DirectoryTreeBrowser),
	file_filters(filters)
{
	connect(browser, SIGNAL(listReceived(QList<TreeBrowser::EntryInfo>)), SLOT(displayFiles(QList<TreeBrowser::EntryInfo>)));
	connect(browser, SIGNAL(allUrlsReceived(QStringList)), SLOT(urlListReceived(QStringList)));

	ItemList *item_list = new ItemList(0, 4);
	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(this, SIGNAL(fileClicked(int)), SLOT(startPlayback(int)));

	connect(this, SIGNAL(Closed()), item_list, SLOT(clear()));

	NavigationBar *nav_bar = new NavigationBar("eject");

	buildPage(item_list, item_list, nav_bar, 0,
		  new PageTitleWidget(tr("Folder"), SMALL_TITLE_HEIGHT));
	layout()->setContentsMargins(0, 5, 25, 10);

	disconnect(nav_bar, SIGNAL(backClick()), 0, 0); // connected by buildPage()

	connect(this, SIGNAL(forwardClick()), SLOT(unmount()));
	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));

	// order here must match the order in enum Type
	file_icons.append(bt_global::skin->getImage("directory_icon"));
	file_icons.append(bt_global::skin->getImage("audio_icon"));
	file_icons.append(bt_global::skin->getImage("video_icon"));
	file_icons.append(bt_global::skin->getImage("image_icon"));

	play_file = bt_global::skin->getImage("play_file");
	browse_directory = bt_global::skin->getImage("browse_directory");

	slideshow = new SlideshowPage;
	connect(this, SIGNAL(displayImages(QList<QString>, unsigned)),
		slideshow, SLOT(displayImages(QList<QString>, unsigned)));
	connect(slideshow, SIGNAL(Closed()), SLOT(showPage()));

	videoplayer = new VideoPlayerPage;
	connect(this, SIGNAL(displayVideos(QList<QString>, unsigned)),
		videoplayer, SLOT(displayVideos(QList<QString>, unsigned)));

	audioplayer = AudioPlayerPage::getAudioPlayerPage(AudioPlayerPage::LOCAL_FILE);
	connect(this, SIGNAL(playAudioFiles(QList<QString>, unsigned)),
		audioplayer, SLOT(playAudioFiles(QList<QString>, unsigned)));
}

void MultimediaFileListPage::displayFiles(const QList<TreeBrowser::EntryInfo> &list)
{
	QList<TreeBrowser::EntryInfo> filtered = TreeBrowser::filterEntries(list, file_filters);

	// TODO better interface, maybe move clicked handling in subclass
	setFiles(filtered);

	if (filtered.empty())
	{
		qDebug() << "[AUDIO] empty directory";
		browser->exitDirectory();
		return;
	}

	QList<ItemList::ItemInfo> names_list;

	for (int i = 0; i < filtered.size(); ++i)
	{
		const TreeBrowser::EntryInfo& f = filtered.at(i);

		QStringList icons;

		if (!f.is_directory)
		{
			MultimediaFileType t = fileType(f.name);
			if (t == UNKNOWN)
				continue;

			icons << file_icons[t];
			icons << play_file;

			ItemList::ItemInfo info(f.name, QString(), icons);

			names_list.append(info);
		}
		else
		{
			icons << file_icons[DIRECTORY];
			icons << browse_directory;

			ItemList::ItemInfo info(f.name, QString(), icons);

			names_list.append(info);
		}
	}

	page_content->setList(names_list, displayedPage(browser->pathKey()));
	page_content->showList();

	operationCompleted();
}

MultimediaFileType MultimediaFileListPage::fileType(const QString &file)
{
	QString ext = QFileInfo(file).suffix().toLower();

	foreach (const QString &extension, getFileExtensions(IMAGE))
		if (ext == extension)
			return IMAGE;

	foreach (const QString &extension, getFileExtensions(VIDEO))
		if (ext == extension)
			return VIDEO;

	foreach (const QString &extension, getFileExtensions(AUDIO))
		if (ext == extension)
			return AUDIO;

	return UNKNOWN;
}

QList<QString> MultimediaFileListPage::filterFileList(int item, MultimediaFileType &type, int &current)
{
	const QList<TreeBrowser::EntryInfo> &files_list = getFiles();
	const TreeBrowser::EntryInfo &current_file = files_list[item];
	QList<QString> files;

	type = fileType(current_file.name);
	for (int i = 0; i < files_list.size(); ++i)
	{
		const TreeBrowser::EntryInfo& fn = files_list[i];
		if (fn.is_directory || fileType(fn.name) != type)
			continue;
		if (fn == current_file)
			current = files.size();

		files.append(fn.name);
	}

	return files;
}

void MultimediaFileListPage::startPlayback(int item)
{
	startOperation();

	QList<QString> files = filterFileList(item, last_clicked_type, last_clicked);

	browser->getAllFileUrls(files);
}

void MultimediaFileListPage::urlListReceived(const QStringList &files)
{
	operationCompleted();

	if (last_clicked_type == IMAGE)
		emit displayImages(files, last_clicked);
	else if (last_clicked_type == VIDEO)
		emit displayVideos(files, last_clicked);
	else if (last_clicked_type == AUDIO)
		emit playAudioFiles(files, last_clicked);
}

void MultimediaFileListPage::cleanUp()
{
	page_content->clear();
	setFiles(QList<TreeBrowser::EntryInfo>());
}
