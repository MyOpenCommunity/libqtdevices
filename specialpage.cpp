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


#include "specialpage.h"
#include "timescript.h"
#include "btbutton.h"
#include "main.h" // ICON_FRECCIA_SX
#include "xml_functions.h" // getChildren, getTextChild, getChildWithName
#include "generic_functions.h" // createMsgOpen
#include "fontmanager.h" // bt_global::font
#include "temperatureviewer.h"
#include "skinmanager.h" //skin

#include <openmsg.h>

#include <QDomNode>
#include <QDebug>
#include <QLabel>


#define DIM_BUT_BACK 60

#define BORDER_SIZE 10
#define ITEM_HEIGHT 60
namespace
{
	inline int getPosition(int item_number)
	{
		return (item_number - 1) * 80 + BORDER_SIZE;
	}
}


SpecialPage::SpecialPage(const QDomNode &config_node)
{
	temp_viewer = new TemperatureViewer(this);
	loadItems(config_node);
	loadSpecial(config_node);
}

void SpecialPage::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		// We get the number after "item"
		int itemNum = item.nodeName().mid(QString("item").length()).toInt();
		switch (id)
		{
		case DATA:
		case OROLOGIO:
		{
			timeScript *d = new timeScript(this, id == DATA ? 25 : 1);
			d->setGeometry(BORDER_SIZE, getPosition(itemNum), width() - BORDER_SIZE, ITEM_HEIGHT);
			d->setLineWidth(3);
			break;
		}
		case TEMPERATURA:
		case TERMO_HOME_NC_PROBE:
		case TERMO_HOME_NC_EXTPROBE:
		{
			QString ext = (id == TERMO_HOME_NC_EXTPROBE) ? "1" : "0";
			temp_viewer->add(getTextChild(item, "where"), getTextChild(item, "openserver_id").toInt(),
				BORDER_SIZE, getPosition(itemNum), width() - BORDER_SIZE, ITEM_HEIGHT, getTextChild(item, "descr"), ext);
			break;
		}
		default:
			qFatal("Unknown item type on SpecialPage!");
		}
	}
}

void SpecialPage::loadSpecial(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());

	// Load the back button
	BtButton *b = new BtButton(this);
	b->setImage(bt_global::skin->getImage("back"));
	b->setGeometry(0, height() - ITEM_HEIGHT, DIM_BUT_BACK, DIM_BUT_BACK);
	connect(b, SIGNAL(clicked()), SIGNAL(Closed()));

	// Load the special button
	b = new BtButton(this);
	b->setImage(bt_global::skin->getImage("command"));
	const int command_button_y = height() - ITEM_HEIGHT;
	b->setGeometry(DIM_BUT_BACK, command_button_y, width() - DIM_BUT_BACK, DIM_BUT_BACK);

	QDomNode command = getChildWithName(config_node, "command");
	type = static_cast<specialType>(getTextChild(command, "type").toInt());
	who = getTextChild(command, "who");
	what = getTextChild(command, "what");
	where = getTextChild(command, "where");

	if (type == BUTTON)
	{
		connect(b, SIGNAL(pressed()), SLOT(pressedButton()));
		connect(b, SIGNAL(released()), SLOT(releasedButton()));
	}
	else
		connect(b, SIGNAL(clicked()), SLOT(clickedButton()));

	// Load the description of special button
	QLabel *box_text = new QLabel(this);
	box_text->setFont(bt_global::font->get(FontManager::TEXT));
	box_text->setAlignment(Qt::AlignCenter);
	box_text->setText(getTextChild(command, "descr"));
	const int TEXT_HEIGHT = 20;
	box_text->setGeometry(DIM_BUT_BACK, command_button_y - TEXT_HEIGHT, width() - DIM_BUT_BACK, TEXT_HEIGHT);

	subscribe_monitor(who.toInt());
}

void SpecialPage::clickedButton()
{
	if (type == CYCLIC)
		what = what == "0" ? "1" : "0";

	sendFrame(createMsgOpen(who, what, where));
}

void SpecialPage::pressedButton()
{
	sendFrame(createMsgOpen(who, "1", where));
}

void SpecialPage::releasedButton()
{
	sendFrame(createMsgOpen(who, "0", where));
}

void SpecialPage::manageFrame(OpenMsg &msg)
{
	if (who.toInt() == msg.who() && type == CYCLIC)
		if (where == QString(msg.Extract_dove()))
			what = QString(msg.Extract_cosa());
}

void SpecialPage::inizializza()
{
	temp_viewer->inizializza();
}
