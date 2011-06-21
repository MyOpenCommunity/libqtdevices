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


#include "ipradio.h"
#include "skinmanager.h"
#include "xml_functions.h" // getTextChild, getChildren
#ifdef LAYOUT_TS_3_5
#include "audioplayer_ts3.h"
#else
#include "audioplayer_ts10.h"
#endif
#include "itemlist.h"
#include "navigation_bar.h"
#include "sounddiffusionpage.h" // SoundDiffusionPage::showCurrentAmbientPage

#include <QLayout>


IPRadioPage::IPRadioPage(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());

	ItemList *item_list = new ItemList;

	player = AudioPlayerPage::getAudioPlayerPage(AudioPlayerPage::IP_RADIO);

	PageTitleWidget *title_widget = 0;

	NavigationBar *nav_bar;

#ifdef LAYOUT_TS_10
	title_widget = new PageTitleWidget(getTextChild(config_node, "descr"), SMALL_TITLE_HEIGHT);
	nav_bar = new NavigationBar;
#else
	nav_bar = new NavigationBar("goto_sounddiffusion");
	connect(nav_bar, SIGNAL(forwardClick()), SLOT(gotoSoundDiffusion()));
#endif

	buildPage(item_list, item_list, nav_bar, title_widget);

#ifdef LAYOUT_TS_10
	layout()->setContentsMargins(13, 5, 25, 10);
#endif

	loadItems(config_node);
	connect(page_content, SIGNAL(itemIsClicked(int)), SLOT(itemIsClicked(int)));
}

void IPRadioPage::gotoSoundDiffusion()
{
	SoundDiffusionPage::showCurrentAmbientPage();
}

void IPRadioPage::showPage()
{
#ifdef LAYOUT_TS_10
	player->showPrevButton(page_content->itemCount() > 1);
	player->showNextButton(page_content->itemCount() > 1);
#endif
	ScrollablePage::showPage();
}

void IPRadioPage::loadItems(const QDomNode &config_node)
{
	QList<ItemList::ItemInfo> radio_list;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		QStringList icons;
#ifdef LAYOUT_TS_10
		icons << bt_global::skin->getImage("ipradio_icon");
		icons << bt_global::skin->getImage("forward");
#else
		icons << bt_global::skin->getImage("forward");
#endif

		ItemList::ItemInfo info(getTextChild(item, "title"), getTextChild(item, "descr"),
					icons,
					getTextChild(item, "url"));

		radio_list.append(info);
	}

	page_content->setList(radio_list);
	page_content->showList();
}

QStringList IPRadioPage::radioUrls()
{
	QStringList urls;
	for (int i = 0; i < page_content->itemCount(); ++i)
		urls.append(page_content->item(i).data.toString());

	return urls;
}

void IPRadioPage::itemIsClicked(int index)
{
	player->playAudioFiles(radioUrls(), index);
	disconnect(player, SIGNAL(Closed()), this, SLOT(radioClosed())); // avoid multiple connections
	connect(player, SIGNAL(Closed()), this, SLOT(radioClosed()));
}

void IPRadioPage::radioClosed()
{
	if (!player->isPlayerInstanceRunning())
		disconnect(player, SIGNAL(Closed()), this, SLOT(radioClosed()));
	showPage();
}
