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


#include "stopngo.h"
#include "fontmanager.h" // bt_global::font
#include "icondispatcher.h" // bt_global::icons_cache
#include "devices_cache.h" // bt_global::devices_cache
#include "stopandgo_device.h"
#include "skinmanager.h"
#include "xml_functions.h" // getTextChild

enum {
	STOP_AND_GO = 6101,
	STOP_AND_GO_PLUS = 6102,
	STOP_AND_GO_BTEST = 6103,
};

enum
{
	STATUS_CLOSED = 0,
	STATUS_OPENED,
	STATUS_LOCKED,
	STATUS_FAIL,
	STATUS_GROUND_FAIL,
	STATUS_VMAX
};


BannStopAndGo::BannStopAndGo(StopAndGoDevice *dev, QWidget *parent) :
	Bann2Buttons(parent)
{
	status_icons[STATUS_CLOSED] = bt_global::skin->getImage("status_closed");
	status_icons[STATUS_OPENED] = bt_global::skin->getImage("status_opened");
	status_icons[STATUS_LOCKED] = bt_global::skin->getImage("status_locked");
	status_icons[STATUS_FAIL] = bt_global::skin->getImage("status_cc");
	status_icons[STATUS_GROUND_FAIL] = bt_global::skin->getImage("status_ground_fail");
	status_icons[STATUS_VMAX] = bt_global::skin->getImage("status_vmax");

	initBanner("", status_icons[STATUS_CLOSED], bt_global::skin->getImage("forward_icon"));

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(statusChanged(DeviceValues)));
}

void BannStopAndGo::statusChanged(const DeviceValues &values_list)
{
	int status = values_list[StopAndGoDevice::DIM_ICM_STATE].toInt();
	QString icon;

	if (!getStatusValue(status, OPENED))
		icon = status_icons[STATUS_CLOSED];
	else
	{
		if (getStatusValue(status, LOCKED))
			icon = status_icons[STATUS_LOCKED];
		else if (getStatusValue(status, OPENED_LE_N))
			icon = status_icons[STATUS_FAIL];
		else if (getStatusValue(status, OPENED_GROUND))
			icon = status_icons[STATUS_GROUND_FAIL];
		else if (getStatusValue(status, OPENED_VMAX))
			icon = status_icons[STATUS_VMAX];
		else
			icon = status_icons[STATUS_OPENED];
	}
	setBackgroundImage(icon);
}


StopAndGoMenu::StopAndGoMenu(const QDomNode &conf_node)
{
#ifdef LAYOUT_BTOUCH
	buildPage();
#else
	buildPage(getTextChild(conf_node, "descr"));
#endif

	QList<QDomNode> items = getChildren(conf_node, "item");
	foreach (const QDomNode &item, items)
	{
		int id = getTextChild(item, "id").toInt();
		QString descr = getTextChild(item, "descr");
		QString where = getTextChild(item, "where");
		StopAndGoDevice *dev = 0;
		Page *p = 0;

		switch (id)
		{
		case STOP_AND_GO:
			dev = bt_global::add_device_to_cache(new StopAndGoDevice(where));
			p = new StopAndGoPage();
			break;
		case STOP_AND_GO_PLUS:
			dev = bt_global::add_device_to_cache(new StopAndGoPlusDevice(where));
			p = new StopAndGoPlusPage();
			break;
		case STOP_AND_GO_BTEST:
			dev = bt_global::add_device_to_cache(new StopAndGoBTestDevice(where));
			p = new StopAndGoBTestPage;
			break;
		default:
			Q_ASSERT_X(false, "StopAndGoMenu::StopAndGoMenu", "Unknown id");
		}

		BannStopAndGo *banner = new BannStopAndGo(dev);
		banner->connectRightButton(p);
		connect(banner, SIGNAL(pageClosed()), SLOT(showPage()));

		page_content->appendBanner(banner);

		next_page = p;
	}

	// skip list if there's only one item
	if (items.size() > 1)
		next_page = NULL;
	else
		connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

void StopAndGoMenu::showPage()
{
	if (next_page)
		next_page->showPage();
	else
		BannerPage::showPage();
}


StopAndGoPage::StopAndGoPage() :
	Page()
{
}


StopAndGoPlusPage::StopAndGoPlusPage() :
	Page()
{
}


StopAndGoBTestPage::StopAndGoBTestPage() :
	Page()
{
};
