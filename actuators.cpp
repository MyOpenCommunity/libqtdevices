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
#include "devices_cache.h" // bt_global::devices_cache
#include "lighting_device.h" // LightingDevice
#include "entryphone_device.h" // EntryphoneDevice
#include "skinmanager.h" //skin

#include <QDebug>


SingleActuator::SingleActuator(const QString &descr, const QString &where, int openserver_id, PullMode pull_mode)
	: BannOnOffState(0)
{
	initBanner(bt_global::skin->getImage("off"), bt_global::skin->getImage("actuator_state"),
		bt_global::skin->getImage("on"), OFF, descr);

	// TODO: read pull mode from config
	dev = bt_global::add_device_to_cache(new LightingDevice(where, pull_mode, openserver_id));
	setOpenserverConnection(dev);

	connect(left_button, SIGNAL(clicked()), SLOT(deactivate()));
	connect(right_button, SIGNAL(clicked()), SLOT(activate()));
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void SingleActuator::activate()
{
	dev->turnOn();
}

void SingleActuator::deactivate()
{
	dev->turnOff();
}

void SingleActuator::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
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



ButtonActuator::ButtonActuator(const QString &descr, const QString &_where, int t, int openserver_id, PullMode pull_mode) : BannSinglePuls(0)
{
	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("action_icon"), descr);

	type = t;
	where = _where;

	switch (type)
	{
	case PULSE_ACT:
		dev = bt_global::add_device_to_cache(new LightingDevice(where, pull_mode, openserver_id));
		break;
	case VCT_LOCK:
	case VCT_STAIRLIGHT:
		dev = bt_global::add_device_to_cache(new EntryphoneDevice(where, QString(), openserver_id));
		break;
	default:
		Q_ASSERT_X(false, "ButtonActuator::ButtonActuator", "Type of actuator unknown!");
	}
	setOpenserverConnection(dev);
	connect(right_button, SIGNAL(pressed()), SLOT(activate()));
	connect(right_button, SIGNAL(released()), SLOT(deactivate()));
}

void ButtonActuator::activate()
{
	if (type == PULSE_ACT)
		(static_cast<LightingDevice*>(dev))->turnOn();
	else
	{
		EntryphoneDevice *d = static_cast<EntryphoneDevice*>(dev);
		if (type == VCT_LOCK)
			d->openLock();
		else
			d->stairLightActivate();
	}
}

void ButtonActuator::deactivate()
{
	if (type == PULSE_ACT)
		(static_cast<LightingDevice*>(dev))->turnOff();
	else
	{
		EntryphoneDevice *d = static_cast<EntryphoneDevice*>(dev);
		if (type == VCT_LOCK)
			d->releaseLock();
		else
			d->stairLightRelease();
	}
}

