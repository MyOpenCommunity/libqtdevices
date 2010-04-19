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


#include "homepage.h"
#include "timescript.h"
#include "fontmanager.h"
#include "temperatureviewer.h"
#include "btbutton.h"
#include "xml_functions.h" // getChildren, getTextChild
#include "hardware_functions.h"
#include "btmain.h"

#include <QDomNode>
#include <QLayout>


HomePage::HomePage(const QDomNode &config_node) : SectionPage(config_node)
{
#ifdef LAYOUT_TOUCHX
	page_content->layout()->setContentsMargins(25, 35, 100, 0);
	page_content->layout()->setSpacing(28);
#endif
	temp_viewer = new TemperatureViewer(this);
	loadItems(config_node);
}

// Load only the item that is not a section page (which is loaded by SectionPage)
void HomePage::loadItems(const QDomNode &config_node)
{
#ifdef CONFIG_BTOUCH
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		int x = getTextChild(item, "left").toInt();
		int y = getTextChild(item, "top").toInt();

		switch (id)
		{
		case DATA:
		case OROLOGIO:
		{
			timeScript *d = new timeScript(this, id == DATA ? 25 : 1);
			d->setGeometry(x + 10, y + 10, 220, 60);
			d->setLineWidth(3);
			break;
		}
		case TEMPERATURA:
		case TERMO_HOME_NC_PROBE:
			temp_viewer->add(getTextChild(item, "where"), getTextChild(item, "openserver_id").toInt(),
				x, y, 220, 60, "", "0");
			break;
		case TERMO_HOME_NC_EXTPROBE:
			temp_viewer->add(getTextChild(item, "where"), getTextChild(item, "openserver_id").toInt(),
				x, y, 220, 60, "", "1");
			break;
		}
	}
#endif
}

void HomePage::inizializza()
{
	temp_viewer->inizializza();
}

Page::PageType HomePage::pageType()
{
	return HOMEPAGE;
}

void HomePage::showSectionPage(int page_id)
{
#ifdef CONFIG_BTOUCH
	qFatal("Can't be implemented with old config, and not necessary on BTouch anyway");
#else
	// TODO: a section page can be built only once, so pass as argument the id instead of the page_id!
	int id = getTextChild(getPageNodeFromPageId(page_id), "id").toInt();
	bt_global::btmain->page_list[id]->showPage();
#endif
}
