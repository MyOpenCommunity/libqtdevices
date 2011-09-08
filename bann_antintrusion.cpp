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


#include "bann_antintrusion.h"
#include "icondispatcher.h" // getIcon
#include "skinmanager.h"
#include "btbutton.h"
#include "labels.h"
#include "generic_functions.h" // getPressName

#include <QLayout>



AntintrusionZone::AntintrusionZone(int zone, QString descr)
{
	zone_number = zone;
	partialized = true;
	partialization_enabled = true;
	zone_description = descr;

	left_off = bt_global::skin->getImage("zone_inactive");
	left_on = bt_global::skin->getImage("zone_active");

#ifdef LAYOUT_TS_3_5
	status_off = bt_global::skin->getImage("zone_status_off");
	status_on = bt_global::skin->getImage("zone_status_on");

	initBanner(QString(), status_off, bt_global::skin->getImage("zone"), left_off, descr);
	setCentralSpacing(false);
	disableCentralButtons();
#else
	initBanner(left_off, QString(), descr);
	disabled_left_off = getPressName(left_off);
	disabled_left_on = getPressName(left_on);
#endif

	connect(left_button, SIGNAL(clicked()), SLOT(leftClicked()));
}

QString AntintrusionZone::zoneDescription() const
{
	return zone_description;
}

void AntintrusionZone::leftClicked()
{
	emit requestPartialization(zone_number, !partialized);
	setPartialization(!partialized);
}

void AntintrusionZone::enablePartialization(bool enabled)
{
	if (enabled != partialization_enabled)
	{
		if (enabled)
		{
#ifdef LAYOUT_TS_3_5
			left_button->show();
#else
			left_button->setImage(partialized ? left_off : left_on);
			left_button->enable();
#endif
		}
		else
		{
#ifdef LAYOUT_TS_3_5
			left_button->hide();
#else
			left_button->setImage(partialized ? disabled_left_off : disabled_left_on);
			left_button->disable();
#endif
		}
		partialization_enabled = enabled;
	}
}

void AntintrusionZone::setPartialization(bool _partialized)
{
#ifdef LAYOUT_TS_3_5
	center_right_button->setImage(_partialized ? status_off : status_on);
#endif
	left_button->setImage(_partialized ? left_off : left_on);
	partialized = _partialized;
}

bool AntintrusionZone::isPartialized() const
{
	return partialized;
}


BannAntintrusion::BannAntintrusion(QWidget *parent) : Bann2Buttons(parent)
{
	turn_off = bt_global::skin->getImage("off");
	turn_on = bt_global::skin->getImage("on");

	state_off = bt_global::skin->getImage("alarm_state_off");
	state_on = bt_global::skin->getImage("alarm_state_on");

	initBanner(turn_on, state_off, bt_global::skin->getImage("info"), QString());
#ifdef LAYOUT_TS_10
	layout()->setSpacing(5); // TODO: blah! Find a better way to set the spacing!
#endif
	showAlarmsButton(false);

	connect(left_button, SIGNAL(clicked()), this, SIGNAL(toggleActivation()));
	connect(right_button, SIGNAL(clicked()), this, SIGNAL(showAlarms()));
}

void BannAntintrusion::setState(bool on)
{
	left_button->setImage(on ? turn_off : turn_on);
	setBackgroundImage(on ? state_on : state_off);
}

void BannAntintrusion::showAlarmsButton(bool show)
{
	right_button->setVisible(show);
}


