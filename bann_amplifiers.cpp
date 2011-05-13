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
#include "skinmanager.h" // bt_global::skin
#include "generic_functions.h" // getBostikName, scsToGraphicalVolume
#include "media_device.h"
#include "btbutton.h"


// TODO: in poweramplifier.h there's a base graphic banner to handle volume and state changes for amplifiers
// use it also for Amplifier

Amplifier::Amplifier(const QString &descr, const QString &where) : BannLevel(0)
{
	volume_value = 0;
	active = false;

	// The images for the amplifier level are from 0 to 8 for ts 10'' and from 1 to 9
	// for ts 3.5''. The function scsToGraphicalVolume adjust the scs value
	// properly.

	QString initial_value = QString::number(scsToGraphicalVolume(volume_value));

	center_left_active = bt_global::skin->getImage("volume_on_left");
	center_right_active = bt_global::skin->getImage("volume_on_right");
	center_left_inactive = bt_global::skin->getImage("volume_off_left");
	center_right_inactive = bt_global::skin->getImage("volume_off_right");
	initBanner(bt_global::skin->getImage("off"), getBostikName(center_left_inactive, initial_value),
		getBostikName(center_right_inactive, initial_value), bt_global::skin->getImage("on"), descr);

	dev = AmplifierDevice::createDevice(where);

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
	int index = scsToGraphicalVolume(volume_value);

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
	initBanner(bt_global::skin->getImage("off"), center_left_inactive,
		center_right_inactive, bt_global::skin->getImage("on"), descr);

	foreach (const QString &where, addresses)
		devices.append(AmplifierDevice::createDevice(where));

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

	setCenterLeftIcon(center_left_active);
	setCenterRightIcon(center_right_active);
}

void AmplifierGroup::turnOff()
{
	foreach (AmplifierDevice *dev, devices)
		dev->turnOff();

	setCenterLeftIcon(center_left_inactive);
	setCenterRightIcon(center_right_inactive);
}
