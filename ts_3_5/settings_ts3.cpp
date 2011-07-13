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


#include "settings_ts3.h"
#include "bann_settings.h"
#include "btmain.h" // bt_global::btmain
#include "lansettings.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "cleanscreen.h"
#include "changedatetime.h"
#include "brightnesspage.h"
#include "displaypage.h"
#include "version.h"
#include "main.h"
#include "skinmanager.h"

#include <QDebug>

enum
{
#ifdef CONFIG_TS_3_5
	SET_BEEP=25,                                  /*!<  Beep */
	SET_DATETIME=14,                              /*!<  Time setting */
	PASSWORD=26,                                  /*!<  Password's settings */
	VERSION=27,                                   /*!<  Version */
	CONTRAST=28,                                  /*!<  Contrast */
	DISPLAY=21,                                   /*!<  Display */
	LANSETTINGS=72,                               /*!< LAN settings and information */
	SET_ALARMCLOCK=20,                            /*!<  AlarmClock setting */
#else
	SET_BEEP=14001,                               /*!<  Beep */
	SET_DATETIME=14002,                           /*!<  Time setting */
	PASSWORD=14003,                               /*!<  Password's settings */
	VERSION=14006,                                /*!<  Version */
	DISPLAY=14007,                                /*!<  Display */
	LANSETTINGS=14008,                            /*!< LAN settings and information */
	PAGE_ALARMCLOCK = 14009,                      /*!< The alarm clock page */
	SET_ALARMCLOCK=14201,                         /*!<  AlarmClock setting */
#endif
};

Settings::Settings(const QDomNode &config_node)
{
	buildPage();
	// Because there are no banners with text under buttons, this is the quickly
	// way to adjust the spacing.
	setSpacing(10);
	loadItems(config_node);
}

Banner *Settings::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	int item_id = getTextChild(item_node, "itemID").toInt();
	Banner *b = 0;

	QString descr = getTextChild(item_node, "descr");

	switch (id)
	{
	case SET_BEEP:
	{
#ifdef CONFIG_TS_3_5
		bool enable = getTextChild(item_node, "value").toInt();
#else
		bool enable = getTextChild(item_node, "enabled").toInt();
#endif
		b = new BannBeep(getTextChild(item_node, "itemID").toInt(),
				enable,
				bt_global::skin->getImage("state_on"),
				bt_global::skin->getImage("state_off"),
				getTextChild(item_node, "descr"));
		break;
	}
	case SET_ALARMCLOCK:
	{

		int type = getTextChild(item_node, "type").toInt();
		int enabled = getTextChild(item_node, "enabled").toInt();
		int hour = getTextChild(item_node, "hour").toInt();
		int minute = getTextChild(item_node, "minute").toInt();
		int days = getTextChild(item_node, "days").toInt();

		b = new BannAlarmClock(item_id, hour, minute, bt_global::skin->getImage("state_on"),
			bt_global::skin->getImage("state_off"), bt_global::skin->getImage("edit"),
			getTextChild(item_node, "descr"), enabled, type, days);

		break;
	}
	case PAGE_ALARMCLOCK:
	{
		QDomNode page_node = getPageNodeFromChildNode(item_node, "lnk_pageID");
		ListPage *list_page = new ListPage(page_node);
		list_page->setSpacing(10);
		Bann2Buttons *bann = new Bann2Buttons;
		bann->initBanner(QString(), bt_global::skin->getImage("alarmclock"), descr);
		bann->connectRightButton(list_page);
		b = bann;
		break;
	}
	case SET_DATETIME:
	{
		Bann2Buttons *bann = new Bann2Buttons;
		bann->initBanner(QString(), bt_global::skin->getImage("info"), descr);
		bann->connectRightButton(new ChangeTime);
		b = bann;
		break;
	}
#ifdef CONFIG_TS_3_5 // To be or not to be? Remove the Contrast or port in the new version of the ts3?
	case CONTRAST:
		// TODO CONTRAST below needs to be changed when removing CONFIG_TS_3_5
		b = new BannContrast(CONTRAST, getTextChild(item_node, "value").toInt(),
					 bt_global::skin->getImage("edit"), descr);
		break;
#endif
	case DISPLAY:
	{
		Bann2Buttons *bann = new Bann2Buttons;
		bann->initBanner(QString(), bt_global::skin->getImage("forward"), descr);
		bann->connectRightButton(new DisplayPage(item_node));
		b = bann;
		break;
	}
	case PASSWORD:
		b = new BannPassword(bt_global::skin->getImage("state_on"), bt_global::skin->getImage("state_off"),
				bt_global::skin->getImage("edit"), getTextChild(item_node, "descr"), item_id,
				getTextChild(item_node, "password"), getTextChild(item_node, "actived").toInt());
		break;
	case LANSETTINGS:
	{
		Bann2Buttons *bann = new Bann2Buttons;
		bann->initBanner(QString(), bt_global::skin->getImage("info"), descr);
		bann->connectRightButton(new LanSettings(item_node));
		b = bann;
		break;
	}
	case VERSION:
		b = new BannVersion(bt_global::skin->getImage("info"), descr, bt_global::btmain->version);
		break;
	}

	if (b)
	{
		b->setText(descr);
		b->setId(id);
	}

	return b;
}

void Settings::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
#ifdef BT_HARDWARE_X11
#ifdef CONFIG_TS_3_5
		if (id == CONTRAST)
			continue;
#endif
#endif
		if (Banner *b = getBanner(item))
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
			qFatal("Type of item %d not handled on settings page!", id);
	}
}

