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
#include "xml_functions.h" // getChildren, getTextChild, getChildWithName
#include "frame_functions.h" // createCommandFrame
#include "fontmanager.h" // bt_global::font
#include "temperatureviewer.h"
#include "skinmanager.h" //skin

#include <openmsg.h>

#include <QDomNode>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>


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
	int item_counter = 1;
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		switch (id)
		{
		case ITEM_DATE:
		case ITEM_TIME:
		{
			timeScript *d = new timeScript(this, id == ITEM_DATE ? 25 : 1);
			d->setGeometry(BORDER_SIZE, getPosition(item_counter), width() - BORDER_SIZE, ITEM_HEIGHT);
			d->setLineWidth(3);
			break;
		}
		case ITEM_TEMPERATURE_PROBE:
		case ITEM_TEMPERATURE_EXTPROBE:
		{
			QString ext = (id == ITEM_TEMPERATURE_EXTPROBE) ? "1" : "0";
			temp_viewer->add(getTextChild(item, "where"), getTextChild(item, "openserver_id").toInt(),
				BORDER_SIZE, getPosition(item_counter), width() - BORDER_SIZE, ITEM_HEIGHT, getTextChild(item, "descr"), ext);
			break;
		}
		default:
			qFatal("Unknown item type on SpecialPage!");
		}
		item_counter++;
	}
}

void SpecialPage::loadSpecial(const QDomNode &config_node)
{
	SkinContext context(getTextChild(config_node, "cid").toInt());
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);
	// We want to put the items loaded in the loadItems method (which use a fixed
	// alignment) on the top of the page, so we use a stretch to position the
	// buttons (which use layouts) on the bottom.
	main_layout->addStretch();


	QGridLayout *buttons_layout = new QGridLayout;
	main_layout->addLayout(buttons_layout);
	buttons_layout->setContentsMargins(5, 0, 0, 15);
	buttons_layout->setSpacing(0);

	QDomNode command = getChildWithName(config_node, "command");

	QLabel *description = new QLabel(getTextChild(command, "descr"));

	description->setFont(bt_global::font->get(FontManager::TEXT));
	description->setAlignment(Qt::AlignCenter);
	buttons_layout->addWidget(description, 0, 1);

	BtButton *back = new BtButton(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), SIGNAL(Closed()));
	buttons_layout->addWidget(back, 1, 0);

	BtButton *special = new BtButton(bt_global::skin->getImage("command"));
	buttons_layout->addWidget(special, 1, 1);

	type = static_cast<SpecialType>(getTextChild(command, "type").toInt());
	who = getTextChild(command, "who");
	what = getTextChild(command, "what");
	where = getTextChild(command, "where");

	if (type == BUTTON)
	{
		connect(special, SIGNAL(pressed()), SLOT(pressedButton()));
		connect(special, SIGNAL(released()), SLOT(releasedButton()));
	}
	else
		connect(special, SIGNAL(clicked()), SLOT(clickedButton()));

	subscribeMonitor(who.toInt());
}

void SpecialPage::clickedButton()
{
	if (type == CYCLIC)
		what = (what == "0" ? "1" : "0");

	sendFrame(createCommandFrame(who, what, where));
}

void SpecialPage::pressedButton()
{
	sendFrame(createCommandFrame(who, "1", where));
}

void SpecialPage::releasedButton()
{
	sendFrame(createCommandFrame(who, "0", where));
}

void SpecialPage::manageFrame(OpenMsg &msg)
{
	if (who.toInt() == msg.who() && type == CYCLIC)
		if (where == QString(msg.Extract_dove()))
			what = QString(msg.Extract_cosa());
}
