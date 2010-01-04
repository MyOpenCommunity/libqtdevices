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
#include "probe_device.h"
#include "plantmenu.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "bannercontent.h"
#include "skinmanager.h"
#include "airconditioning.h"

#include <QRegExp>
#include <QDebug>

enum
{
	PAGE_PLANT_MENU_4ZONES = 501,
	PAGE_PLANT_MENU_99ZONES = 500,
	PAGE_EXTERNAL_PROBES = 530,
	PAGE_INTERNAL_PROBES = 540
};

ThermalMenu::ThermalMenu(const QDomNode &config_node)
{
	bann_number = 0;
	buildPage(getTextChild(config_node, "descr"));
	loadBanners(config_node);

	// check if plant menus are created?
	if (bann_number == 1)
		connect(single_submenu, SIGNAL(Closed()), SIGNAL(Closed()));
}

int ThermalMenu::sectionId()
{
	return TERMOREGOLAZIONE;
}

void ThermalMenu::createPlantMenu(QDomNode config, BannSinglePuls *bann)
{
	Page *sm = new PlantMenu(config);
	bann->connectRightButton(sm);
	connect(bann, SIGNAL(pageClosed()), SLOT(showPage()));
	single_submenu = sm;
}

#ifdef CONFIG_BTOUCH

void ThermalMenu::loadBanners(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
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
		b->connectRightButton(new AirConditioning(air_node));
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

		int page_id = getTextChild(node, "lnk_pageID").toInt();
		QDomNode page_node = getPageNodeFromPageId(page_id);
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
		default:
			qFatal("Unhandled id in ThermalMenu::loadBanners");
		}
		single_submenu = p;

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

void ThermalMenu::createProbeMenu(QDomNode config, BannSinglePuls *bann, bool external)
{
	ProbesPage *sm = new ProbesPage(config, external);
	single_submenu = sm;

	bann->connectRightButton(sm);
	connect(bann, SIGNAL(pageClosed()), SLOT(showPage()));
}

void ThermalMenu::showPage()
{
	if (bann_number == 1)
		single_submenu->showPage();
	else
		Page::showPage();
}


ProbesPage::ProbesPage(const QDomNode &config_node, bool are_probes_external)
{
	buildPage();
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		QString addr = getTextChild(item, "where");
		QString text = getTextChild(item, "descr");
		if (are_probes_external)
			addr += "00";
		NonControlledProbeDevice *dev = static_cast<NonControlledProbeDevice *>(
				bt_global::devices_cache.get_temperature_probe(addr, are_probes_external));

		banner *b = new BannTemperature(this, addr, text, dev);

		page_content->appendBanner(b);
	}
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
	int id = getTextChild(conf_root, "id").toInt();

	int index = 0;
	foreach (const QDomNode &node, getChildren(program, name))
	{
		BannWeekly *bp = 0;
		if (create_banner)
		{
			bp = new BannWeekly(this);
			// setting the banner ID, appendBanner() sets the banner serNum
			// that is implicitly used to select the program number when the
			// user chooses the program/scenario
			bp->setId(id);
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
		QString text;
		if (node.isElement())
			text = node.toElement().text();
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
