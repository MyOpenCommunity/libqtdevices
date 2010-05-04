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


#include "sorgentiradio.h"
#include "btbutton.h" // BtButton
#include "state_button.h" // StateButton
#include "skinmanager.h" // bt_global::skin
#include "media_device.h" // RadioSourceDevice
#include "radio.h" // RadioInfo

#include <QWidget>
#include <QDebug>
#include <QChar>
#include <QHBoxLayout>


RadioSource::RadioSource(const QString &area, RadioSourceDevice *dev, Page *details) :
	AudioSource(area, dev, details)
{
	radio_info = new RadioInfo(bt_global::skin->getImage("source_background"));

	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("off"), bt_global::skin->getImage("previous"),
		   bt_global::skin->getImage("next"), bt_global::skin->getImage("details"));

	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(radio_info);
	hbox->addWidget(center_right_button);
	hbox->addWidget(right_button);

	right_button->hide();

	connect(this, SIGNAL(sourceStateChanged(bool)), SLOT(sourceStateChanged(bool)));
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void RadioSource::sourceStateChanged(bool active)
{
	right_button->setVisible(active);
}

void RadioSource::valueReceived(const DeviceValues &values_list)
{
	foreach (int dim, values_list.keys())
	{
		switch (dim)
		{
		case RadioSourceDevice::DIM_RDS:
		{
			QString label = values_list[dim].toString();
			radio_info->setRadioName(label);
			break;
		}
		case RadioSourceDevice::DIM_FREQUENCY:
			radio_info->setFrequency(values_list[dim].toInt());
			break;
		case RadioSourceDevice::DIM_TRACK:
			radio_info->setChannel(values_list[dim].toInt());
			break;
		}
	}
}
