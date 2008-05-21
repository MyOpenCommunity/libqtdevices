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

#include <qregexp.h>

#define I_EXT_PROBE                  "sonda_esterna.png"
#define I_RIGHT_ARROW                "arrrg.png"
#define I_PLANT                      "impianto.png"

ThermalMenu::ThermalMenu(QWidget *parent, const char *name, QDomNode n, QColor bg, QColor fg) :
	sottoMenu(parent, name)
{
	qDebug("[TERMO] thermalmenu: before adding items...");
	subtreeRoot = n;
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
	QDomNode n = subtreeRoot.firstChild();
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
	QDomNode node = subtreeRoot.firstChild();
	while (!node.isNull())
	{
		QDomElement e = node.toElement();
		if (!e.isNull())
		{
			if (e.tagName().contains(QRegExp("plant(\\d*)")))
			{
				// create plant banner
				create2ButBanner(e, I_PLANT, e.text());
			}
			else if (e.tagName() == "extprobe")
			{
				// create extprobe banner
				create2ButBanner(e, I_EXT_PROBE, "extprobe");
				// also create termopage
				bannPuls* b = static_cast<bannPuls*>(elencoBanner.getLast());
				createProbeMenu(e, b);
			}
			else if (e.tagName() == "tempprobe")
			{
				//create tempprobe banner
				create2ButBanner(e, I_EXT_PROBE, "tempprobe");
				// also create termopage
			}
		}

		node = node.nextSibling();
	}
}

void ThermalMenu::createProbeMenu(QDomNode node, bannPuls *bann)
{
	sottoMenu *sm = new sottoMenu(this, "sottomenu extprobe");
	sm->setBGColor(paletteBackgroundColor());
	sm->setFGColor(paletteForegroundColor());
	QObject::connect(bann, SIGNAL(sxClick()), sm, SLOT(showFullScreen()));
	QObject::connect(sm, SIGNAL(Closed()), this, SLOT(showFullScreen()));
	QObject::connect(sm,SIGNAL(Closed()),sm,SLOT(hide()));
	sm->forceDraw();
	//how do we create connections with the rest of the application??????
	
	QDomNode n = node;
	while (!n.isNull())
	{
		if (n.nodeName().contains(QRegExp("item(\\d\\d?)")))
		{
			/*banner *bp = new banner(sm, "banner");
			bp->SetTextU(n.toElement().text());
			bp->setAnimationParams(0, 0);
			bp->setBGColor(paletteBackgroundColor());
			bp->setFGColor(paletteForegroundColor());
			QDomNode id = findNamedNode(n, "id");
			bp->setId(id.toElement().text().toInt());

			sm->appendBanner(bp);*/
		}
		n = n.nextSibling();
	}
}

void ThermalMenu::create2ButBanner(QDomElement e, QString ci, QString descr)
{	
	bannPuls *bp = new bannPuls(this, descr.ascii());
	QString leftIcon(IMG_PATH + QString(I_RIGHT_ARROW));
	QString centralIcon(IMG_PATH + ci);

	bp->SetIcons(leftIcon.ascii(), 0, centralIcon.ascii());
	QString addr = getDeviceAddress(e);
	bp->setAddress(addr.ascii());
	elencoBanner.append(bp);

	connectLastBanner();
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
}

QDomNode ThermalMenu::findNamedNode(QDomNode root, QString name)
{
	QValueList<QDomNode> nodes;
	nodes.append(root);
	while (!nodes.isEmpty())
	{
		QDomNode n = nodes.first();
		QDomNode item = n.namedItem(name);
		qDebug("[TERMO] findNamedNode: item = %s", item.nodeName().ascii());
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
	throw ;
}

QString ThermalMenu::getDeviceAddress(QDomNode root)
{
	QDomNode n = findNamedNode(root, "where");
	QDomElement where = n.toElement();
	//FIXME: should we also check for `what'?
	//from conf.xml: used only by pagespecial and scenarios
	return where.text();
}
