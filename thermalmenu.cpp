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


#include "thermalmenu.h"
#include "bann_thermal_regulation.h" // BannTemperature
#include "devices_cache.h" // bt_global::devices_cache
#include "probe_device.h"
#include "plantmenu.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "skinmanager.h"
#include "airconditioning.h"
#include "navigation_bar.h"
#include "icondispatcher.h"
#include "fontmanager.h"
#include "main.h" // THERMALREGULATION

#include <QRegExp>
#include <QDebug>
#include <QLabel>

enum
{
	PAGE_PLANT_MENU_4ZONES = 8040,
	PAGE_PLANT_MENU_99ZONES = 8010,
	PAGE_EXTERNAL_PROBES = 8020,
	PAGE_INTERNAL_PROBES = 8030,
	PAGE_AIRCONDITIONING = 4000,
};

ThermalMenu::ThermalMenu(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	bann_number = 0;
	buildPage(getTextChild(config_node, "descr"));
	loadBanners(config_node);

	// check if plant menus are created?
	if (bann_number == 1)
		connect(single_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

int ThermalMenu::sectionId() const
{
	return THERMALREGULATION;
}

void ThermalMenu::createPlantMenu(QDomNode config, BannSinglePuls *bann)
{
	Page *sm = new PlantMenu(config);
	bann->connectRightButton(sm);
	connect(bann, SIGNAL(pageClosed()), SLOT(showPage()));
	single_page = sm;
}

#ifdef CONFIG_TS_3_5

void ThermalMenu::loadBanners(const QDomNode &config_node)
{
	BannSinglePuls *b = NULL;
	foreach (const QDomNode &node, getChildren(config_node, "plant"))
	{
		b = addMenuItem(node.toElement(), bt_global::skin->getImage("plant"));
		createPlantMenu(node.toElement(), b);
	}
	foreach (const QDomNode &node, getChildren(config_node, "extprobe"))
	{
		b = addMenuItem(node.toElement(), bt_global::skin->getImage("extprobe"));
		createProbeMenu(node.toElement(), b, true);
	}
	foreach (const QDomNode &node, getChildren(config_node, "tempprobe"))
	{
		b = addMenuItem(node.toElement(), bt_global::skin->getImage("probe"));
		createProbeMenu(node.toElement(), b, false);
	}

	QDomNode air_node = getChildWithName(config_node, "airconditioning");
	if (!air_node.isNull())
	{
		b = addMenuItem(air_node.toElement(), bt_global::skin->getImage("air_conditioning"));
		single_page = new AirConditioning(air_node);
		b->connectRightButton(single_page);
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}

#else

void ThermalMenu::loadBanners(const QDomNode &config_node)
{
	foreach (const QDomNode &node, getChildren(config_node, "item"))
	{
		SkinContext context(getTextChild(node, "cid").toInt());
		BannSinglePuls *bp = addMenuItem(node.toElement(), bt_global::skin->getImage("central_icon"));

		QDomNode page_node = getPageNodeFromChildNode(node, "lnk_pageID");
		int id = getTextChild(page_node, "id").toInt();
		Page *p;

		switch (id)
		{
		case PAGE_PLANT_MENU_4ZONES:
		case PAGE_PLANT_MENU_99ZONES:
			p = new PlantMenu(page_node);
			break;
		case PAGE_EXTERNAL_PROBES:
			p = new ProbesPage(page_node, true);
			break;
		case PAGE_INTERNAL_PROBES:
			p = new ProbesPage(page_node, false);
			break;
		case PAGE_AIRCONDITIONING:
			p = new AirConditioning(page_node);
			break;
		default:
			qFatal("Unhandled id in ThermalMenu::loadBanners");
		}
		single_page = p;

		bp->connectRightButton(p);
		connect(bp, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}

#endif

BannSinglePuls *ThermalMenu::addMenuItem(QDomElement e, QString central_icon)
{
	BannSinglePuls *bp = new BannSinglePuls(this);
	bp->initBanner(bt_global::skin->getImage("forward"), central_icon, getTextChild(e, "descr"));

	page_content->appendBanner(bp);
	++bann_number;
	return bp;
}

#ifdef CONFIG_TS_3_5

void ThermalMenu::createProbeMenu(QDomNode config, BannSinglePuls *bann, bool external)
{
	ProbesPage *sm = new ProbesPage(config, external);
	single_page = sm;

	bann->connectRightButton(sm);
	connect(bann, SIGNAL(pageClosed()), SLOT(showPage()));
}

#endif

void ThermalMenu::showPage()
{
	if (bann_number == 1)
		single_page->showPage();
	else
		Page::showPage();
}


#ifdef LAYOUT_TS_3_5

ProbesPage::ProbesPage(const QDomNode &config_node, bool are_probes_external)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node, are_probes_external);
}

#else

static QLabel *getLabel(const QString &text)
{
	QLabel *l = new QLabel(text);
	l->setAlignment(Qt::AlignCenter);
	l->setFont(bt_global::font->get(FontManager::TITLE));

	return l;
}

ProbesPage::ProbesPage(const QDomNode &config_node, bool are_probes_external)
{
	QWidget *content = new QWidget;
	QGridLayout *l = new QGridLayout(content);
	BannerContent *banners = new BannerContent(0, 1);

	QLabel *icon = new QLabel;
	icon->setPixmap(*bt_global::icons_cache.getIcon(bt_global::skin->getImage("central_icon")));

	l->addWidget(getLabel(are_probes_external ? tr("Probe") : tr("Zone")), 0, 1);
	l->addWidget(getLabel(tr("Temperature")), 0, 2);
	l->addWidget(icon, 1, 0);
	l->addWidget(banners, 1, 1, 2, 2);
	l->setRowStretch(2, 1);
	l->setColumnStretch(1, 1);
	l->setColumnStretch(2, 1);

	buildPage(content, banners, new NavigationBar,
		  getTextChild(config_node, "descr"), 40);
	loadItems(config_node, are_probes_external);
}

#endif

void ProbesPage::loadItems(const QDomNode &config_node, bool are_probes_external)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		QString addr = getTextChild(item, "where");
		QString text = getTextChild(item, "descr");
#ifdef CONFIG_TS_3_5
		if (are_probes_external)
			addr += "00";
#endif

		NonControlledProbeDevice *dev = bt_global::add_device_to_cache(new NonControlledProbeDevice(addr,
			are_probes_external ? NonControlledProbeDevice::EXTERNAL : NonControlledProbeDevice::INTERNAL));
		banner *b = new BannTemperature(text, dev);

		page_content->appendBanner(b);
	}
}

int ProbesPage::sectionId() const
{
	return THERMALREGULATION;
}
