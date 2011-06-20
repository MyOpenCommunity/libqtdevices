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


#include "multimedia_ts3.h"
#include "xml_functions.h"
#include "bann2_buttons.h"
#include "skinmanager.h"
#include "ipradio.h"
#include "main.h" // getPageNodeFromChildNode
#include "multimedia_filelist.h"
#include "xmldevice.h" // bt_global::xml_device

#include <QDomNode>
#include <QString>

enum
{
	PAGE_WEB_RADIO = 16001,
	PAGE_UPNP = 16006
};



MultimediaContainer::MultimediaContainer(const QDomNode &config_node)
{
	buildPage();
	setSpacing(10);
	radio_page = 0;
	upnp_page = 0;
	loadItems(config_node);
}

void MultimediaContainer::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext context(getTextChild(item, "cid").toInt());

		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		int item_id = getTextChild(item, "id").toInt();
		Page *p = 0;
		QString descr;

		switch (item_id)
		{
		case PAGE_WEB_RADIO:
			radio_page = p = new IPRadioPage(page_node);
			descr = tr("IP Radio");
			break;
		case PAGE_UPNP:
			if (!bt_global::xml_device)
				bt_global::xml_device = new XmlDevice;
			MultimediaFileListFactory f(TreeBrowser::UPNP, EntryInfo::DIRECTORY | EntryInfo::AUDIO, false);
			upnp_page = p = f.getFileSelector();
			descr = tr("Servers");
			break;
		}

		if (p)
		{
			Bann2Buttons *b = new Bann2Buttons;
			b->initBanner(QString(), bt_global::skin->getImage("forward"), descr);
			b->connectRightButton(p);
			connect(b, SIGNAL(pageClosed()), SLOT(handleClose()));
			page_content->appendBanner(b);
		}
	}
}

void MultimediaContainer::showPage()
{
	if (radio_page && upnp_page)
		BannerPage::showPage();
	else if (radio_page)
		radio_page->showPage();
	else
		upnp_page->showPage();
}

void MultimediaContainer::handleClose()
{
	if (radio_page && upnp_page)
		showPage();
	else
		emit Closed();
}


