/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "supervisionmenu.h"
#include "device.h"
#include "devices_cache.h" // bt_global::devices_cache
#include "xml_functions.h" // getChildren, getTextChild

#include <QDebug>

#include <assert.h>

#define STOPNGO_BANN_IMAGE ICON_STOPNGO_CHIUSO


SupervisionMenu::SupervisionMenu(const QDomNode &config_node)
{
	stopngoSubmenu = NULL;
	loadItems(config_node);
}

SupervisionMenu::~SupervisionMenu()
{
	while (!stopngoList.isEmpty())
		delete stopngoList.takeFirst();

	while (!stopngoPages.isEmpty())
		delete stopngoPages.takeFirst();

	if (this != stopngoSubmenu)
		delete stopngoSubmenu;
}

void SupervisionMenu::loadItems(const QDomNode &config_node)
{
	classesCount = 0;

	foreach (const QDomNode &node, getChildren(config_node, "class"))
	{
		int id = getTextChild(node, "id").toInt();
		switch (id)
		{
		case CLASS_STOPNGO:
		{
			bannPuls *b = new bannPuls(this);
			b->SetIcons(ICON_FRECCIA_DX, QString(), STOPNGO_BANN_IMAGE);
			b->setAddress(getTextChild(node, "where"));
			b->setText(getTextChild(node, "descr"));
			b->setId(id);
			appendBanner(b);
			CreateStopnGoMenu(node, b);
			++classesCount;
			break;
		}
		default:
			assert(!"Type of class not handled on supervision page!");
		}
	}

	if (classesCount == 1)  // Only one class has been defined in the supervision section of conf.xml
	{
		bannPuls* bann = static_cast<bannPuls*>(elencoBanner.last());
		connect(this, SIGNAL(quickOpen()), bann, SIGNAL(sxClick()));

		if (stopngoSubmenu)  // Check is the only submenu is a stopngo menu
		{
			if (this == stopngoSubmenu)  // Only one Stop&Go device is mapped
			{
				StopngoPage* pg = stopngoPages.first();
				if (pg)
				{
					disconnect(pg, SIGNAL(Closed()), this, SLOT(showFullScreen()));
					connect(pg, SIGNAL(Closed()), this, SIGNAL(Closed()));
				}
			}
			else  // A Stop&Go submenu instance has been created
			{
				disconnect(stopngoSubmenu, SIGNAL(Closed()), this, SLOT(showPage()));
				connect(stopngoSubmenu, SIGNAL(Closed()), this, SIGNAL(Closed()));
			}
		}
	}
}

void SupervisionMenu::CreateStopnGoMenu(QDomNode node, bannPuls *bann)
{
	foreach (const QDomNode &item, getChildren(node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		int cid = getTextChild(item, "cid").toInt();
		QString descr = getTextChild(item, "descr");
		QString where = getTextChild(item, "where");
		stopngoList.append(new StopngoItem(id, cid, descr, where));
	}

	if (stopngoList.size() > 1)  // more than one device
	{
		// Show a submenu listing the devices to control
		stopngoSubmenu = new sottoMenu;
		connect(bann, SIGNAL(sxClick()), stopngoSubmenu, SLOT(showPage()));  // Connect submenu
		connect(stopngoSubmenu, SIGNAL(Closed()), this, SLOT(showPage()));

		for (int i = 0; i < stopngoList.size(); ++i)
		{
			StopngoItem *itm = stopngoList.at(i);
			BannPulsDynIcon *bp = new BannPulsDynIcon(stopngoSubmenu);  // Create a new banner
			bp->SetIcons(ICON_FRECCIA_DX, 0, ICON_STOPNGO_CHIUSO);
			bp->setText(itm->GetDescr());
			bp->setAnimationParams(0, 0);
			bp->setId(itm->GetId());

			// Get status changed events back
			mci_device* dev = (mci_device*)bt_global::devices_cache.get_mci_device(itm->GetWhere());
			connect(dev, SIGNAL(status_changed(QList<device_status*>)), bp, SLOT(status_changed(QList<device_status*>)));
			stopngoSubmenu->appendBanner(bp);  // Add the new banner to the submenu
			LinkBanner2Page(bp, itm);  // Connect the new banner to the page
		}
		stopngoSubmenu->forceDraw();
	}
	else if (stopngoList.size() == 1)  // one device
	{
		// directly open the only available device page
		stopngoSubmenu = this;
		LinkBanner2Page(bann, stopngoList.at(0));
	}
}

void SupervisionMenu::LinkBanner2Page(bannPuls* bnr, StopngoItem* itm)
{
	StopngoPage* pg = new StopngoPage(itm->GetWhere(), itm->GetId(), itm->GetDescr());

	connect(bnr, SIGNAL(sxClick()), pg, SLOT(showPage()));
	connect(pg, SIGNAL(Closed()), stopngoSubmenu, SLOT(showPage()));

	// Get status changed events back
	mci_device* dev = (mci_device*)bt_global::devices_cache.get_mci_device(itm->GetWhere());
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			pg, SLOT(status_changed(QList<device_status*>)));

	stopngoPages.append(pg);
}

void SupervisionMenu::showPage()
{
	qDebug("SupervisionMenu::showPage()");
	if (classesCount == 1)
		emit quickOpen();
	else
		sottoMenu::showPage();
}
