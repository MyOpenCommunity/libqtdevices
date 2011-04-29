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


#include "settings.h"
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


Settings::Settings(const QDomNode &config_node)
{
	buildPage();
	// Because there are no banners with text under buttons, this is the quickly
	// way to adjust the spacing.
	setSpacing(10);
	loadItems(config_node);
}

banner *Settings::getBanner(const QDomNode &item_node)
{
	SkinContext context(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	banner *b = 0;

	QString descr = getTextChild(item_node, "descr");

	switch (id)
	{
	case SET_BEEP:
		b = new BannBeep(getTextChild(item_node, "itemID").toInt(),
				getTextChild(item_node, "value").toInt(),
				bt_global::skin->getImage("state_on"),
				bt_global::skin->getImage("state_off"),
				getTextChild(item_node, "descr"));
		break;
	case SET_ALARMCLOCK:
	{
		int item_id = getTextChild(item_node, "itemID").toInt();
		int type = getTextChild(item_node, "type").toInt();
		int enabled = getTextChild(item_node, "enabled").toInt();
		int hour = getTextChild(item_node, "hour").toInt();
		int minute = getTextChild(item_node, "minute").toInt();
		int alarmset = getTextChild(item_node, "alarmset").toInt();

		b = new BannAlarmClock(item_id, hour, minute, bt_global::skin->getImage("state_on"),
			bt_global::skin->getImage("state_off"), bt_global::skin->getImage("edit"),
			getTextChild(item_node, "descr"), enabled, type, alarmset);

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
	case CONTRAST:
		// TODO CONTRAST below needs to be changed when removing CONFIG_TS_3_5
		b = new BannContrast(CONTRAST, getTextChild(item_node, "value").toInt(),
					 bt_global::skin->getImage("edit"), descr);
		break;
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
				bt_global::skin->getImage("edit"), getTextChild(item_node, "descr"), PASSWORD,
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
		b->Draw();
	}

	return b;
}

void Settings::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
#ifdef BT_HARDWARE_X11
		if (id == CONTRAST)
			continue;
#endif
		if (banner *b = getBanner(item))
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
			qFatal("Type of item %d not handled on settings page!", id);
	}
}

