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


#include "loads.h"
#include "frame_functions.h" // createCommandFrame
#include "xml_functions.h" // getChildren, getTextChild
#include "skinmanager.h"
#include "main.h" // LOADS


enum BannerType
{
	LOAD = 5001,
};


BannLoad::BannLoad(const QString &descr, const QString &where)
{
	initBanner(bt_global::skin->getImage("on"), QString(), descr);
	connect(this, SIGNAL(leftClicked()), SLOT(active()));
	load_where = where;
}

void BannLoad::active()
{
	sendFrame(createCommandFrame("3", "2", load_where));
}


Loads::Loads(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	buildPage();
	loadItems(config_node);
}

int Loads::sectionId() const
{
	return LOADS;
}

void Loads::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext context(getTextChild(item, "cid").toInt());
		int id = getTextChild(item, "id").toInt();
		Q_ASSERT_X(id == LOAD, "Loads::loadItems", "Type of item not handled on loads page!");

		page_content->appendBanner(new BannLoad(getTextChild(item, "descr"), getTextChild(item, "where")));
	}
}
