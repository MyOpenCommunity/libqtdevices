/*!
 * \file
 * <!--
 * Copyright 2008 MAC S.r.l. (http://www.mac-italia.com/)
 * All rights reserved.
 * -->
 *
 * \brief  Supervision main menu handling class
 *
 *  TODO: detailed description (optional) 
 *
 * \author Lucio Macellari
 */

#include "supervisionmenu.h"
#include "device_cache.h"
#include "device.h"

#include <QDebug>

#define STOPNGO_BANN_IMAGE ICON_STOPNGO_CHIUSO

/*==================================================================================================
	SupervisionMenu class definition
==================================================================================================*/

SupervisionMenu::SupervisionMenu(QWidget *parent, const char *name, QDomNode n, QColor bg, QColor fg) :
	sottoMenu(parent, name)
{
	qDebug("[SUPERVISION] SupervisionMenu()");
	subtreeRoot = n;
	stopngoSubmenu = NULL;
	setBGColor(bg);
	setFGColor(fg);
	AddItems();
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

void SupervisionMenu::AddItems()
{
	AddBanners();
}

void SupervisionMenu::AddBanners()
{
	classesCount = 0;
	QDomNode level1Node = subtreeRoot.firstChild();
	while (!level1Node.isNull())
	{
		QDomElement l1Element = level1Node.toElement();
		if (!l1Element.isNull() && l1Element.tagName().startsWith("class"))
		{
			// Now look for the ID subnode
			QDomNode n = FindNamedNode(l1Element, "id");
			switch (n.toElement().text().toInt())
			{
			case CLASS_STOPNGO:
				// Create a banner for the class
				Create2ButBanner(l1Element, STOPNGO_BANN_IMAGE, "StopnGo banner");
				// Create the submenu/page related to this banner
				bannPuls* b = static_cast<bannPuls*>(elencoBanner.getLast());
				CreateStopnGoMenu(l1Element, b);
				break;
			}
			classesCount++;
		}
		level1Node = level1Node.nextSibling();
	}

	if (classesCount == 1)  // Only one class has been defined in the supervision section of conf.xml
	{
		bannPuls* bann = static_cast<bannPuls*>(elencoBanner.getLast());
		connect(this, SIGNAL(quickOpen()), bann, SIGNAL(sxClick()));

		if (stopngoSubmenu)  // Check is the only submenu is a stopngo menu
		{
			if (this == stopngoSubmenu)  // Only one Stop&Go devise is mapped
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
				disconnect(stopngoSubmenu, SIGNAL(Closed()), this, SLOT(showFullScreen()));
				connect(stopngoSubmenu, SIGNAL(Closed()), this, SIGNAL(Closed()));
			}
		}
	}
}

void SupervisionMenu::Create2ButBanner(QDomElement e, QString ci, QString descr)
{
	bannPuls *bp = new bannPuls(this, 0);

	QByteArray buf = ci.toAscii();
	bp->SetIcons(ICON_FRECCIA_DX, 0, buf.constData());
	QString addr = GetDeviceAddress(e);
	bp->setAddress(addr);
	elencoBanner.append(bp);
	connectLastBanner();

	QDomNode n = FindNamedNode(e, "descr");
	elencoBanner.getLast()->SetTextU(n.toElement().text());
	elencoBanner.getLast()->setAnimationParams(0, 0);

	elencoBanner.getLast()->setBGColor(paletteBackgroundColor());
	elencoBanner.getLast()->setFGColor(paletteForegroundColor());
	n = FindNamedNode(e, "id");
	elencoBanner.getLast()->setId(n.toElement().text().toInt());
}

QDomNode SupervisionMenu::FindNamedNode(QDomNode root, QString name)
{
	QList<QDomNode> nodes;
	nodes.append(root);
	while (!nodes.isEmpty())
	{
		QDomNode n = nodes.first();
		QDomNode item = n.namedItem(name);
		qDebug() << "[SUPERVISOR] FindNamedNode: item = " << item.nodeName();
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

QString SupervisionMenu::GetDeviceAddress(QDomNode root)
{
	QDomNode n = FindNamedNode(root, "where");
	QDomElement where = n.toElement();
	return where.text();
}

void SupervisionMenu::CreateStopnGoMenu(QDomNode node, bannPuls *bann)
{
	qDebug("[SUPERVISOR] CreateStopnGoMenu()");
	
	QDomNode n = node.firstChild();
	while (!n.isNull())  // Scan XML subtree to build a list of device items
	{
		if (n.nodeName().startsWith("item"))
		{
			QDomNode id = FindNamedNode(n, "id");
			QDomNode cid = FindNamedNode(n, "cid");
			QDomNode des   = FindNamedNode(n, "descr");
			QDomNode where = FindNamedNode(n, "where");
			stopngoList.append(new StopngoItem(id.toElement().text().toInt(), cid.toElement().text().toInt(),
				des.toElement().text(), where.toElement().text()));

			qDebug() << "[SUPERVISOR] CreateStopnGoMenu() found: " << n.nodeName() << " desc="
				<< des.toElement().text() << " id=" << id.toElement().text();
		}
		n = n.nextSibling();
	}

	if (stopngoList.size() > 1)  // more than one device
	{
		// Show a submenu listing the devices to control
		sottoMenu *sm = new sottoMenu(NULL, "StopnGo Group");  // Create submenu
		sm->hide();
		sm->setBGColor(paletteBackgroundColor());
		sm->setFGColor(paletteForegroundColor());
		QObject::connect(bann, SIGNAL(sxClick()), sm, SLOT(showFullScreen()));  // Connect submenu
		QObject::connect(this, SIGNAL(hideChildren()), sm, SLOT(hide()));
		QObject::connect(sm, SIGNAL(Closed()), this, SLOT(showFullScreen()));
		QObject::connect(sm, SIGNAL(Closed()), sm, SLOT(hide()));
		QObject::connect(this, SIGNAL(frez(bool)), sm, SLOT(freezed(bool)));
		stopngoSubmenu = sm;

		for (int i = 0; i < stopngoList.size(); ++i)
		{
			StopngoItem *itm = stopngoList.at(i);
			BannPulsDynIcon *bp = new BannPulsDynIcon(sm, "banner");  // Create a new banner
			bp->SetIcons(ICON_FRECCIA_DX, 0, ICON_STOPNGO_CHIUSO);
			bp->SetTextU(itm->GetDescr());
			bp->setAnimationParams(0, 0);
			bp->setBGColor(paletteBackgroundColor());
			bp->setFGColor(paletteForegroundColor());
			bp->setId(itm->GetId());

			// Get status changed events back
			mci_device* dev = (mci_device*)btouch_device_cache.get_mci_device(itm->GetWhere());
			connect(dev, SIGNAL(status_changed(QList<device_status*>)), bp, SLOT(status_changed(QList<device_status*>)));
			sm->appendBanner(bp);  // Add the new banner to the submenu
			LinkBanner2Page(bp, itm);  // Connect the new banner to the page
		}
		sm->forceDraw();
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
	StopngoPage* pg = new StopngoPage(NULL, "stopngo", itm->GetWhere(), itm->GetId(), itm->GetDescr());
	pg->hide();
	pg->setBGColor(paletteBackgroundColor());
	pg->setFGColor(paletteForegroundColor());
	
	connect(bnr, SIGNAL(sxClick()), pg, SLOT(showPage()));
	connect(stopngoSubmenu, SIGNAL(hideChildren()), pg, SLOT(hide()));
	connect(pg, SIGNAL(Closed()), stopngoSubmenu, SLOT(showFullScreen()));
	connect(pg, SIGNAL(Closed()), pg, SLOT(hide()));
	connect(stopngoSubmenu, SIGNAL(frez(bool)), pg, SLOT(freezed(bool)));
	connect(pg, SIGNAL(sendFrame(char*)), this, SIGNAL(sendFrame(char*)));
	
	// Get status changed events back
	mci_device* dev = (mci_device*)btouch_device_cache.get_mci_device(itm->GetWhere());
	connect(dev, SIGNAL(status_changed(QList<device_status*>)),
			pg, SLOT(status_changed(QList<device_status*>)));

	stopngoPages.append(pg);
}

void SupervisionMenu::showPg()
{
	qDebug("SupervisionMenu::showPg()");
	if (classesCount == 1)
		emit(quickOpen());
	else
		showFullScreen();
}

const QColor& SupervisionMenu::paletteBackgroundColor()
{
	return palette().color(backgroundRole());
}

const QColor& SupervisionMenu::paletteForegroundColor()
{
	return palette().color(foregroundRole());
}

