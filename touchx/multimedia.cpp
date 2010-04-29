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

#include <QtDebug>


enum
{
	PAGE_USB = 16002,
	PAGE_WEB_RADIO = 16001,
	PAGE_WEB_CAM = 16004,
	PAGE_RSS = 16003,
	PAGE_SD = 16005,
};


FileSystemBrowseButton::FileSystemBrowseButton(MountWatcher &watch, FileSelector *_browser,
					       MountType _type, const QString &label,
					       const QString &icon_mounted, const QString &icon_unmounted) :
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
	browser->browse(directory);
}

MultimediaSectionPage::MultimediaSectionPage(const QDomNode &config_node, MultimediaSectionPage::Items items, FileSelector *selector) :
	browser(0), delete_browser(true)
{
	Q_ASSERT_X(items.testFlag(MultimediaSectionPage::ITEMS_FILESYSTEM) && selector != 0,
				"MultimediaSectionPage::MultimediaSectionPage",
				"ITEMS_FILESYSTEM == true && browser == 0");

	SkinContext cxt(getTextChild(config_node, "cid").toInt());

	showed_items = items;
	browser = selector;

	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	loadItems(config_node);
	if (delete_browser)
		browser->deleteLater();
}

int MultimediaSectionPage::sectionId() const
{
	return MULTIMEDIA;
}

void MultimediaSectionPage::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QString icon = bt_global::skin->getImage("link_icon");
		QString descr = getTextChild(item, "descr");

		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		int page_id = getTextChild(page_node, "id").toInt();
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
				QWidget *t = new FileSystemBrowseButton(MountWatcher::getWatcher(), browser,
									item_id == PAGE_USB ? MOUNT_USB : MOUNT_SD, descr,
									bt_global::skin->getImage("mounted"),
									bt_global::skin->getImage("unmounted"));
				page_content->addWidget(t);
				connect(browser, SIGNAL(Closed()), this, SLOT(showPage()));
				delete_browser = false;
			}
			break;
		}
		case PAGE_WEB_CAM:
			if (showed_items.testFlag(MultimediaSectionPage::ITEMS_WEBCAM))
			{
				p = new WebcamListPage(page_node);
			}
			break;
		case PAGE_WEB_RADIO:
			if (showed_items.testFlag(MultimediaSectionPage::ITEMS_WEBRADIO))
			{
				p = new IPRadioPage(page_node);
			}
			break;
		case PAGE_RSS:
			if (showed_items.testFlag(MultimediaSectionPage::ITEMS_RSS))
			{
				p = new FeedManager(page_node);
			}
			break;
		default:
			;// qFatal("Unhandled page id in SettingsTouchX::loadItems");
		};

		if (p)
		{
			p->inizializza();
			addPage(p, descr, icon);
		}
	}

	MountWatcher::getWatcher().startWatching();
}
