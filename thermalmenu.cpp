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
#include "bannpuls.h"

#include <qregexp.h>

ThermalMenu::ThermalMenu(QWidget *parent, const char *name, QDomNode n) :
	sottoMenu(parent, name)
{
	qDebug("[TERMO] thermalmenu: before adding items...");
	this->root = n;
	addItems();
	qDebug("[TERMO] thermalmenu: end adding items.");
	// check if plant menus are created?
}

void ThermalMenu::addItems()
{
	createPlantMenu();
	addBanners();
}

void ThermalMenu::createPlantMenu()
{
	QDomNode n = this->root.firstChild();
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
	QDomNode n = this->root.firstChild();
	while (!n.isNull())
	{
		QDomElement e = n.toElement();
		if (!e.isNull())
		{
			if (e.tagName().contains(QRegExp("plant(\\d*)")))
			{
				// create plant banner
				//elencoBanner.append(new PlantBanner());
				qDebug("[TERMO] thermalmenu: add plant banner");
			}
			else if (e.tagName() == "extprobe")
			{
				// create extprobe banner
				qDebug("[TERMO] thermalmenu: add extprobe banner");
				char *descr = "extprobe";
				bannPuls *bp = new bannPuls(this, descr);

				QString leftIcon(IMG_PATH + QString("arrrg.png"));
				QString centralIcon(IMG_PATH + QString("termo/sonda_esterna.png"));

				bp->SetIcons(leftIcon.ascii(), 0, centralIcon.ascii());
				QString addr = getDeviceAddress(e);
				bp->setAddress(addr.ascii());
				elencoBanner.append(bp);

				this->connectLastBanner();
				// also create termopage
			}
			else if (e.tagName() == "tempprobe")
			{
				//create tempprobe banner
				//elencoBanner.append(new ProbeBanner());
				qDebug("[TERMO] thermalmenu: add extprobe banner");
				// also create termopage
			}
		}

		n = n.nextSibling();
	}
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
}

QString ThermalMenu::getDeviceAddress(QDomNode root)
{
	QDomNode n = findNamedNode(root, "where");
	QDomElement where = n.toElement();
	//FIXME: should we also check for `what'?
	//from conf.xml: used only by pagespecial and scenarios
	return where.text();
}
