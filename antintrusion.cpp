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


#include "antintrusion.h"
#include "main.h" // ANTIINTRUSION
#include "xml_functions.h" // getTextChild, getChildren
#include "bann_antintrusion.h"
#include "btbutton.h"
#include "antintrusion_device.h"
#include "keypad.h" // KeypadWithState
#include "devices_cache.h"

#include <QHBoxLayout>
#include <QDebug>
#include <QVariant>


Antintrusion::Antintrusion(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	skin_cid = bt_global::skin->getCidState();

	dev = bt_global::add_device_to_cache(new AntintrusionDevice);
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	antintrusion_system = new BannAntintrusion;
	connect(antintrusion_system, SIGNAL(toggleActivation()), SLOT(toggleActivation()));

	action = NONE;
	QWidget *top_widget;
	top_widget = new QWidget;

	QHBoxLayout *layout = new QHBoxLayout(top_widget);
	layout->setContentsMargins(18, 0, 20, 10);
	layout->setSpacing(5);
	layout->addWidget(antintrusion_system);
	layout->addStretch(1);

#ifdef LAYOUT_TOUCHX
	partial_button = new BtButton(bt_global::skin->getImage("partial"));
	layout->addWidget(partial_button);
#endif

	QList<int> states;
	for (int i = 0; i < NUM_ZONES; ++i)
	{
		int state = -1;
		if (zones[i] != 0)
			state = (zones[i]->isPartialized() ? 1 : 0);

		states.append(state);
	}

	keypad = new KeypadWithState(QList<int>());
	updateKeypadStates();
	keypad->setMode(Keypad::HIDDEN);
	connect(keypad, SIGNAL(Closed()), SLOT(showPage()));
	connect(keypad, SIGNAL(accept()), SLOT(doAction()));
	connect(keypad, SIGNAL(accept()), SLOT(showPage()));
}

void Antintrusion::updateKeypadStates()
{
	QList<int> states;
	for (int i = 0; i < NUM_ZONES; ++i)
	{
		int state = -1;
		if (zones[i] != 0)
			state = (zones[i]->isPartialized() ? 1 : 0);

		states.append(state);
	}
	keypad->setStates(states);
}

int Antintrusion::sectionId() const

{
	return ANTIINTRUSION;
}

void Antintrusion::loadZones(const QDomNode &config_node)
{
	for (int i = 0; i < NUM_ZONES; ++i)
		zones[i] = 0;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int zone_number = getTextChild(item, "where").mid(1).toInt();
		AntintrusionZone *b = new AntintrusionZone(zone_number, getTextChild(item, "descr"));
		connect(b, SIGNAL(requestPartialization(int,bool)), dev, SLOT(partializeZone(int,bool)));
		page_content->appendBanner(b);
		zones[zone_number - 1] = b;
	}
}

void Antintrusion::toggleActivation()
{
	action = ACTIVE;
	updateKeypadStates();
	keypad->showPage();
}

void Antintrusion::partialize()
{
	action = PARTIALIZE;
	updateKeypadStates();
	keypad->showPage();
}

void Antintrusion::doAction()
{
	QString password = keypad->getText();
	if (password.isEmpty())
		return;

	switch (action)
	{
	case ACTIVE:
		dev->toggleActivation(password);
		break;
	case PARTIALIZE:
		dev->setPartialization(password);
		break;
	default:
		qWarning() << "Antintrusion::doAction() Unknown action " << action;
	}
	keypad->resetText();
}

void Antintrusion::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case AntintrusionDevice::DIM_SYSTEM_INSERTED:
		{
			bool inserted = it.value().toBool();
			for (int i = 0; i < NUM_ZONES; ++i)
				if (zones[i])
					zones[i]->enablePartialization(!inserted);
		}
		break;
		}
		++it;
	}
}

