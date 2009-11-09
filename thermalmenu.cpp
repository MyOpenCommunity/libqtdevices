/*!
 * \thermalmenu.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief  A class to handle thermal regulation menu
 *
 * \author Luca Ottaviano
 */

#include "thermalmenu.h"
#include "banntemperature.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "plantmenu.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "bannfrecce.h"
#include "content_widget.h" // content_widget
#include "skinmanager.h"

#include <QRegExp>
#include <QDebug>

#define IMG_OK IMG_PATH "btnok.png"

static const QString i_right_arrow = QString("%1%2").arg(IMG_PATH).arg("arrrg.png");
static const QString i_temp_probe = QString("%1%2").arg(IMG_PATH).arg("zona.png");
static const QString i_ext_probe = QString("%1%2").arg(IMG_PATH).arg("sonda_esterna.png");
static const QString i_plant = QString("%1%2").arg(IMG_PATH).arg("impianto.png");


ThermalMenu::ThermalMenu(const QDomNode &config_node)
{
	bann_number = 0;
	buildPage();
	loadBanners(config_node);

	// check if plant menus are created?
	if (bann_number == 1)
		connect(single_submenu, SIGNAL(Closed()), SIGNAL(Closed()));
}

void ThermalMenu::createPlantMenu(QDomNode config, bannPuls *bann)
{
	Page *sm = new PlantMenu(NULL, config);
	connect(bann, SIGNAL(sxClick()), sm, SLOT(showPage()));
	connect(sm, SIGNAL(Closed()), this, SLOT(showPage()));
	single_submenu = sm;
}

void ThermalMenu::loadBanners(const QDomNode &config_node)
{
	bannPuls *b = NULL;
	foreach (const QDomNode &node, getChildren(config_node, "plant"))
	{
		b = addMenuItem(node.toElement(), i_plant);
		createPlantMenu(node.toElement(), b);
	}
	foreach (const QDomNode &node, getChildren(config_node, "extprobe"))
	{
		b = addMenuItem(node.toElement(), i_ext_probe);
		createProbeMenu(node.toElement(), b, true);
	}
	foreach (const QDomNode &node, getChildren(config_node, "tempprobe"))
	{
		b = addMenuItem(node.toElement(), i_temp_probe);
		createProbeMenu(node.toElement(), b, false);
	}
}

bannPuls *ThermalMenu::addMenuItem(QDomElement e, QString central_icon)
{
	bannPuls *bp = new bannPuls(this);

	bp->SetIcons(i_right_arrow, QString(), central_icon);
	bp->setText(getTextChild(e, "descr"));
	bp->Draw();
	page_content->appendBanner(bp);
	++bann_number;
	return bp;
}

void ThermalMenu::createProbeMenu(QDomNode config, bannPuls *bann, bool external)
{
	sottoMenu *sm = new sottoMenu;
	single_submenu = sm;
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
		single_submenu->showPage();
	else
		Page::showPage();
}


ProgramMenu::ProgramMenu(QWidget *parent, QDomNode conf) : BannerPage(parent)
{
	conf_root = conf;
	buildPage();
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
	Q_ASSERT_X(what == "scen" || what == "prog", "ProgramMenu::createSeasonBanner",
		"'what' must be either 'prog' or 'scen'");

	bool create_banner = false;
	if (page_content->bannerCount() == 0)
		create_banner = true;

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
			page_content->appendBanner(bp);
			connect(bp, SIGNAL(programNumber(int)), SIGNAL(programClicked(int)));
		}
		if (index >= page_content->bannerCount())
		{
			qWarning("ProgramMenu::createSeasonBanner: updating a menu with different number of programs between summer and winter");
			break;
		}
		bp = static_cast<BannWeekly*>(page_content->getBanner(index));
		++index;
		// set Text taken from conf.xml
		QString text = "";
		if (node.isElement())
		{
			text = node.toElement().text();
			// here node name is of the form "s12" or "p3"
			int program_number = node.nodeName().mid(1).toInt();
			bp->setProgram(program_number);
		}
		bp->initBanner(bt_global::skin->getImage("ok"), icon, text);
	}
}

WeeklyMenu::WeeklyMenu(QWidget *parent, QDomNode conf) : ProgramMenu(parent, conf)
{
	summer_icon = bt_global::skin->getImage("summer_program");
	winter_icon = bt_global::skin->getImage("winter_program");
	season = SUMMER;
	createSummerBanners();
}

void WeeklyMenu::createSummerBanners()
{
	createSeasonBanner("summer", "prog", summer_icon);
}

void WeeklyMenu::createWinterBanners()
{
	createSeasonBanner("winter", "prog", winter_icon);
}

ScenarioMenu::ScenarioMenu(QWidget *parent, QDomNode conf) : ProgramMenu(parent, conf)
{
	summer_icon = bt_global::skin->getImage("summer_scenario");
	winter_icon = bt_global::skin->getImage("winter_scenario");
	season = SUMMER;
	createSummerBanners();
}

void ScenarioMenu::createSummerBanners()
{
	createSeasonBanner("summer", "scen", summer_icon);

}

void ScenarioMenu::createWinterBanners()
{
	createSeasonBanner("winter", "scen", winter_icon);
}
