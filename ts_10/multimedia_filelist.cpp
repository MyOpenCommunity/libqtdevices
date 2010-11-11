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


MultimediaFileListPage::MultimediaFileListPage(TreeBrowser *browser, int filters, bool mount_enabled) :
	FileSelector(browser)
{
	browser->setFilter(filters);
	connect(browser, SIGNAL(listReceived(EntryInfoList)), SLOT(displayFiles(EntryInfoList)));

	ItemList *item_list = new ItemList(0, 4);
	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(this, SIGNAL(fileClicked(int)), SLOT(startPlayback(int)));

	connect(this, SIGNAL(Closed()), item_list, SLOT(clear()));

	NavigationBar *nav_bar;
	if (mount_enabled)
	{
		nav_bar = new NavigationBar("eject");
		connect(nav_bar, SIGNAL(forwardClick()), SLOT(unmount()));
	}
	else
		nav_bar = new NavigationBar;

	buildPage(item_list, item_list, nav_bar, 0,
		  new PageTitleWidget(tr("Folder"), SMALL_TITLE_HEIGHT));
	layout()->setContentsMargins(0, 5, 25, 10);

	disconnect(nav_bar, SIGNAL(backClick()), 0, 0); // connected by buildPage()
	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));

	// order here must match the order in enum Type
	file_icons.insert(EntryInfo::DIRECTORY, bt_global::skin->getImage("directory_icon"));
	file_icons.insert(EntryInfo::AUDIO, bt_global::skin->getImage("audio_icon"));
	file_icons.insert(EntryInfo::VIDEO, bt_global::skin->getImage("video_icon"));
	file_icons.insert(EntryInfo::IMAGE, bt_global::skin->getImage("image_icon"));
#ifdef PDF_EXAMPLE
	file_icons.insert(EntryInfo::PDF, bt_global::skin->getImage("pdf_icon"));
#endif

	play_file = bt_global::skin->getImage("play_file");
	browse_directory = bt_global::skin->getImage("browse_directory");

	slideshow = new SlideshowPage;
	connect(slideshow, SIGNAL(Closed()), SLOT(showPage()));

	videoplayer = new VideoPlayerPage;

	audioplayer = AudioPlayerPage::getAudioPlayerPage(AudioPlayerPage::LOCAL_FILE);

#ifdef PDF_EXAMPLE
	pdfdisplay = new PdfPage;
	connect(pdfdisplay, SIGNAL(Closed()), SLOT(showPageNoReload()));
#endif
	last_clicked_type = EntryInfo::UNKNOWN;
}

void MultimediaFileListPage::displayFiles(const EntryInfoList &list)
{
	setFiles(list);

	if (list.empty())
	{
		if (browser->isRoot()) // Special case empty root directory
		{
			operationCompleted();
			emit Closed();
		}
		qDebug() << "[AUDIO] empty directory";
		browser->exitDirectory();
		return;
	}

	QList<ItemList::ItemInfo> names_list;

	for (int i = 0; i < list.size(); ++i)
	{
		const EntryInfo& f = list.at(i);

		QStringList icons;

		if (f.type != EntryInfo::DIRECTORY)
		{
			EntryInfo::Type t = f.type;
			if (t == EntryInfo::UNKNOWN)
				continue;

			icons << file_icons[t];
			icons << play_file;

			ItemList::ItemInfo info(f.name, QString(), icons);

			names_list.append(info);
		}
		else
		{
			icons << file_icons[EntryInfo::DIRECTORY];
			icons << browse_directory;

			ItemList::ItemInfo info(f.name, QString(), icons);

			names_list.append(info);
		}
	}

	page_content->setList(names_list, displayedPage(browser->pathKey()));
	page_content->showList();

	operationCompleted();
}

void MultimediaFileListPage::startPlayback(int item)
{
	const EntryInfoList &files_list = getFiles();
	const EntryInfo &current_file = files_list[item];

	// For now we mantain both methods of passing data to the players,
	// in the future we probabily use the EntryInfoList only.
	QList<QString> urls;
	EntryInfoList filtered;

	for (int i = 0; i < files_list.size(); ++i)
	{
		const EntryInfo& fn = files_list[i];
		if ((fn.type == EntryInfo::DIRECTORY || fn.type != last_clicked_type) && last_clicked_type != EntryInfo::UNKNOWN)
			continue;
		if (fn == current_file)
		{
			last_clicked = urls.size();
			last_clicked_type = fn.type;
		}
		filtered.append(fn);
		urls.append(fn.url);
	}

	if (last_clicked_type == EntryInfo::IMAGE)
		slideshow->displayImages(urls, last_clicked);
	else if (last_clicked_type == EntryInfo::VIDEO)
		videoplayer->displayVideos(urls, last_clicked);
	else if (last_clicked_type == EntryInfo::AUDIO)
		audioplayer->playAudioFiles(filtered, last_clicked);
#ifdef PDF_EXAMPLE
	else if (type == EntryInfo::PDF)
		pdfdisplay->displayPdf(files[current]);
#endif
}

void MultimediaFileListPage::cleanUp()
{
	FileSelector::cleanUp();

	page_content->clear();
	setFiles(EntryInfoList());
}
