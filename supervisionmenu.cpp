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
#include "bann1_button.h"
#include "main.h" // getPageNodeFromChildNode
#include "xml_functions.h" // getChildren, getTextChild
#include "bannercontent.h"
#include "skinmanager.h"
#include "bann2_buttons.h"
#include "bann_energy.h" // BannLoadDiagnostic
#include "devices_cache.h"
#include "loads_device.h"

#include <QDebug>

enum
{
#ifdef LAYOUT_TOUCHX
	CLASS_STOPNGO = 6001,
	LOAD_DIAGNOSTIC = 6002
#else
	CLASS_STOPNGO = 59,
	LOAD_DIAGNOSTIC = 82
#endif
};

SupervisionMenu::SupervisionMenu(const QDomNode &config_node)
{
	next_page = NULL;
	buildPage();
	loadItems(config_node);
}

int SupervisionMenu::sectionId() const
{
	return SUPERVISIONE;
}

void SupervisionMenu::loadItems(const QDomNode &config_node)
{
#ifdef CONFIG_BTOUCH
	foreach (const QDomNode &node, getChildren(config_node, ""))
	{
		if (!node.nodeName().startsWith("class") && node.nodeName() != "load")
			continue;
#else
	foreach (const QDomNode &node, getChildren(config_node, "item"))
	{
#endif
		SkinContext cxt(getTextChild(node, "cid").toInt());
		int id = getTextChild(node, "id").toInt();

		BannSinglePuls *b = new BannSinglePuls(0);
		b->initBanner(bt_global::skin->getImage("forward"),
			      bt_global::skin->getImage("center_icon"),
			      getTextChild(node, "descr"));

		page_content->appendBanner(b);

#ifdef CONFIG_BTOUCH
		QDomNode page_node = node;
#else
		QDomNode page_node = getPageNodeFromChildNode(node, "lnk_pageID");
#endif

		switch (id)
		{
		case CLASS_STOPNGO:
			next_page = new StopNGoMenu(page_node);
			break;
		case LOAD_DIAGNOSTIC:
			next_page = new LoadDiagnosticPage(page_node);
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

		LoadsDevice *dev = bt_global::add_device_to_cache(new LoadsDevice(getTextChild(item, "where")));
		devices.append(dev);
		banner *b = new BannLoadDiagnostic(dev, getTextChild(item, "descr"));
		page_content->appendBanner(b);
	}

	// we don't need extra memory, all the devices are built above
	devices.squeeze();
}

void LoadDiagnosticPage::showEvent(QShowEvent *)
{
	foreach (const LoadsDevice *d, devices)
		d->requestLevel();
}
