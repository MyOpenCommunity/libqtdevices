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

#include <qregexp.h>

static const QString i_right_arrow = QString("%1%2").arg(IMG_PATH).arg("arrrg.png");
static const QString i_temp_probe = QString("%1%2").arg(IMG_PATH).arg("zona.png");
static const QString i_ext_probe = QString("%1%2").arg(IMG_PATH).arg("sonda_esterna.png");
static const QString i_plant = QString("%1%2").arg(IMG_PATH).arg("impianto.png");

ThermalMenu::ThermalMenu(QWidget *parent, const char *name, QDomNode n, QColor bg, QColor fg, QColor fg2) :
	sottoMenu(parent, name)
{
	qDebug("[TERMO] thermalmenu: before adding items...");
	conf_root = n;
	setBGColor(bg);
	setFGColor(fg);
	second_fg = fg2;
	addBanners();

	qDebug("[TERMO] thermalmenu: end adding items.");
	// check if plant menus are created?
}

void ThermalMenu::addItems()
{
}

void ThermalMenu::createPlantMenu(QDomNode config, bannPuls *bann)
{
	sottoMenu *sm = new PlantMenu(parentWidget(), "plant menu", config,
			paletteBackgroundColor(), paletteForegroundColor(), second_fg);
	connect(bann, SIGNAL(sxClick()), sm, SLOT(show()));
	connect(bann, SIGNAL(sxClick()), sm, SLOT(raise()));
	connect(bann, SIGNAL(sxClick()), this, SLOT(hide()));

	connect(sm, SIGNAL(Closed()), this, SLOT(show()));
	connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));
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
	bannPuls *bp = new bannPuls(this, descr.ascii());
	qDebug("[TERMO] addBanners1: %s", descr.ascii());

	bp->SetIcons(i_right_arrow.ascii(), 0, central_icon.ascii());

	initBanner(bp, e);

	elencoBanner.append(bp);
	connectLastBanner();

	return bp;
}

void ThermalMenu::createProbeMenu(QDomNode config, bannPuls *bann, bool external)
{
	sottoMenu *sm = new sottoMenu(parentWidget(), "sottomenu extprobe");
	sm->setBGColor(paletteBackgroundColor());
	sm->setFGColor(paletteForegroundColor());
	/**
	 * Now submenus work. To fix this:
	 *  - make all the submenus children of the same parent (home page), so that they are all siblings.
	 *  - create the submenu as usual, ie. make all banners children of the sottoMenu class.
	 *  - connect the clicked() signal with show() and raise() slots of the submenu to be shown,
	 *      and to the hide() slot of the submenu that is now showing.
	 *  - connect the Closed() signal of the submenu now created with its hide() slot and the show() 
	 *      slot of the submenu that is creating it
	 */
	connect(bann, SIGNAL(sxClick()), sm, SLOT(show()));
	connect(bann, SIGNAL(sxClick()), sm, SLOT(raise()));
	connect(bann, SIGNAL(sxClick()), this, SLOT(hide()));

	connect(sm, SIGNAL(Closed()), this, SLOT(show()));
	connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));

	QDomNode n = config.firstChild();
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			device *dev;
			QString addr = n.namedItem("where").toElement().text();
			if (external)
				dev = btouch_device_cache.get_temperature_probe(addr.ascii(), external);
			else
				dev = btouch_device_cache.get_temperature_probe(addr.ascii(), external);


			banner *b = new BannTemperature(sm, "banner", n, dev);
			initBanner(b, n);

			sm->appendBanner(b);
		}
		n = n.nextSibling();
	}
	//sm->show();
}

QString ThermalMenu::getDeviceAddress(QDomNode root)
{
	QString where = "";
	QDomNode n = findNamedNode(root, "where");
	if (!n.isNull())
	{
		where = n.toElement().text();
	}
	else
	{
		qDebug("[TERMO] getDeviceAddress: `where' not found, defaulting to `\"0\"' ");
		where = "0";
	}
	//FIXME: should we also check for `what'?
	//from conf.xml: used only by pagespecial and scenarios
	return where;
}
