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


#include "actuators.h"
#include "btbutton.h"
#include "generic_functions.h" // createMsgOpen
#include "devices_cache.h" // bt_global::devices_cache
#include "lighting_device.h"
#include "skinmanager.h" //skin

#include <QDebug>


SingleActuator::SingleActuator(const QString &descr, const QString &where, int openserver_id)
	: BannOnOffState(0)
{
	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("actuator_state"),
		bt_global::skin->getImage("on"), OFF, descr);

	// TODO: read pull mode from config
	dev = bt_global::add_device_to_cache(new LightingDevice(where, PULL_UNKNOWN, openserver_id));

	connect(left_button, SIGNAL(clicked()), SLOT(deactivate()));
	connect(right_button, SIGNAL(clicked()), SLOT(activate()));
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
}

void SingleActuator::activate()
{
	dev->turnOn();
}

void SingleActuator::deactivate()
{
	dev->turnOff();
}

void SingleActuator::status_changed(const StatusList &status_list)
{
	StatusList::const_iterator it = status_list.constBegin();
	while (it != status_list.constEnd())
	{
		switch (it.key())
		{
		case LightingDevice::DIM_DEVICE_ON:
			it.value().toBool() ? setState(ON) : setState(OFF);
			break;
		}
		++it;
	}
}



ButtonActuator::ButtonActuator(const QString &descr, const QString &_where, int t) :
	BannSinglePuls(0),
	where(_where)
{
	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("action_icon"), descr);

	type = t;

	connect(right_button, SIGNAL(pressed()), SLOT(activate()));
	connect(right_button, SIGNAL(released()), SLOT(deactivate()));
}

// This banner should have a real device but 1) no vct devices are implemented, 2) three classes should be
// implemented, which really seems to me an overkill
void ButtonActuator::activate()
{
	switch (type)
	{
	case  AUTOMAZ:
		sendFrame(createMsgOpen("1", "1", where));
		break;
	case  VCT_SERR:
		sendFrame(createMsgOpen("6", "10", where));
		break;
	case  VCT_LS:
		sendFrame(createMsgOpen("6", "12", where));
		break;
	}
}

void ButtonActuator::deactivate()
{
	switch (type)
	{
	case  AUTOMAZ:
		sendFrame(createMsgOpen("1", "0", where));
		break;
	case  VCT_SERR:
		break;
	case  VCT_LS:
		sendFrame(createMsgOpen("6", "11", where));
		break;
	}
}

