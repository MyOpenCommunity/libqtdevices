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
#include "device_cache.h"
#include "plantmenu.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "bannfrecce.h"

#include <QRegExp>
#include <QDebug>

#define IMG_OK IMG_PATH "btnok.png"

static const QString i_right_arrow = QString("%1%2").arg(IMG_PATH).arg("arrrg.png");
static const QString i_temp_probe = QString("%1%2").arg(IMG_PATH).arg("zona.png");
static const QString i_ext_probe = QString("%1%2").arg(IMG_PATH).arg("sonda_esterna.png");
static const QString i_plant = QString("%1%2").arg(IMG_PATH).arg("impianto.png");

ThermalMenu::ThermalMenu(QDomNode config_node)
{
	qDebug("[TERMO] thermalmenu: before adding items...");
	bann_number = 0;
	loadBanners(config_node);

	qDebug("[TERMO] thermalmenu: end adding items.");
	// check if plant menus are created?
	if (bann_number == 1)
	{
		connect(single_submenu, SIGNAL(Closed()), SIGNAL(Closed()));
	}
}

void ThermalMenu::createPlantMenu(QDomNode config, bannPuls *bann)
{
	sottoMenu *sm = new PlantMenu(NULL, config);

	connect(bann, SIGNAL(sxClick()), sm, SLOT(show()));
	connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));
	single_submenu = sm;

	sm->hide();
	// hide children
	connect(this, SIGNAL(hideChildren()), sm, SLOT(hide()));
}

void ThermalMenu::loadBanners(QDomNode config_node)
{
	bannPuls *b = NULL;
	QDomNode node;
	foreach (node, getChildren(config_node, "plant"))
	{
		b = addMenuItem(node.toElement(), i_plant);
		createPlantMenu(node.toElement(), b);
	}
	foreach (node, getChildren(config_node, "extprobe"))
	{
		b = addMenuItem(node.toElement(), i_ext_probe);
		createProbeMenu(node.toElement(), b, true);
	}
	foreach (node, getChildren(config_node, "tempprobe"))
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

	elencoBanner.append(bp);
	connectLastBanner();

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

	/**
	 * Now submenus work. To add another submenu:
	 *  - make all the submenus children of the same parent (NULL), so that they are all siblings.
	 *  - create the submenu as usual, ie. make all banners children of the sottoMenu class.
	 *  - connect the clicked() signal with show() slot of the submenu to be shown.
	 *  - connect the Closed() signal of the submenu now created with its hide() slot.
	 */
	connect(bann, SIGNAL(sxClick()), sm, SLOT(show()));
	connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));
	sm->hide();
	// hide children
	connect(this, SIGNAL(hideChildren()), sm, SLOT(hide()));

	QDomNode n = config.firstChild();
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			QString addr = n.namedItem("where").toElement().text();
			if (external)
				addr += "00";
			device *dev = btouch_device_cache.get_temperature_probe(addr, external);

			banner *b = new BannTemperature(sm, n, dev);
			b->setText(getTextChild(n, "descr"));

			sm->appendBanner(b);
		}
		n = n.nextSibling();
	}
}

void ThermalMenu::showPage()
{
	if (bann_number == 1)
		single_submenu->showPage();
	else
		sottoMenu::showPage();
}

ProgramMenu::ProgramMenu(QWidget *parent, QDomNode conf) : sottoMenu(parent)
{
	conf_root = conf;
}

void ProgramMenu::setSeason(Season new_season)
{
	bool update = false;
	if (new_season != season)
	{
		season = new_season;
		update = true;
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

	if (update)
		forceDraw();
}

void ProgramMenu::createSeasonBanner(const QString season, const QString what, const QString icon)
{
	assert((what == "scen" || what == "prog") && "'what' must be either 'prog' or 'scen'");
	elencoBanner.clear();
	const QString i_ok = QString(IMG_PATH) + "btnok.png";

	if (conf_root.nodeName().contains(QRegExp("item(\\d\\d?)")) == 0)
	{
		qFatal("[TERMO] WeeklyMenu:wrong node in config file");
	}
	QDomElement program = getElement(conf_root, season + "/" + what);
	// The leaves we are looking for start with either "p" or "s"
	QString name = what.left(1);

	QDomNode node;
	foreach(node, getChildren(program, name))
	{
		BannWeekly *bp = new BannWeekly(this);
		bp->SetIcons(i_ok, QString(), icon);
		connect(bp, SIGNAL(programNumber(int)), this, SIGNAL(programClicked(int)));
		// set Text taken from conf.xml
		if (node.isElement())
		{
			bp->setText(node.toElement().text());
			// here node name is of the form "s12" or "p3"
			int program_number = node.nodeName().mid(1).toInt();
			bp->setProgram(program_number);
		}
		elencoBanner.append(bp);
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
