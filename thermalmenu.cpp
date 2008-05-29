/*!
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief  A class to handle thermal regulation menu
 *
 *  TODO: detailed description (optional) 
 *
 * \author Luca Ottaviano
 */

#include "thermalmenu.h"
#include "banntemperature.h"
#include "device_cache.h"
#include "plantmenu.h"

#include <qregexp.h>

#define I_EXT_PROBE                  "sonda_esterna.png"
#define I_TEMP_PROBE                 "zona.png"
#define I_RIGHT_ARROW                "arrrg.png"
#define I_PLANT                      "impianto.png"

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
	sm->show();
	QObject::connect(bann, SIGNAL(sxClick()), sm, SLOT(raise()));
	QObject::connect(sm, SIGNAL(Closed()), this, SLOT(raise()));
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
				b = addMenuItem(e, I_PLANT, descr);
				createPlantMenu(e, b);
			}
			else if (e.tagName() == "extprobe")
			{
				b = addMenuItem(e, I_EXT_PROBE, "extprobe");
				createProbeMenu(e, b, true);
			}
			else if (e.tagName() == "tempprobe")
			{
				b = addMenuItem(e, I_TEMP_PROBE, "tempprobe");
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

	QString leftIcon(IMG_PATH + QString(I_RIGHT_ARROW));
	central_icon = QString(IMG_PATH) + central_icon;
	bp->SetIcons(leftIcon.ascii(), 0, central_icon.ascii());

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
	 *  - after creating the submenu, use the show() method on it,
	 *       ie:
	 *         sottoMenu *sm = new sottoMenu(...);
	 *         // create sm children
	 *         // ...
	 *         sm->show();
	 *  - connect the relevant signal with the raise() slot of the menu that must be on top
	 */
	connect(bann, SIGNAL(sxClick()), sm, SLOT(raise()));
	connect(sm, SIGNAL(Closed()), this, SLOT(raise()));

	QDomNode n = config.firstChild();
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			device *dev;
			QString addr = n.namedItem("addr").toElement().text();
			if (external)
				dev = btouch_device_cache.get_temperature_probe(addr.ascii(), true);
			else
				dev = btouch_device_cache.get_temperature_probe(addr.ascii(), false);


			banner *b = new BannTemperature(sm, "banner", n, dev);
			initBanner(b, n);

			sm->appendBanner(b);
		}
		n = n.nextSibling();
	}
	sm->show();
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
