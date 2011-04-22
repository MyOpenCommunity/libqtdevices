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


HomePage::HomePage(const QDomNode &config_node) : SectionPage(config_node, false)
{
#ifdef LAYOUT_TS_10
	// TODO: the following margins uses empirical values to align the content
	// in the center of the page. We can do better!
	page_content->layout()->setContentsMargins(55, 35, 100, 0);
	page_content->layout()->setSpacing(25);
#endif
	temp_viewer = new TemperatureViewer(this);
	loadItems(config_node);
}

// Load only the item that is not a section page (which is loaded by SectionPage)
void HomePage::loadItems(const QDomNode &config_node)
{
#ifdef CONFIG_TS_3_5
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

void HomePage::showSectionPage(int page_id)
{
#ifdef CONFIG_TS_3_5
	qFatal("Can't be implemented with old config, and not necessary on TS 3.5'' anyway");
#else
	Q_ASSERT_X(bt_global::btmain->page_list.contains(page_id), "HomePage::showSectionPage", "Page id not found!");
	bt_global::btmain->page_list[page_id]->showPage();
#endif
}

void HomePage::changeIconState(StateButton::Status st)
{
	QHashIterator<int, Page*> it(bt_global::btmain->page_list);
	int page_id = -1;
	while (it.hasNext())
	{
		it.next();
		if (it.value() == sender())
			page_id = it.key();
	}

	if (page_id != -1)
		emit iconStateChanged(page_id, st);
}

