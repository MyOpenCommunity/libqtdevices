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
#include "bannercontent.h"
#include "skinmanager.h"
#include "bann2_buttons.h"
#include "bann_energy.h" // BannLoadDiagnostic
#include "devices_cache.h"
#include "loads_device.h"

#include <QDebug>


SupervisionMenu::SupervisionMenu(const QDomNode &config_node)
{
	next_page = NULL;
	buildPage();
	loadItems(config_node);
}

void SupervisionMenu::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &node, getChildren(config_node, ""))
	{
		if (!node.nodeName().startsWith("class") && node.nodeName() != "load")
			continue;

		SkinContext cxt(getTextChild(node, "cid").toInt());
		int id = getTextChild(node, "id").toInt();

		BannSinglePuls *b = new BannSinglePuls(0);
		b->initBanner(bt_global::skin->getImage("forward"),
			      bt_global::skin->getImage("center_icon"),
			      getTextChild(node, "descr"));

		page_content->appendBanner(b);

		switch (id)
		{
		case CLASS_STOPNGO:
			next_page = new StopNGoMenu(node);
			break;
		case LOAD_DIAGNOSTIC:
			next_page = new LoadDiagnosticPage(node);
			break;
		default:
			qFatal("Unsupported node type in supervision menu");
		};

		b->connectRightButton(next_page);
		connect(b, SIGNAL(pageClosed()), SLOT(showPage()));

	}

	// skip page if only one item
	if (page_content->bannerCount() > 1)
		next_page = NULL;
	else
		connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

void SupervisionMenu::showPage()
{
	if (next_page)
		next_page->showPage();
	else
		BannerPage::showPage();
}


StopNGoMenu::StopNGoMenu(const QDomNode &conf_node)
{
	buildPage();

	QList<QDomNode> items = getChildren(conf_node, "item");
	foreach (const QDomNode &item, items)
	{
		int id = getTextChild(item, "id").toInt();
		QString descr = getTextChild(item, "descr");
		QString where = getTextChild(item, "where");

		BannPulsDynIcon *bp = new BannPulsDynIcon(this, where);  // Create a new banner
		bp->SetIcons(ICON_FRECCIA_DX, 0, ICON_STOPNGO_CHIUSO);
		bp->setText(descr);
		bp->setAnimationParams(0, 0);
		bp->setId(id);
		bp->Draw();

		page_content->appendBanner(bp);

		next_page = new StopngoPage(where, id, descr);
		bp->connectDxButton(next_page);
		connect(bp, SIGNAL(pageClosed()), SLOT(showPage()));
	}

	// skip list if there's only one item
	if (items.size() > 1)
		next_page = NULL;
	else
		connect(next_page, SIGNAL(Closed()), SIGNAL(Closed()));
}

void StopNGoMenu::showPage()
{
	if (next_page)
		next_page->showPage();
	else
		BannerPage::showPage();
}


LoadDiagnosticPage::LoadDiagnosticPage(const QDomNode &config_node)
{
	buildPage();

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());

		device *dev = bt_global::add_device_to_cache(new LoadsDevice(getTextChild(item, "where")));
		banner *b = new BannLoadDiagnostic(dev, getTextChild(item, "descr"));
		page_content->appendBanner(b);
	}
}
