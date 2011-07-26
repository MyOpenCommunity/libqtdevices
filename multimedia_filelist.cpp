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
#include "btbutton.h"
#include "sounddiffusionpage.h" // SoundDiffusionPage::showCurrentAmbientPage

#ifdef LAYOUT_TS_10
#include "slideshow.h"
#include "mount_watcher.h"
#include "videoplayer.h"
#include "audioplayer_ts10.h"
#else
#include "audioplayer_ts3.h"
#endif

#ifdef PDF_EXAMPLE
#include "examples/pdf/pages/pdfpage.h"
#endif

#include <QButtonGroup>
#include <QLayout>
#include <QDebug>


MultimediaList::MultimediaList(int rows_per_page, QWidget *parent) : ItemList(rows_per_page, parent)
{

}

void MultimediaList::addHorizontalBox(QGridLayout *layout, const ItemInfo &item, int id_btn)
{
	// In the current code when we click an audio item the reproduction starts
	// immediately, and then the audio state machine change its state.
	// If the beep is on the sound is performed on the mousePressEvents, that
	// is _before_ the click and if the sound ends after the reproduction is
	// started, the direct_audio_access of the audio state machine is set
	// wrongly as false. So, when another event (like a videocall) trigger a
	// sound, the audio state machine change immediately its state, and the sound
	// is not played.
	// The quickly way to resolve this problem is to disable the beep for audio files.

	ItemList::addHorizontalBox(layout, item, id_btn);
	if (item.data.toInt() == EntryInfo::AUDIO)
	{
		BtButton *btn = static_cast<BtButton*>(buttons_group->button(id_btn));
		btn->enableBeep(false);
	}
}


MultimediaFileListPage::MultimediaFileListPage(TreeBrowser *browser, int filters, bool mount_enabled) :
	FileSelector(browser)
{
	browser->setFilter(filters);

	// See the comment in MultimediaFileListPage::pageUp() method
	qRegisterMetaType<EntryInfoList>("EntryInfoList");
	connect(browser, SIGNAL(listReceived(EntryInfoList)), this, SLOT(displayFiles(EntryInfoList)), Qt::QueuedConnection);

	rows_per_page = 4;
	ItemList *item_list = new MultimediaList(rows_per_page);
	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
	connect(this, SIGNAL(fileClicked(int)), SLOT(startPlayback(int)));

	connect(this, SIGNAL(Closed()), item_list, SLOT(clear()));

	nav_bar = 0;

#ifdef BT_HARDWARE_PXA270
	if (mount_enabled)
	{
		nav_bar = new NavigationBar("eject");
		connect(nav_bar, SIGNAL(forwardClick()), SLOT(unmount()));
	}
#endif

#ifdef LAYOUT_TS_3_5
	nav_bar = new NavigationBar("goto_sounddiffusion");
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(gotoSoundDiffusion()));
#endif

	if (!nav_bar)
		nav_bar = new NavigationBar;

#ifdef LAYOUT_TS_10
	title_widget = new PageTitleWidget(tr("Folder"), SMALL_TITLE_HEIGHT);
#else
	title_widget = 0;
#endif

	if (qobject_cast<UPnpClientBrowser*>(browser))
	{
		Page::buildPage(item_list, item_list, nav_bar, title_widget);
		audioplayer = AudioPlayerPage::getAudioPlayerPage(AudioPlayerPage::UPNP_FILE);
	}
	else
	{
#ifdef LAYOUT_TS_10
		buildPage(item_list, item_list, nav_bar, title_widget);
		disconnect(nav_bar, SIGNAL(backClick()), 0, 0); // connected by buildPage()

		audioplayer = AudioPlayerPage::getAudioPlayerPage(AudioPlayerPage::LOCAL_FILE);
#else
		Q_ASSERT_X(false, "MultimediaFileListPage::MultimediaFileListPage",
			"Cannot create instances to navigate over local files on ts3!");
#endif
	}

	connect(nav_bar, SIGNAL(backClick()), SLOT(browseUp()));
	connect(nav_bar, SIGNAL(upClick()), SLOT(pageUp()));
	connect(nav_bar, SIGNAL(downClick()), SLOT(pageDown()));
#ifdef LAYOUT_TS_10
	layout()->setContentsMargins(13, 5, 25, 10);

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
	connect(slideshow, SIGNAL(cleanedUp()), SLOT(cleanUp()));

	videoplayer = new VideoPlayerPage;
#endif

#ifdef PDF_EXAMPLE
	pdfdisplay = new PdfPage;
	connect(pdfdisplay, SIGNAL(Closed()), SLOT(showPageNoReload()));
#endif
	last_clicked_type = EntryInfo::UNKNOWN;
}

void MultimediaFileListPage::gotoSoundDiffusion()
{
	SoundDiffusionPage::showCurrentAmbientPage();
}

void MultimediaFileListPage::audioPageClosed()
{
	if (audioplayer->isPlayerInstanceRunning())
	{
		// We don't want that FileSelector::showPage requests the list of
		// the files if the files_list is empty (after a call to the cleanUp method)
		ScrollablePage::showPage();
		startOperation();

		navigation_context = playing_navigation_context;
		pages_indexes = playing_pages_indexes;
		// the directoryChanged signal trigger a call to the displayFiles
		browser->setContext(navigation_context);
	}
	else
	{
		disconnect(audioplayer, SIGNAL(Closed()), this, SLOT(audioPageClosed()));
		showPage();
	}
}

void MultimediaFileListPage::loopDetected()
{
	disconnect(audioplayer, SIGNAL(Closed()), this, SLOT(loopDetected()));
	if (UPnpClientBrowser *b = qobject_cast<UPnpClientBrowser*>(browser))
	{
		operationCompleted();
		b->reset();
		emit Closed();
		return;
	}
	showPage();
}

void MultimediaFileListPage::itemIsClicked(int item)
{
	const EntryInfo &current_file = getFiles()[item];
	if (current_file.type == EntryInfo::DIRECTORY) // we add only albums or directories
		navigation_context << current_file.name;

	FileSelector::itemIsClicked(item);
}

void MultimediaFileListPage::emptyDirectory()
{
	navigation_context.removeLast();
	FileSelector::emptyDirectory();
}

void MultimediaFileListPage::directoryChangeError()
{
	navigation_context.removeLast();
	FileSelector::directoryChangeError();
}

void MultimediaFileListPage::handleError()
{
	navigation_context.clear();
	FileSelector::handleError();
}

void MultimediaFileListPage:: browseUp()
{
	FileSelector::browseUp();

	if (!navigation_context.isEmpty())
		navigation_context.removeLast();
}

void MultimediaFileListPage::pageUp()
{
	// The getPreviousFileList() method updates the starting element, used
	// in currentPage() and called by the FileSelector::pageUp() method to
	// update the displayedPage().
	// Due to that the displayFiles() method must be called after call both
	// methods, so we need an asynchronous connection to manage properly
	// the case when the response is immediately (for example when we ask
	// the list of the upnp servers).

	if (UPnpClientBrowser *b = qobject_cast<UPnpClientBrowser*>(browser))
		b->getPreviousFileList();
	FileSelector::pageUp();
}

void MultimediaFileListPage::pageDown()
{
	if (UPnpClientBrowser *b = qobject_cast<UPnpClientBrowser*>(browser))
		b->getNextFileList();
	FileSelector::pageDown();
}

int MultimediaFileListPage::currentPage()
{
	if (UPnpClientBrowser *b = qobject_cast<UPnpClientBrowser*>(browser))
		return (b->getStartingElement() - 1) / rows_per_page;

	return FileSelector::currentPage();
}

void MultimediaFileListPage::displayFiles(const EntryInfoList &list)
{
	int page_index = displayedPage(browser->pathKey());

	if (list.empty())
	{
		if (page_index != 0)
		{
			// If we are requested the page n.X but that page does not exists
			// anymore(which can happen especially on upnp directories) the list
			// is empty, so we reset the page number.
			resetDisplayedPage();
			browser->getFileList();
			return;
		}

		if (browser->isRoot()) // Special case empty root directory
		{
			operationCompleted();
			browser->reset();
			emit Closed();
			return;
		}
		qDebug() << "MultimediaFileListPage::displayFiles -> empty directory";
		browser->exitDirectory();
		return;
	}

	static int loop_counter = 0;

	if (UPnpClientBrowser *b = qobject_cast<UPnpClientBrowser*>(browser))
	{
		// When we browse up to a directory, we want to show the directory
		// previously selected. Because the UPnpClientBrowser has an asyncronous
		// api, we have to wait the listReiceved signal before request the right
		// page index.
		if (page_index != 0 && b->getStartingElement() == 1)
		{
			if (loop_counter < 10)
			{
				b->getFileList(page_index * rows_per_page + 1);
				++loop_counter;
				return;
			}
			else
			{
				qWarning() << "MultimediaFileListPage::displayFiles -> Loop detected!";
				resetDisplayedPage();
				b->getFileList();
				return;
			}
		}

		loop_counter = 0;
		page_index = 0;

		nav_bar->displayScrollButtons(b->getNumElements() > rows_per_page);

		int current_page = (b->getStartingElement() - 1) / rows_per_page;
		int total_pages =(b->getNumElements() - 1) / rows_per_page + 1;

		if (title_widget)
			title_widget->setCurrentPage(current_page, total_pages);
	}

	EntryInfoList filtered_list;

	QList<ItemList::ItemInfo> names_list;

	for (int i = 0; i < list.size(); ++i)
	{
		const EntryInfo& f = list.at(i);
		QStringList icons;

#ifdef LAYOUT_TS_10
		if (f.type != EntryInfo::DIRECTORY)
		{
			EntryInfo::Type t = f.type;
			if (t == EntryInfo::UNKNOWN)
				continue;

			icons << file_icons[t];
			icons << play_file;
		}
		else
		{
			icons << file_icons[EntryInfo::DIRECTORY];
			icons << browse_directory;
		}
#else
		icons << bt_global::skin->getImage("forward");
#endif
		ItemList::ItemInfo info(f.name, QString(), icons, static_cast<int>(f.type));
		names_list.append(info);
		filtered_list.append(f);
	}

	setFiles(filtered_list);
	page_content->setList(names_list, page_index);
	page_content->showList();

	operationCompleted();
}

void MultimediaFileListPage::connectAudioPage()
{
	// disconnect & connect to avoid multiple connect
	disconnect(audioplayer, SIGNAL(Closed()), this, SLOT(audioPageClosed()));
	connect(audioplayer, SIGNAL(Closed()), this, SLOT(audioPageClosed()));

	disconnect(audioplayer, SIGNAL(loopDetected()), this, SLOT(loopDetected()));
	connect(audioplayer, SIGNAL(loopDetected()), this, SLOT(loopDetected()));

	disconnect(audioplayer, SIGNAL(serverDown()), this, SLOT(handleServerDown()));
	connect(audioplayer, SIGNAL(serverDown()), this, SLOT(handleServerDown()));
}

void MultimediaFileListPage::handleServerDown()
{
	disconnect(audioplayer, SIGNAL(serverDown()), this, SLOT(handleServerDown()));
	handleError();
}

void MultimediaFileListPage::startPlayback(int item)
{
	playing_navigation_context = navigation_context;
	playing_pages_indexes = pages_indexes;

	const EntryInfoList &files_list = getFiles();
	const EntryInfo &current_file = files_list[item];
	last_clicked_type = current_file.type;

	if (UPnpClientBrowser *b = qobject_cast<UPnpClientBrowser*>(browser))
	{
		// For now, we manage only the audio files using the upnp client.
		audioplayer->playAudioFile(current_file, item + b->getStartingElement() - 1, b->getNumElements());
		// Because the are many instances of MultimediaFileListPage and only
		// one per type of AudioPlayerPage we have to connect the last with
		// the right MultimediaFileListPage instance.

		connectAudioPage();
		return;
	}

	// For now we mantain both methods of passing data to the players,
	// in the future we probabily use the EntryInfoList only.
	QList<QString> urls;
	EntryInfoList filtered;

	for (int i = 0; i < files_list.size(); ++i)
	{
		const EntryInfo& fn = files_list[i];
		// If the last clicked item is not of type unknown we want only the items
		// (not directory) with the same type.
		if ((fn.type == EntryInfo::DIRECTORY || fn.type != last_clicked_type) && last_clicked_type != EntryInfo::UNKNOWN)
			continue;
		if (fn == current_file)
			last_clicked = urls.size();

		filtered.append(fn);
		urls.append(fn.path);
	}

	if (last_clicked_type == EntryInfo::AUDIO)
	{
		audioplayer->playAudioFiles(filtered, last_clicked);
		connectAudioPage();
	}
#ifdef LAYOUT_TS_10
	else if (last_clicked_type == EntryInfo::VIDEO)
		videoplayer->displayVideos(files_list, last_clicked);
	else if (last_clicked_type == EntryInfo::IMAGE)
		slideshow->displayImages(urls, last_clicked);
#endif
#ifdef PDF_EXAMPLE
	else if (type == EntryInfo::PDF)
		pdfdisplay->displayPdf(current_file.path);
#endif
}

void MultimediaFileListPage::cleanUp()
{
	FileSelector::cleanUp();

	page_content->clear();
	setFiles(EntryInfoList());
}


MultimediaFileListPage *MultimediaFileListFactory::upnp_page = 0;

MultimediaFileListFactory::MultimediaFileListFactory(TreeBrowser::Types _type, int _filters, bool _mount_enabled)
{
	filters = _filters;
	type = _type;
	mount_enabled = _mount_enabled;
}

FileSelector* MultimediaFileListFactory::getFileSelector()
{
	if (type == TreeBrowser::DIRECTORY)
		return new MultimediaFileListPage(new DirectoryTreeBrowser, filters, mount_enabled);
	else if (type != TreeBrowser::UPNP)
		Q_ASSERT(qPrintable(QString("MultimediaFileListFactory::getFileSelector -> cannot create browser of unknown type %d").arg(type)));

	// Because bt_controlpoint supports only one client, we use an unique instance of the related MultimediaFileListPage
	// (now shared between the multimedia and the sound diffusion section).
	if (!upnp_page)
		upnp_page = new MultimediaFileListPage(new UPnpClientBrowser(bt_global::xml_device), filters, mount_enabled);

	return upnp_page;
}

