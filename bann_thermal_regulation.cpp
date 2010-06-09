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


#include "bann_thermal_regulation.h"

#include "thermal_device.h"
#include "skinmanager.h"
#include "btbutton.h"


BannOff::BannOff(QWidget *parent, ThermalDevice *_dev) : BannCenteredButton(parent)
{
	initBanner(bt_global::skin->getImage("off_button"));
	dev = _dev;
	connect(center_button, SIGNAL(clicked()), SLOT(performAction()));
	connect(center_button, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannOff::performAction()
{
	dev->setOff();
}


BannAntifreeze::BannAntifreeze(QWidget *parent, ThermalDevice *_dev) : BannCenteredButton(parent)
{
	initBanner(bt_global::skin->getImage("regulator_antifreeze"));
	dev = _dev;
	connect(center_button, SIGNAL(clicked()), SLOT(performAction()));
	connect(center_button, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannAntifreeze::performAction()
{
	dev->setProtection();
}


BannOffAntifreeze::BannOffAntifreeze(QWidget *parent, ThermalDevice *_dev)
{
	initBanner(bt_global::skin->getImage("off_button"), bt_global::skin->getImage("regulator_antifreeze"), "");
	dev = _dev;

	connect(center_left, SIGNAL(clicked()), SLOT(setOff()));
	connect(center_left, SIGNAL(clicked()), SIGNAL(clicked()));
	connect(center_right, SIGNAL(clicked()), SLOT(setAntifreeze()));
	connect(center_right, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannOffAntifreeze::setOff()
{
	dev->setOff();
}

void BannOffAntifreeze::setAntifreeze()
{
	dev->setProtection();
}


BannSummerWinter::BannSummerWinter(QWidget *parent, ThermalDevice *_dev)
{
	initBanner(bt_global::skin->getImage("winter"), bt_global::skin->getImage("summer"), "");
	dev = _dev;

	connect(center_left, SIGNAL(clicked()), SLOT(setWinter()));
	connect(center_left, SIGNAL(clicked()), SIGNAL(clicked()));
	connect(center_right, SIGNAL(clicked()), SLOT(setSummer()));
	connect(center_right, SIGNAL(clicked()), SIGNAL(clicked()));
}

void BannSummerWinter::setSummer()
{
	dev->setSummer();
}

void BannSummerWinter::setWinter()
{
	dev->setWinter();
}


BannWeekly::BannWeekly(QWidget *parent, int _index)
	: BannSinglePuls(parent)
{
	index = _index;
	connect(right_button, SIGNAL(clicked()), SLOT(performAction()));
}

void BannWeekly::performAction()
{
	emit programNumber(index);
}

