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
#include "main.h" // getPageNodeFromChildNode, SUPERVISION
#include "xml_functions.h" // getChildren, getTextChild
#include "skinmanager.h"
#include "bann2_buttons.h"
#include "bann_energy.h" // BannLoadDiagnostic
#include "devices_cache.h"
#include "loads_device.h"
#include "energy_management.h" // isBuilt
#include "stopngo.h"

#include <QDebug>

enum SystemType
{
	CLASS_STOPNGO = 6001,
	LOAD_DIAGNOSTIC = 6002
};


SupervisionMenu::SupervisionMenu(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	next_page = NULL;
#ifdef LAYOUT_TS_3_5
	buildPage();
#else
	buildPage(getTextChild(config_node, "descr"), TITLE_HEIGHT);
#endif
	loadItems(config_node);
}

int SupervisionMenu::sectionId() const
{
	return EnergyManagement::isBuilt() ? ENERGY_MANAGEMENT : SUPERVISION;
}

void SupervisionMenu::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &node, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(node, "cid").toInt());
		int id = getTextChild(node, "id").toInt();

		BannSinglePuls *b = new BannSinglePuls(0);
		b->initBanner(bt_global::skin->getImage("forward"),
			      bt_global::skin->getImage("center_icon"),
			      getTextChild(node, "descr"));

		page_content->appendBanner(b);

		QDomNode page_node = getPageNodeFromChildNode(node, "lnk_pageID");

		switch (id)
		{
		case CLASS_STOPNGO:
			next_page = new StopAndGoMenu(page_node);
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


LoadDiagnosticPage::LoadDiagnosticPage(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
#ifdef LAYOUT_TS_3_5
	buildPage();
	setSpacing(10);
#else
	buildPage(getTextChild(config_node, "descr"), TITLE_HEIGHT);
#endif

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());

		LoadsDevice *dev = bt_global::add_device_to_cache(new LoadsDevice(getTextChild(item, "where")));
		devices.append(dev);
		Banner *b = new BannLoadDiagnostic(dev, getTextChild(item, "descr"));
		page_content->appendBanner(b);
	}

	// we don't need extra memory, all the devices are built above
	devices.squeeze();
}

int LoadDiagnosticPage::sectionId() const
{
	return EnergyManagement::isBuilt() ? ENERGY_MANAGEMENT : SUPERVISION;
}

void LoadDiagnosticPage::showEvent(QShowEvent *)
{
	foreach (const LoadsDevice *d, devices)
		d->requestLevel();
}
