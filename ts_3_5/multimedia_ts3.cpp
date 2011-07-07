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
#include "xml_functions.h" // getChildren
#include "skinmanager.h"
#include "ipradio.h"
#include "main.h" // getPageNodeFromChildNode
#include "multimedia_filelist.h"
#include "xmldevice.h" // bt_global::xml_device
#include "navigation_bar.h"
#include "fontmanager.h"
#include "btbutton.h"
#include "audioplayer_ts3.h"

#include <QDebug>
#include <QDomNode>
#include <QString>
#include <QBoxLayout>

enum
{
	PAGE_WEB_RADIO = 16001,
	PAGE_UPNP = 16006
};

IPRadioPage *MultimediaContainer::radio_page = 0;
FileSelector *MultimediaContainer::upnp_page = 0;


MultimediaContainer::MultimediaContainer(const QDomNode &config_node)
{
	NavigationBar *nav_bar = new NavigationBar("play_file");
	play_button = nav_bar->forward_button;
	connect(play_button, SIGNAL(clicked()), SLOT(gotoPlayerPage()));
	play_button->hide();
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	buildPage(new QWidget, nav_bar);
	radio_page = 0;
	upnp_page = 0;
	loadItems(config_node);
}

void MultimediaContainer::loadItems(const QDomNode &config_node)
{
	QVBoxLayout *main_layout = new QVBoxLayout(page_content);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(10);

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
			p = radio_page = new IPRadioPage(page_node);
			descr = tr("IP Radio");
			break;
		case PAGE_UPNP:
			if (!bt_global::xml_device)
				bt_global::xml_device = new XmlDevice;
			MultimediaFileListFactory f(TreeBrowser::UPNP, EntryInfo::DIRECTORY | EntryInfo::AUDIO, false);
			p = upnp_page = f.getFileSelector();
			descr = tr("Servers");
			break;
		}

		if (p)
		{
			QHBoxLayout *hbox = new QHBoxLayout;
			hbox->setContentsMargins(0, 0, 0, 0);
			hbox->setSpacing(10);
			QLabel *l = new QLabel(descr);
			l->setFont(bt_global::font->get(FontManager::TEXT));
			hbox->addWidget(l);
			BtButton *b = new BtButton(bt_global::skin->getImage("forward"));
			hbox->addWidget(b);
			connect(b, SIGNAL(clicked()), p, SLOT(showPage()));
			connect(p, SIGNAL(Closed()), SLOT(handleClose()));
			main_layout->addLayout(hbox);
		}
	}
	main_layout->addStretch();
}

void MultimediaContainer::playSomethingRandomly()
{
	if (radio_page)
	{
		qDebug() << "Playing from Web radio";
		AudioPlayerPage *page = AudioPlayerPage::getAudioPlayerPage(AudioPlayerPage::IP_RADIO);
		page->playAudioFilesBackground(radio_page->radioUrls(), 0);
	}
}

void MultimediaContainer::showPage()
{
	play_button->hide();
	foreach (AudioPlayerPage *page, AudioPlayerPage::audioPlayerPages())
	{
		if (page && page->isPlayerInstanceRunning())
		{
			play_button->show();
			break;
		}
	}

	Page::showPage();
}

void MultimediaContainer::handleClose()
{
	if (radio_page && upnp_page)
		showPage();
	else
		emit Closed();
}

void MultimediaContainer::gotoPlayerPage()
{
	foreach (AudioPlayerPage *page, AudioPlayerPage::audioPlayerPages())
		if (page->isPlayerInstanceRunning())
		{
			page->showPage();
			break;
		}
}
