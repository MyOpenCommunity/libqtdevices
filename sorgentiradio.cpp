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
#include "devices_cache.h" // bt_global::devices_cache
#include "deviceold.h"
#include "generic_functions.h" // createCommandFrame
#include "btbutton.h" // BtButton
#include "skinmanager.h" // bt_global::skin
#include "icondispatcher.h" // bt_global::icons_cache
#include "media_device.h" // RadioSourceDevice
#include "fontmanager.h"

#include <QWidget>
#include <QDebug>
#include <QChar>
#include <QHBoxLayout>
#include <QLabel>


RadioSource::RadioSource(const QString &area, RadioSourceDevice *dev, Page *details) :
	AudioSource(area, dev, details)
{
	left_button = new BtButton;
	center_left_button = new BtButton;
	center_right_button = new BtButton;
	right_button = new BtButton;
	background = new QLabel;

	radio_station = new QLabel("----");
	radio_station->setAlignment(Qt::AlignCenter);
	radio_station->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_TEXT));

	radio_frequency = new QLabel(tr("FM %1").arg("--.-"));
	radio_frequency->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_DESCRIPTION));

	radio_channel = new QLabel(tr("Channel: %1").arg("-"));
	radio_channel->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_DESCRIPTION));

	QGridLayout *texts = new QGridLayout(background);
	texts->setColumnStretch(1, 1);
	texts->setRowStretch(0, 1);
	texts->addWidget(radio_station, 0, 0, 1, 3);
	texts->addWidget(radio_frequency, 1, 0);
	texts->addWidget(radio_channel, 1, 2);

	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("previous"), bt_global::skin->getImage("radio_dummy"),
		bt_global::skin->getImage("next"), bt_global::skin->getImage("details"));
	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(background);
	hbox->addWidget(center_right_button);
	hbox->addWidget(right_button);

	right_button->hide();

	connect(this, SIGNAL(sourceStateChanged(bool)), SLOT(sourceStateChanged(bool)));
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	connect(left_button, SIGNAL(clicked()), SLOT(turnOn()));
	connect(center_left_button, SIGNAL(clicked()), dev, SLOT(prevTrack()));
	connect(center_right_button, SIGNAL(clicked()), dev, SLOT(nextTrack()));
	connect(right_button, SIGNAL(clicked()), SLOT(showDetails()));
}

void RadioSource::initBanner(const QString &left, const QString &center_left, const QString &center,
	const QString &center_right, const QString &right)
{
	initButton(left_button, left);
	initButton(center_left_button, center_left);
	initButton(center_right_button, center_right);
	initButton(right_button, right);
	background->setPixmap(*bt_global::icons_cache.getIcon(center));
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
			if (label.isEmpty())
				radio_station->setText("----");
			else
				radio_station->setText(label);
			break;
		}
		case RadioSourceDevice::DIM_FREQUENCY:
			radio_frequency->setText(QString(tr("FM %1")).arg(values_list[dim].toInt() / 100.0, 0, 'f', 2));
			break;
		case RadioSourceDevice::DIM_TRACK:
			radio_channel->setText(QString(tr("Channel: %1")).arg(values_list[dim].toInt()));
			break;
		}
	}
}
