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

#include <QRegExp>
#include <QDebug>

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
