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
#include "banntemperature.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "plantmenu.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "bannfrecce.h"
#include "airconditioning.h"
#include "skinmanager.h" // bt_global::skin

#include <QRegExp>
#include <QDebug>

#include <assert.h>

#define IMG_OK IMG_PATH "btnok.png"

static const QString i_right_arrow = QString("%1%2").arg(IMG_PATH).arg("arrrg.png");

ThermalMenu::ThermalMenu(const QDomNode &config_node)
{
	bann_number = 0;
	loadBanners(config_node);

	// check if plant menus are created?
	if (bann_number == 1)
	{
		connect(single_page, SIGNAL(Closed()), SIGNAL(Closed()));
	}
}

void ThermalMenu::createPlantMenu(QDomNode config, bannPuls *bann)
{
	sottoMenu *sm = new PlantMenu(NULL, config);

	connect(bann, SIGNAL(sxClick()), sm, SLOT(showPage()));
	connect(sm, SIGNAL(Closed()), this, SLOT(showPage()));
	single_page = sm;
}

void ThermalMenu::loadBanners(const QDomNode &config_node)
{
	SkinContext ctx(getTextChild(config_node, "cid").toInt());
	bannPuls *b = NULL;
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
		b->connectDxButton(single_page);
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	}
}

bannPuls *ThermalMenu::addMenuItem(QDomElement e, QString central_icon)
{
	bannPuls *bp = new bannPuls(this);

	bp->SetIcons(i_right_arrow, QString(), central_icon);
	bp->setText(getTextChild(e, "descr"));

	elencoBanner.append(bp);
	connectLastBanner();

	++bann_number;
	return bp;
}

void ThermalMenu::createProbeMenu(QDomNode config, bannPuls *bann, bool external)
{
	sottoMenu *sm = new sottoMenu;
	single_page = sm;
	// we want to scroll external probes per pages and not per probes
	// By default, submenus show only 3 banners in each page (see sottomenu.h:44)
	unsigned submenu_scroll_step = NUM_RIGHE - 1;
	sm->setScrollStep(submenu_scroll_step);

	connect(bann, SIGNAL(sxClick()), sm, SLOT(showPage()));
	connect(sm, SIGNAL(Closed()), this, SLOT(showPage()));

	foreach (const QDomNode &item, getChildren(config, "item"))
	{
		QString addr = getTextChild(item, "where");
		QString text = getTextChild(item, "descr");
		if (external)
			addr += "00";
		device *dev = bt_global::devices_cache.get_temperature_probe(addr, external);

		banner *b = new BannTemperature(sm, addr, text, dev);
		b->setText(text);

		sm->appendBanner(b);
	}
}

void ThermalMenu::showPage()
{
	if (bann_number == 1)
		single_page->showPage();
	else
		sottoMenu::showPage();
}

ProgramMenu::ProgramMenu(QWidget *parent, QDomNode conf) : sottoMenu(parent)
{
	conf_root = conf;
}

void ProgramMenu::setSeason(Season new_season)
{
	if (new_season != season)
	{
		season = new_season;
		switch (season)
		{
		case SUMMER:
			createSummerBanners();
			break;
		case WINTER:
			createWinterBanners();
			break;
		}

		forceDraw();
	}
}

void ProgramMenu::createSeasonBanner(const QString season, const QString what, const QString icon)
{
	assert((what == "scen" || what == "prog") && "'what' must be either 'prog' or 'scen'");

	bool create_banner = false;
	if (elencoBanner.isEmpty())
		create_banner = true;

	const QString i_ok = QString(IMG_PATH) + "btnok.png";

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] WeeklyMenu:wrong node in config file");
	}
	QDomElement program = getElement(conf_root, season + "/" + what);
	// The leaves we are looking for start with either "p" or "s"
	QString name = what.left(1);

	int index = 0;
	foreach (const QDomNode &node, getChildren(program, name))
	{
		BannWeekly *bp = 0;
		if (create_banner)
		{
			bp = new BannWeekly(this);
			elencoBanner.append(bp);
			connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
		}
		if (index >= elencoBanner.size())
		{
			qWarning("ProgramMenu::createSeasonBanner: updating a menu with different number \
of programs between summer and winter");
			break;
		}
		bp = static_cast<BannWeekly*>(elencoBanner[index]);
		++index;
		bp->SetIcons(i_ok, QString(), icon);
		// set Text taken from conf.xml
		if (node.isElement())
		{
			bp->setText(node.toElement().text());
			// here node name is of the form "s12" or "p3"
			int program_number = node.nodeName().mid(1).toInt();
			bp->setProgram(program_number);
		}
	}
}

WeeklyMenu::WeeklyMenu(QWidget *parent, QDomNode conf) : ProgramMenu(parent, conf)
{
	season = SUMMER;
	createSummerBanners();
}

void WeeklyMenu::createSummerBanners()
{
	const QString i_central = QString(IMG_PATH) + "programma_estivo.png";
	createSeasonBanner("summer", "prog", i_central);
}

void WeeklyMenu::createWinterBanners()
{
	const QString i_central = QString(IMG_PATH) + "programma_invernale.png";
	createSeasonBanner("winter", "prog", i_central);
}

ScenarioMenu::ScenarioMenu(QWidget *parent, QDomNode conf) : ProgramMenu(parent, conf)
{
	season = SUMMER;
	createSummerBanners();
}

void ScenarioMenu::createSummerBanners()
{
	const QString i_central = QString(IMG_PATH) + "scenario_estivo.png";
	createSeasonBanner("summer", "scen", i_central);

}

void ScenarioMenu::createWinterBanners()
{
	const QString i_central = QString(IMG_PATH) + "scenario_invernale.png";
	createSeasonBanner("winter", "scen", i_central);
}

TimeEditMenu::TimeEditMenu(QWidget *parent) : sottoMenu(parent, 10, MAX_WIDTH, MAX_HEIGHT, 1)
{
	setNavBarMode(10, IMG_OK);
	time_edit = new FSBannTime(this);
	elencoBanner.append(time_edit);
	connect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(performAction()));
}

void TimeEditMenu::performAction()
{
	emit timeSelected(time());
}

BtTime TimeEditMenu::time()
{
	return time_edit->time();
}

DateEditMenu::DateEditMenu(QWidget *parent) : sottoMenu(parent, 10, MAX_WIDTH, MAX_HEIGHT, 1)
{
	setNavBarMode(10, IMG_OK);
	date_edit = new FSBannDate(this);
	elencoBanner.append(date_edit);
	connect(bannNavigazione, SIGNAL(forwardClick()), this, SLOT(performAction()));
}

void DateEditMenu::performAction()
{
	emit dateSelected(date());
}

QDate DateEditMenu::date()
{
	return date_edit->date();
}
