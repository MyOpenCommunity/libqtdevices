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

#include <QRegExp>
#include <QDebug>

static const QString i_right_arrow = QString("%1%2").arg(IMG_PATH).arg("arrrg.png");
static const QString i_temp_probe = QString("%1%2").arg(IMG_PATH).arg("zona.png");
static const QString i_ext_probe = QString("%1%2").arg(IMG_PATH).arg("sonda_esterna.png");
static const QString i_plant = QString("%1%2").arg(IMG_PATH).arg("impianto.png");

ThermalMenu::ThermalMenu(QWidget *parent, const char *name, QDomNode n) : sottoMenu(parent, name)
{
	qDebug("[TERMO] thermalmenu: before adding items...");
	conf_root = n;
	bann_number = 0;
	addBanners();

	qDebug("[TERMO] thermalmenu: end adding items.");
	// check if plant menus are created?
	if (bann_number == 1)
	{
		connect(single_submenu, SIGNAL(Closed()), SIGNAL(Closed()));
	}
}

void ThermalMenu::createPlantMenu(QDomNode config, bannPuls *bann)
{
	sottoMenu *sm = new PlantMenu(NULL, "plant menu", config);

	connect(bann, SIGNAL(sxClick()), sm, SLOT(show()));
	connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));
	single_submenu = sm;

	sm->hide();
	// hide children
	connect(this, SIGNAL(hideChildren()), sm, SLOT(hide()));
}

void ThermalMenu::addBanners()
{
	bannPuls *b = NULL;

	QDomNode node = conf_root.firstChild();
	while (!node.isNull())
	{
		QDomElement e = node.toElement();
		if (!e.isNull())
		{
			if (e.tagName().contains(QRegExp("plant(\\d*)")))
			{
				QString descr = findNamedNode(e, "descr").toElement().text();
				b = addMenuItem(e, i_plant, descr);
				createPlantMenu(e, b);
			}
			else if (e.tagName() == "extprobe")
			{
				b = addMenuItem(e, i_ext_probe, "extprobe");
				createProbeMenu(e, b, true);
			}
			else if (e.tagName() == "tempprobe")
			{
				b = addMenuItem(e, i_temp_probe, "tempprobe");
				createProbeMenu(e, b, false);
			}
		}

		node = node.nextSibling();
	}
}

bannPuls *ThermalMenu::addMenuItem(QDomElement e, QString central_icon, QString descr)
{
	bannPuls *bp = new bannPuls(this, 0);
	qDebug() << "[TERMO] addBanners1: " << descr;

	bp->SetIcons(i_right_arrow, QString(), central_icon);
	initBanner(bp, e);

	elencoBanner.append(bp);
	connectLastBanner();

	++bann_number;
	return bp;
}

void ThermalMenu::createProbeMenu(QDomNode config, bannPuls *bann, bool external)
{
	sottoMenu *sm = new sottoMenu(NULL, "sottomenu extprobe");
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

			banner *b = new BannTemperature(sm, "banner", n, dev);
			initBanner(b, n);

			sm->appendBanner(b);
		}
		n = n.nextSibling();
	}
}

void ThermalMenu::showPage()
{
	if (bann_number == 1)
		single_submenu->show();
	else
		show();
}
