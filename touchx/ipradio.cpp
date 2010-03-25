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


#include "ipradio.h"
#include "skinmanager.h"
#include "xml_functions.h" // getTextChild, getChildren

#include <QLayout>


IPRadioPage::IPRadioPage(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());

	buildPage(4, getTextChild(config_node, "descr"), SMALL_TITLE_HEIGHT);
	loadItems(config_node);
}

void IPRadioPage::loadItems(const QDomNode &config_node)
{
	QList<ItemList::ItemInfo> radio_list;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		ItemList::ItemInfo info(getTextChild(item, "title"), getTextChild(item, "descr"),
					bt_global::skin->getImage("ipradio_icon"),
					bt_global::skin->getImage("forward"),
					getTextChild(item, "url"));

		radio_list.append(info);
	}

	page_content->setList(radio_list);
	page_content->showList();
}
