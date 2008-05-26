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
#include "bannextprobe.h"
#include "bannsingleprobe.h"

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
	addItems();
	qDebug("[TERMO] thermalmenu: end adding items.");
	// check if plant menus are created?
}

void ThermalMenu::addItems()
{
	//createPlantMenu();
	addBanners();
}

void ThermalMenu::createPlantMenu()
{
	QDomNode n = conf_root.firstChild();
	while (!n.isNull())
	{
		if (n.isElement() && n.nodeName().contains(QRegExp("plant(\\d*)")))
		{
			//PlantMenu pm = new PlantMenu(this, n.nodeName().ascii(), n);
			qDebug("[TERMO] ThermalMenu: create new PlantMenu");
		}
		n = n.nextSibling();
	}
}

void ThermalMenu::addBanners()
{
	sottoMenu *sm = NULL;
	bannPuls *b = NULL;

	QDomNode node = conf_root.firstChild();
	while (!node.isNull())
	{
		QDomElement e = node.toElement();
		if (!e.isNull())
		{
			if (e.tagName().contains(QRegExp("plant(\\d*)")))
			{
				// create plant banner
				QString descr = findNamedNode(e, "descr").toElement().text();
				addMenuItem(e, I_PLANT, descr);
			}
			else if (e.tagName() == "extprobe")
			{
				// create extprobe banner
				b = addMenuItem(e, I_EXT_PROBE, "extprobe");
				// also create termopage
				sm = createProbeMenu(e, b);
			}
			else if (e.tagName() == "tempprobe")
			{
				//create tempprobe banner
				addMenuItem(e, I_TEMP_PROBE, "tempprobe");
				// also create termopage
			}
		}

		node = node.nextSibling();
	}

	if (sm)
		sm->forceDraw();
#if 0
	if (b)
	{
		qDebug("[TERMO] ci sono!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		sottoMenu *s = new sottoMenu(0, "sottomenu extprobe");
		s->setBGColor(paletteBackgroundColor());
		s->setFGColor(paletteForegroundColor());
		QObject::connect(b, SIGNAL(sxClick()), s, SLOT(showFullScreen()));
		QObject::connect(b, SIGNAL(sxClick()), s, SLOT(show()));
		QObject::connect(b, SIGNAL(sxClick()), this, SLOT(hide()));

		QObject::connect(s, SIGNAL(Closed()), this, SLOT(showFullScreen()));
		QObject::connect(s, SIGNAL(Closed()), s, SLOT(hide()));
	}
#endif
}

sottoMenu *ThermalMenu::createProbeMenu(QDomNode config, bannPuls *bann)
{
	sottoMenu *sm = new sottoMenu(this, "sottomenu extprobe");
	sm->setBGColor(paletteBackgroundColor());
	sm->setFGColor(paletteForegroundColor());
	sm->show();
	QObject::connect(bann, SIGNAL(sxClick()), sm, SLOT(showFullScreen()));
	QObject::connect(sm, SIGNAL(Closed()), this, SLOT(showFullScreen()));
	QObject::connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));
	//how do we create connections with the rest of the application??????
	QDomNode n = config.firstChild();
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			banner *b = new BannExtProbe(sm, "banner", n);
			b->SetTextU(n.toElement().text());
			b->setAnimationParams(0, 0);
			b->setBGColor(paletteBackgroundColor());
			b->setFGColor(paletteForegroundColor());
			QDomNode id = findNamedNode(n, "id");
			b->setId(id.toElement().text().toInt());

			sm->appendBanner(b);
		}
		n = n.nextSibling();
	}

	return sm;
}

bannPuls *ThermalMenu::addMenuItem(QDomElement e, QString central_icon, QString descr)
{	
	bannPuls *bp = new bannPuls(this, descr.ascii());
	qDebug("[TERMO] addBanners1: %s", descr.ascii());
	item_list.append(bp);
	QString leftIcon(IMG_PATH + QString(I_RIGHT_ARROW));
	central_icon = QString(IMG_PATH) + central_icon;
	bp->SetIcons(leftIcon.ascii(), 0, central_icon.ascii());

	QString addr = getDeviceAddress(e);
	//qDebug("[TERMO] addBanners2: %s", addr.ascii());

	bp->setAddress(addr.ascii());
	elencoBanner.append(bp);

	connectLastBanner();
	// boh?
	//connect(this, SIGNAL(hideChildren()), elencoBanner.getLast(), SLOT(hide()));

	QDomNode n = findNamedNode(e, "descr");
	elencoBanner.getLast()->SetTextU(n.toElement().text());
	// we are ignoring animationParams
	elencoBanner.getLast()->setAnimationParams(0, 0);

	elencoBanner.getLast()->setBGColor(paletteBackgroundColor());
	elencoBanner.getLast()->setFGColor(paletteForegroundColor());
	n = findNamedNode(e, "id");
	elencoBanner.getLast()->setId(n.toElement().text().toInt());
	// note: we are ignoring the serial number...
	// seems not used for thermal regulation
	return bp;
}

QDomNode ThermalMenu::findNamedNode(QDomNode root, QString name)
{
	QValueList<QDomNode> nodes;
	nodes.append(root);
	while (!nodes.isEmpty())
	{
		QDomNode n = nodes.first();
		QDomNode item = n.namedItem(name);
		//qDebug("[TERMO] findNamedNode: item = %s", item.nodeName().ascii());
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
	throw;
}

QString ThermalMenu::getDeviceAddress(QDomNode root)
{
	QDomNode n = findNamedNode(root, "where");
	QDomElement where = n.toElement();
	//FIXME: should we also check for `what'?
	//from conf.xml: used only by pagespecial and scenarios
	qDebug("[TERMO] getDeviceAddress: where = %s", where.text().ascii());
	return where.text();
}

void ThermalMenu::hideEvent(QHideEvent *e)
{
	qDebug("[TERMO] hideEvent received");
	QPtrListIterator<banner> it(item_list);
	banner *bann;
	while ((bann = it.current()) != 0)
	{
		++it;
		bann->hide();
	}
}
