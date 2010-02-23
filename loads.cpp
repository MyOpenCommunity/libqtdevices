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
#include "generic_functions.h" // createMsgOpen
#include "xml_functions.h" // getChildren, getTextChild

#include <assert.h>


bannLoads::bannLoads(sottoMenu *parent, QString indirizzo, QString IconaSx) : bannOnSx(parent)
{
	SetIcons(IconaSx, 1);
	setAddress(indirizzo);
	connect(this,SIGNAL(click()),this,SLOT(Attiva()));
}

void bannLoads::Attiva()
{
	sendFrame(createMsgOpen("3", "2", getAddress()));
}


Loads::Loads(const QDomNode &config_node)
{
	loadItems(config_node);
}

void Loads::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		assert(id == CARICO && "Type of item not handled on loads page!");
		QString where = getTextChild(item, "where");
		QString img = IMG_PATH + getTextChild(item, "cimg1");

		banner *b = new bannLoads(this, where, img);
		b->setText(getTextChild(item, "descr"));
		b->setId(id);
		appendBanner(b);
	}
}
