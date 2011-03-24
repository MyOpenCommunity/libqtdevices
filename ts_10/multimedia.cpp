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


#include "multimedia.h"
#include "multimedia_filelist.h"
#include "navigation_bar.h"
#include "xml_functions.h"
#include "main.h" // MULTIMEDIA
#include "skinmanager.h"
#include "webcam.h"
#include "ipradio.h"
#include "feedmanager.h"
#include "state_button.h"
#include "generic_functions.h" // getFileFilter
#include "audioplayer.h"
#include "hardware_functions.h"

#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QtDebug>


enum
{
	PAGE_USB = 16002,
	PAGE_WEB_RADIO = 16001,
	PAGE_WEB_CAM = 16004,
	PAGE_RSS = 16003,
	PAGE_SD = 16005,
	PAGE_UPNP = 16006,
#ifdef BUILD_EXAMPLES
	PAGE_PDF = 55556,
#endif
};

SongSearch *MultimediaSectionPage::song_search = NULL;
AudioPlayerPage *MultimediaSectionPage::current_player = 0;
bool MultimediaSectionPage::usb_initialized = false;


FileSystemBrowseButton::FileSystemBrowseButton(MountWatcher &watch, FileSelector *_browser, MountType _type,
	const QString &label, const QString &icon_mounted, const QString &icon_unmounted) :
	IconPageButton(label)
{
	type = _type;
	browser = _browser;

	button->setOnImage(icon_mounted);
	button->setDisabledImage(icon_unmounted);

	connect(button, SIGNAL(clicked()), SLOT(browse()));

	connect(&watch, SIGNAL(directoryMounted(const QString &, MountType)), SLOT(mounted(const QString &,MountType)));
	connect(&watch, SIGNAL(directoryUnmounted(const QString &, MountType)), SLOT(unmounted(const QString &, MountType)));

	button->setStatus(StateButton::DISABLED);
}

// TODO handle multiple mount points
void FileSystemBrowseButton::mounted(const QString &path, MountType t)
{
	if (t != type)
		return;
	qDebug() << "Mounted" << path << type;

	button->setStatus(StateButton::ON);
	directory = path;
	browser->setRootPath(directory);
}

void FileSystemBrowseButton::unmounted(const QString &path, MountType t)
{
	if (t != type)
		return;
	qDebug() << "Unmounted" << path << type;

	button->setStatus(StateButton::DISABLED);
}

void FileSystemBrowseButton::browse()
{
	browser->showPage();
}



MultimediaSectionPage::MultimediaSectionPage(const QDomNode &config_node, MultimediaSectionPage::Items items, FileSelectorFactory *f, const QString &title)
{
	Q_ASSERT_X(items.testFlag(MultimediaSectionPage::ITEMS_FILESYSTEM),
		"MultimediaSectionPage::MultimediaSectionPage", "ITEMS_FILESYSTEM == true");

	if (!usb_initialized)
	{
		usbHotplug();
		usb_initialized = true;
	}
	SkinContext cxt(getTextChild(config_node, "cid").toInt());

	factory = f;
	showed_items = items;

	QString descr;
	if (title.isEmpty())
		descr = getTextChild(config_node, "descr");
	else
		descr = title;

	NavigationBar *nav_bar = new NavigationBar("play_file");
	buildPage(new IconContent, nav_bar, descr);
	loadItems(config_node);

	play_button = nav_bar->forward_button;
	connect(play_button, SIGNAL(clicked()), SLOT(gotoPlayerPage()));
	play_button->hide();
}

MultimediaSectionPage::~MultimediaSectionPage()
{
	delete factory;
}

void MultimediaSectionPage::showEvent(QShowEvent *)
{
	play_button->setVisible(current_player != 0);
	if (current_player)
		connect(current_player, SIGNAL(playerExited()), this, SLOT(currentPlayerExited()));
}

void MultimediaSectionPage::hideEvent(QHideEvent *)
{
	if (current_player)
		disconnect(current_player, SIGNAL(playerExited()), this, SLOT(currentPlayerExited()));
}

void MultimediaSectionPage::currentPlayerExited()
{
	play_button->setVisible(false);
	disconnect(current_player, SIGNAL(playerExited()), this, SLOT(currentPlayerExited()));
}

void MultimediaSectionPage::gotoPlayerPage()
{
	if (current_player)
		current_player->showPage();
}

int MultimediaSectionPage::sectionId() const
{
	return MULTIMEDIA;
}

void MultimediaSectionPage::loadItems(const QDomNode &config_node)
{
	// for SongSearch
	QList<int> sources;
	FileSystemBrowseButton *usb_button = NULL, *sd_button = NULL;
	IPRadioPage *ip_radio = NULL;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QString icon = bt_global::skin->getImage("link_icon");
		QString descr = getTextChild(item, "descr");

		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		int item_id = getTextChild(item, "id").toInt();

		Page *p = 0;

		// use the item_id for now because some of the items do not
		// have a linked page
		switch (item_id)
		{
		case PAGE_USB:
		case PAGE_SD:
		{
			if (showed_items.testFlag(MultimediaSectionPage::ITEMS_FILESYSTEM))
			{
				FileSelector *selector = factory->getFileSelector();
				FileSystemBrowseButton *t = new FileSystemBrowseButton(*bt_global::mount_watcher, selector,
					item_id == PAGE_USB ? MOUNT_USB : MOUNT_SD, descr, bt_global::skin->getImage("mounted"),
					bt_global::skin->getImage("unmounted"));
				page_content->addWidget(t);
				connect(selector, SIGNAL(Closed()), this, SLOT(showPage()));

				sources.append(item_id);

				if (item_id == PAGE_USB)
					usb_button = t;
				else
					sd_button = t;
			}
			break;
		}
		case PAGE_UPNP:
			if (!bt_global::xml_device)
				bt_global::xml_device = new XmlDevice;

			if (showed_items.testFlag(MultimediaSectionPage::ITEMS_UPNP))
			{
				MultimediaFileListFactory f(TreeBrowser::UPNP, EntryInfo::DIRECTORY | EntryInfo::AUDIO, false);
				p = f.getFileSelector();
			}
			break;
#ifdef BUILD_EXAMPLES
		case PAGE_PDF:
		{
			if (showed_items.testFlag(MultimediaSectionPage::ITEMS_FILESYSTEM))
			{
				MultimediaFileListFactory f(TreeBrowser::DIRECTORY, EntryInfo::DIRECTORY | EntryInfo::PDF, false);
				FileSelector *selector = f.getFileSelector();
				FileSystemBrowseButton *t = new FileSystemBrowseButton(MountWatcher::getWatcher(), selector,
										       MOUNT_USB, descr,
										       bt_global::skin->getImage("mounted"),
										       bt_global::skin->getImage("unmounted"));
				page_content->addWidget(t);
				connect(browser, SIGNAL(Closed()), this, SLOT(showPage()));
			}
			break;
		}
#endif
		case PAGE_WEB_CAM:
			if (showed_items.testFlag(MultimediaSectionPage::ITEMS_WEBCAM))
				p = new WebcamListPage(page_node);
			break;
		case PAGE_WEB_RADIO:
			if (showed_items.testFlag(MultimediaSectionPage::ITEMS_WEBRADIO))
				p = ip_radio = new IPRadioPage(page_node);
			sources.append(item_id);
			break;
		case PAGE_RSS:
			if (showed_items.testFlag(MultimediaSectionPage::ITEMS_RSS))
				p = new FeedManager(page_node);
			break;
		default:
			qFatal("Unhandled page id in MultimediaSectionPage::loadItems");
		};

		if (p)
		{
			p->inizializza();
			addPage(p, descr, icon);
		}
	}

	bt_global::mount_watcher->startWatching();
	bt_global::mount_watcher->notifyAll();

	if (showed_items == ITEMS_ALL && !song_search)
		song_search = new SongSearch(sources, usb_button, sd_button, ip_radio);
}

void MultimediaSectionPage::playSomethingRandomly()
{
	song_search->startSearch();
}


SongSearch::SongSearch(QList<int> _sources, FileSystemBrowseButton *_usb, FileSystemBrowseButton *_sd, IPRadioPage *_radio)
{
	sources = _sources;
	sd_button = _sd;
	usb_button = _usb;
	ip_radio = _radio;
	current_source = -1;
	terminate = NULL;
}

void SongSearch::startSearch()
{
	terminateSearch();
	nextSource();
}

void SongSearch::terminateSearch()
{
	if (current_source == -1)
		return;

	switch (sources[current_source])
	{
	case PAGE_USB:
	case PAGE_SD:
		qDebug() << "Terminating USB/SD search";
		*terminate = true;
		break;
	case PAGE_WEB_RADIO:
		Q_ASSERT_X(false, "SongSearch::terminateSearch", "PANIC: tried to terminate a web radio search");
		break;
	}

	current_source = -1;
}

void SongSearch::nextSource()
{
	current_source += 1;

	if (current_source >= sources.size())
	{
		qDebug() << "Uh, oh! Ran out of sources!";
		current_source = -1;

		return;
	}

	switch (sources[current_source])
	{
	case PAGE_USB:
	case PAGE_SD:
	{
		FileSystemBrowseButton *b = sources[current_source] == PAGE_USB ? usb_button : sd_button;
		QString root = b->currentPath();

		if (root.isEmpty())
		{
			nextSource();

			return;
		}

		// run the search asynchronously
		terminate = new bool(false);
		QFuture<AsyncRes> res = QtConcurrent::run(&scanPath, root, terminate);
		QFutureWatcher<AsyncRes> *watch = new QFutureWatcher<AsyncRes>(this);

		connect(watch, SIGNAL(finished()), SLOT(pathScanComplete()));

		watch->setFuture(res);

		break;
	}
	case PAGE_WEB_RADIO:
		QStringList urls = ip_radio->radioUrls();

		qDebug() << "Playing from Web radio";
		playAudioFiles(urls, AudioPlayerPage::IP_RADIO);
		break;
	}
}

void SongSearch::pathScanComplete()
{
	qDebug() << "USB/SD search complete";

	QFutureWatcher<AsyncRes> *watch = static_cast<QFutureWatcher<AsyncRes> *>(sender());
	QStringList files = watch->result().first;
	bool terminated = *watch->result().second;

	if (!terminated && !files.isEmpty())
	{
		qDebug() << "Playing from USB/SD";
		playAudioFiles(files, AudioPlayerPage::LOCAL_FILE);
	}
	else if (!terminated)
		nextSource();

	delete watch->result().second;
	watch->deleteLater();
}

void SongSearch::playAudioFiles(QStringList things, int type)
{
	AudioPlayerPage *page = AudioPlayerPage::getAudioPlayerPage(static_cast<AudioPlayerPage::MediaType>(type));

	current_source = -1;
	page->playAudioFilesBackground(things, 0);
}

namespace
{
	template<class R>
	R makeAbsolute(QFileInfoList files)
	{
		R result;

		foreach (const QFileInfo &fi, files)
			result.append(fi.absoluteFilePath());

		return result;
	}
}

SongSearch::AsyncRes SongSearch::scanPath(const QString &path, bool * volatile terminate)
{
	QList<QFileInfo> queue;
	QDir files, dirs;

	dirs.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
	files.setFilter(QDir::Files);
	files.setNameFilters(getFileFilter(EntryInfo::AUDIO));

	queue << path;

	while (!queue.isEmpty() && !*terminate)
	{
		QFileInfo dir = queue.takeFirst();

		// search for files
		qDebug() << "Scanning" << dir.absoluteFilePath();
		files.cd(dir.absoluteFilePath());

		// found some files
		QFileInfoList file_list = files.entryInfoList();
		if (!file_list.isEmpty())
			return qMakePair(makeAbsolute<QStringList>(file_list), terminate);

		// recurse into subdirectories
		dirs.cd(dir.absoluteFilePath());
		queue.append(makeAbsolute<QFileInfoList>(dirs.entryInfoList()));
	}

	return qMakePair(QStringList(), terminate);
}
