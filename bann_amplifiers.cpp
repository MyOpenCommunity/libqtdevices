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


#include "bann_amplifiers.h"
#include "devices_cache.h" // bt_global::add_device_to_cache
#include "skinmanager.h" // bt_global::skin
#include "generic_functions.h" // getBostikName
#include "media_device.h"
#include "btbutton.h"

// TODO: in poweramplifier.h there's a base graphic banner to handle volume and state changes for amplifiers
// use it also for Amplifier

namespace {
	AmplifierDevice *createDevice(const QString &where)
	{
		if ((*bt_global::config)[AMPLIFIER_ADDRESS] == where)
			return bt_global::add_device_to_cache(new VirtualAmplifierDevice((*bt_global::config)[AMPLIFIER_ADDRESS]));
		else
			return bt_global::add_device_to_cache(new AmplifierDevice(where));
	}
}

Amplifier::Amplifier(const QString &descr, const QString &where) : BannLevel(0)
{
	volume_value = 0;
	center_left_active = bt_global::skin->getImage("volume_on_left");
	center_right_active = bt_global::skin->getImage("volume_on_right");
	center_left_inactive = bt_global::skin->getImage("volume_off_left");
	center_right_inactive = bt_global::skin->getImage("volume_off_right");
	initBanner(bt_global::skin->getImage("off"), getBostikName(center_left_inactive, QString::number(volume_value)),
		getBostikName(center_right_inactive, QString::number(volume_value)), bt_global::skin->getImage("on"), descr);

	dev = createDevice(where);

	connect(left_button, SIGNAL(clicked()), SLOT(turnOff()));
	connect(right_button, SIGNAL(clicked()), SLOT(turnOn()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(volumeDown()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(volumeUp()));

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void Amplifier::volumeUp()
{
	dev->volumeUp();
}

void Amplifier::volumeDown()
{
	dev->volumeDown();
}

void Amplifier::turnOn()
{
	dev->turnOn();
}

void Amplifier::turnOff()
{
	dev->turnOff();
}

void Amplifier::setIcons()
{
	int index = trasformaVol(volume_value);

	setCenterLeftIcon(getBostikName(active ? center_left_active : center_left_inactive, QString::number(index)));
	setCenterRightIcon(getBostikName(active ? center_right_active : center_right_inactive, QString::number(index)));
}

void Amplifier::valueReceived(const DeviceValues &values_list)
{
	if (values_list.contains(AmplifierDevice::DIM_VOLUME))
		volume_value = values_list[AmplifierDevice::DIM_VOLUME].toInt();

	if (values_list.contains(AmplifierDevice::DIM_STATUS))
		active = values_list[AmplifierDevice::DIM_STATUS].toBool();

	setIcons();
}


AmplifierGroup::AmplifierGroup(QStringList addresses, const QString &descr) : BannLevel(0)
{
	center_left_active = bt_global::skin->getImage("volume_on_left");
	center_right_active = bt_global::skin->getImage("volume_on_right");
	center_left_inactive = bt_global::skin->getImage("volume_off_left");
	center_right_inactive = bt_global::skin->getImage("volume_off_right");
	initBanner(bt_global::skin->getImage("off"), getBostikName(center_left_inactive, "0"),
		getBostikName(center_right_inactive, "0"), bt_global::skin->getImage("on"), descr);

	foreach (const QString &where, addresses)
		devices.append(createDevice(where));

	connect(left_button, SIGNAL(clicked()), SLOT(turnOff()));
	connect(right_button, SIGNAL(clicked()), SLOT(turnOn()));
	connect(this, SIGNAL(center_left_clicked()), SLOT(volumeDown()));
	connect(this, SIGNAL(center_right_clicked()), SLOT(volumeUp()));
}

void AmplifierGroup::volumeUp()
{
	foreach (AmplifierDevice *dev, devices)
		dev->volumeUp();
}

void AmplifierGroup::volumeDown()
{
	foreach (AmplifierDevice *dev, devices)
		dev->volumeDown();
}

void AmplifierGroup::turnOn()
{
	foreach (AmplifierDevice *dev, devices)
		dev->turnOn();

	// always use the 0-volume icons, since groups do not have a definite volume
	setCenterLeftIcon(getBostikName(center_left_active, "0"));
	setCenterRightIcon(getBostikName(center_right_active, "0"));
}

void AmplifierGroup::turnOff()
{
	foreach (AmplifierDevice *dev, devices)
		dev->turnOff();

	// always use the 0-volume icons, since groups do not have a definite volume
	setCenterLeftIcon(getBostikName(center_left_inactive, "0"));
	setCenterRightIcon(getBostikName(center_right_inactive, "0"));
}
