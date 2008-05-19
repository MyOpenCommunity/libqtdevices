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
#include "main.h"

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
				//elencoBanner.append(new ProbeBanner());
				qDebug("[TERMO] thermalmenu: add extprobe banner");
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
