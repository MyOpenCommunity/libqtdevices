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


#include "plantmenu.h"
#include "xml_functions.h"
#include "main.h" //(*bt_global::config)
#include "bann1_button.h"
#include "bannercontent.h"
#include "skinmanager.h"

#include <QVariant>
#include <QRegExp>


PlantMenu::PlantMenu(const QDomNode &conf) : BannerPage(0)
{
	SkinContext cxt(getTextChild(conf, "cid").toInt());

	buildPage(getTextChild(conf, "descr"));
	openserver_id = 0;
	loadItems(conf);
}

#ifdef CONFIG_BTOUCH
void PlantMenu::loadItems(const QDomNode &conf)
{
	QDomNode thermr_address = conf.namedItem("ind_centrale");
	if (thermr_address.isNull())
		ind_centrale = "0";
	else
		ind_centrale = thermr_address.toElement().text();

	QDomNode n = conf.firstChild();
	NavigationPage *first = 0, *prev = 0;
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			NavigationPage *pg = 0;

			SkinContext context(getTextChild(n, "cid").toInt());

			int id = n.namedItem("id").toElement().text().toInt();
			switch (id)
			{
				case TERMO_99Z:
					pg = addMenuItem(n, bt_global::skin->getImage("regulator"), fs_99z_thermal_regulator);
					break;
				case TERMO_4Z:
					pg = addMenuItem(n, bt_global::skin->getImage("regulator"), fs_4z_thermal_regulator);
					break;
				case TERMO_99Z_PROBE:
					pg = addMenuItem(n, bt_global::skin->getImage("zone"), fs_99z_probe);
					break;
				case TERMO_99Z_PROBE_FANCOIL:
					pg = addMenuItem(n, bt_global::skin->getImage("zone"), fs_99z_fancoil);
					break;
				case TERMO_4Z_PROBE:
					pg = addMenuItem(n, bt_global::skin->getImage("zone"), fs_4z_probe);
					break;
				case TERMO_4Z_PROBE_FANCOIL:
					pg = addMenuItem(n, bt_global::skin->getImage("zone"), fs_4z_fancoil);
					break;
			}

			if (prev)
			{
				connect(prev, SIGNAL(downClick()), pg, SLOT(showPage()));
				connect(pg, SIGNAL(upClick()), prev, SLOT(showPage()));
			}
			connect(pg, SIGNAL(backClick()), SLOT(showPage()));

			prev = pg;
			if (!first)
				first = pg;
		}
		n = n.nextSibling();
	}

	connect(prev, SIGNAL(downClick()), first, SLOT(showPage()));
	connect(first, SIGNAL(upClick()), prev, SLOT(showPage()));
}
#else
void PlantMenu::loadItems(const QDomNode &config_node)
{
	// find thermal regulator address
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();

		if (id == TERMO_4Z)
		{
			ind_centrale = getTextChild(item, "where");
			openserver_id = getTextChild(item, "openserver_id").toInt();
			break;
		}
		else if (id == TERMO_99Z)
		{
			ind_centrale = "0";
			openserver_id = getTextChild(item, "openserver_id").toInt();
			break;
		}
	}


	NavigationPage *first = 0, *prev = 0;
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		SkinContext context(getTextChild(item, "cid").toInt());
		int id = getTextChild(item, "id").toInt();

		NavigationPage *pg = 0;

		QString icon = bt_global::skin->getImage("central_icon");
		bool fancoil = getTextChild(item, "fancoil").toInt();

		switch (id)
		{
		case TERMO_99Z:
			pg = addMenuItem(item, icon, fs_99z_thermal_regulator);
			break;
		case TERMO_4Z:
			pg = addMenuItem(item, icon, fs_4z_thermal_regulator);
			break;
#ifdef CONFIG_BTOUCH
		case TERMO_99Z_PROBE:
			pg = addMenuItem(item, icon, fs_99z_probe);
			break;
		case TERMO_99Z_PROBE_FANCOIL:
			pg = addMenuItem(item, icon, fs_99z_fancoil);
			break;
		case TERMO_4Z_PROBE:
			pg = addMenuItem(item, icon, fs_4z_probe);
			break;
		case TERMO_4Z_PROBE_FANCOIL:
			pg = addMenuItem(item, icon, fs_4z_fancoil);
			break;
#else
		case TERMO_99Z_PROBE:
			pg = addMenuItem(item, icon, fancoil ? fs_99z_fancoil : fs_99z_probe);
			break;
		case TERMO_4Z_PROBE:
			pg = addMenuItem(item, icon, fancoil ? fs_4z_fancoil : fs_4z_probe);
			break;
#endif
		}

		if (prev)
		{
			connect(prev, SIGNAL(downClick()), pg, SLOT(showPage()));
			connect(pg, SIGNAL(upClick()), prev, SLOT(showPage()));
		}
		connect(pg, SIGNAL(backClick()), SLOT(showPage()));

		prev = pg;
		if (!first)
			first = pg;
	}

	connect(prev, SIGNAL(downClick()), first, SLOT(showPage()));
	connect(first, SIGNAL(upClick()), prev, SLOT(showPage()));
}
#endif

banner *PlantMenu::getBanner(const QDomNode &item_node)
{
	return NULL;
}

NavigationPage *PlantMenu::addMenuItem(QDomNode n, QString central_icon, BannID type)
{
	/*
	 * Create little banner in selection menu.
	 */
	BannSinglePuls *bp = new BannSinglePuls(this);
	bp->initBanner(bt_global::skin->getImage("forward"), central_icon, getTextChild(n, "descr"));
	page_content->appendBanner(bp);

	/*
	 * Create page in detail menu.
	 */
	TemperatureScale scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());
	NavigationPage *p = getPage(type, n, ind_centrale, openserver_id, scale, bp);
	bp->connectRightButton(p);
	connect(bp, SIGNAL(pageClosed()), SLOT(showPage()));

	return p;
}
