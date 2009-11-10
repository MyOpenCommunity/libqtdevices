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
#include "bann1_button.h"
#include "main.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "content_widget.h"
#include "navigation_bar.h"

#include <QDebug>

#define STOPNGO_BANN_IMAGE ICON_STOPNGO_CHIUSO

static Page *getStopNgoPage(Page *back, bannPuls* bnr, StopngoItem* itm)
{
	StopngoPage* pg = new StopngoPage(itm->GetWhere(), itm->GetId(), itm->GetDescr());

	QObject::connect(bnr, SIGNAL(sxClick()), pg, SLOT(showPage()));
	QObject::connect(pg, SIGNAL(Closed()), back, SLOT(showPage()));

	return pg;
}

SupervisionMenu::SupervisionMenu(const QDomNode &config_node)
{
	stopngoSubmenu = NULL;
	buildPage(new ContentWidget, new NavigationBar);
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
			b->Draw();
			page_content->appendBanner(b);
			CreateStopnGoMenu(node, b);
			++classesCount;
			break;
		}
		default:
			qFatal("Type of class not handled on supervision page!");
		}
	}

	if (classesCount == 1)  // Only one class has been defined in the supervision section of conf.xml
	{
		bannPuls* bann = static_cast<bannPuls*>(page_content->getBanner(page_content->bannerCount() - 1));
		connect(this, SIGNAL(quickOpen()), bann, SIGNAL(sxClick()));

		if (stopngoSubmenu)  // Check is the only submenu is a stopngo menu
		{
			if (this == stopngoSubmenu)  // Only one Stop&Go device is mapped
			{
				Page* pg = stopngoPages.first();
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
		stopngoSubmenu = new StopNGoMenu(stopngoList);
		connect(bann, SIGNAL(sxClick()), stopngoSubmenu, SLOT(showPage()));  // Connect submenu
		connect(stopngoSubmenu, SIGNAL(Closed()), this, SLOT(showPage()));
	}
	else if (stopngoList.size() == 1)  // one device
	{
		// directly open the only available device page
		stopngoSubmenu = this;
		Page *pg = getStopNgoPage(this, bann, stopngoList.at(0));
		stopngoPages.append(pg);
	}
}

void SupervisionMenu::showPage()
{
	qDebug("SupervisionMenu::showPage()");
	if (classesCount == 1)
		emit quickOpen();
	else
		BannerPage::showPage();
}

StopNGoMenu::StopNGoMenu(QList<StopngoItem*> stopngoList)
{
	buildPage(new ContentWidget, new NavigationBar);

	for (int i = 0; i < stopngoList.size(); ++i)
	{
		StopngoItem *itm = stopngoList.at(i);
		BannPulsDynIcon *bp = new BannPulsDynIcon(this, itm->GetWhere());  // Create a new banner
		bp->SetIcons(ICON_FRECCIA_DX, 0, ICON_STOPNGO_CHIUSO);
		bp->setText(itm->GetDescr());
		bp->setAnimationParams(0, 0);
		bp->setId(itm->GetId());
		bp->Draw();

		page_content->appendBanner(bp);  // Add the new banner to the submenu
		getStopNgoPage(this, bp, itm);  // Connect the new banner to the page
	}
}
