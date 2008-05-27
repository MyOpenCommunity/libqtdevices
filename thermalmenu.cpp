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

#include <qregexp.h>

#define I_EXT_PROBE                  "sonda_esterna.png"
#define I_TEMP_PROBE                 "zona.png"
#define I_RIGHT_ARROW                "arrrg.png"
#define I_PLANT                      "impianto.png"

ThermalMenu::ThermalMenu(QWidget *parent, const char *name, QDomNode n, QColor bg, QColor fg) :
	sottoMenu(parent, name)
{
	qDebug("[TERMO] thermalmenu: before adding items...");
	conf_root = n;
	setBGColor(bg);
	setFGColor(fg);
	addBanners();

	qDebug("[TERMO] thermalmenu: end adding items.");
	// check if plant menus are created?
}

void ThermalMenu::addItems()
{
}

void ThermalMenu::createPlantMenu(QDomNode config, bannPuls *bann)
{
	sottoMenu *sm = new sottoMenu(this, "sottomenu extprobe");
	sm->setBGColor(paletteBackgroundColor());
	sm->setFGColor(paletteForegroundColor());
	//sm->show();
	QObject::connect(bann, SIGNAL(sxClick()), sm, SLOT(showFullScreen()));
	QObject::connect(sm, SIGNAL(Closed()), this, SLOT(showFullScreen()));
	QObject::connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));

	QDomNode n = config.firstChild();
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			//QDomNode item = findNamedNode(n, "descr");
			//QString descr = item.toElement().text();
			banner *b = new bannPuls(sm, "");
			qDebug("[TERMO] createPlantMenu: item = %s", n.nodeName().ascii());

			initBanner(b, n);

			QString leftIcon(IMG_PATH + QString(I_RIGHT_ARROW));
			QString central_icon;
			int id = n.namedItem("id").toElement().text().toInt();
			if (id == TERMO || id == TERMO_FANCOIL)
				central_icon = QString(IMG_PATH) + I_TEMP_PROBE;
			else
				central_icon = QString(IMG_PATH) + I_PLANT;
			b->SetIcons(leftIcon.ascii(), 0, central_icon.ascii());


			sm->appendBanner(b);
		}
		n = n.nextSibling();
	}
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
	item_list.append(bp);

	QString leftIcon(IMG_PATH + QString(I_RIGHT_ARROW));
	central_icon = QString(IMG_PATH) + central_icon;
	bp->SetIcons(leftIcon.ascii(), 0, central_icon.ascii());

	initBanner(bp, e);

	elencoBanner.append(bp);
	connectLastBanner();
	// boh?
	//connect(this, SIGNAL(hideChildren()), elencoBanner.getLast(), SLOT(hide()));

	return bp;
}

void ThermalMenu::createProbeMenu(QDomNode config, bannPuls *bann, bool external)
{
	sottoMenu *sm = new sottoMenu(this, "sottomenu extprobe");
	sm->setBGColor(paletteBackgroundColor());
	sm->setFGColor(paletteForegroundColor());
	//sm->show();
	QObject::connect(bann, SIGNAL(sxClick()), sm, SLOT(showFullScreen()));
	QObject::connect(sm, SIGNAL(Closed()), this, SLOT(showFullScreen()));
	QObject::connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));
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
}

void ThermalMenu::initBanner(banner *bann, QDomNode conf)
{
	bann->setBGColor(paletteBackgroundColor());
	bann->setFGColor(paletteForegroundColor());

	//QString addr = getDeviceAddress(conf);
	//bann->setAddress(addr.ascii());
	qDebug("[TERMO] initBanner: remeber to set the address of the device (if applicable)");

	QDomNode n = findNamedNode(conf, "descr");
	bann->SetTextU(n.toElement().text());

	n = findNamedNode(conf, "id");
	bann->setId(n.toElement().text().toInt());

	bann->setAnimationParams(0, 0);
	// note: we are ignoring the serial number...
	// seems not used for thermal regulation
}

QDomNode ThermalMenu::findNamedNode(QDomNode root, QString name)
{
	QValueList<QDomNode> nodes;
	nodes.append(root);
	while (!nodes.isEmpty())
	{
		QDomNode n = nodes.first();
		QDomNode item = n.namedItem(name);
		if (item.isNull())
		{
			QDomNodeList list = n.childNodes();
			for (unsigned i = 0; i < list.length(); ++i)
				nodes.append(list.item(i));
			nodes.pop_front();
		}
		else
		{
			return item;
		}
	}
	QDomNode null;
	return null;
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
