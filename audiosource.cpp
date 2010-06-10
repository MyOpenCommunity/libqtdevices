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


#include "audiosource.h"
#include "media_device.h"
#include "page.h"
#include "state_button.h"
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h" // bt_global::skin
#include "btbutton.h"
#include "media_device.h" // SourceDevice, VirtualSourceDevice, RadioSourceDevice
#include "labels.h" // TextOnImageLabel
#include "radio.h" // RadioInfo

#include <QBoxLayout>
#include <QDebug>


AudioSource::AudioSource(const QString &_area, SourceDevice *_dev, Page *_details) :
	BannerNew(0)
{
	left_button = new StateButton;
	left_button->setOnOff();
	center_left_button = new BtButton;
	center_right_button = new BtButton;
	right_button = new BtButton;

	details = _details;
	area = _area;
	dev = _dev;

	connect(left_button, SIGNAL(clicked()), SLOT(turnOn()));
	connect(center_left_button, SIGNAL(clicked()), dev, SLOT(prevTrack()));
	connect(center_right_button, SIGNAL(clicked()), dev, SLOT(nextTrack()));
	connect(right_button, SIGNAL(clicked()), SLOT(showDetails()));

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceivedAudioSource(DeviceValues)));
}

void AudioSource::initBanner(const QString &left_on, const QString &left_off, const QString &center_left,
	const QString &center_right, const QString &right)
{
	left_button->setOnImage(left_on);
	left_button->setOffImage(left_off);
	initButton(center_left_button, center_left);
	initButton(center_right_button, center_right);
	initButton(right_button, right);
}

void AudioSource::turnOn()
{
	dev->turnOn(area);
}

void AudioSource::showDetails()
{
	// TODO using the page stack here means deciding what the behaviour should
	//      be for pages that allow navigation (f.e. if cleanup should be called
	//      for the current page, for the pushed page or for both)
	disconnect(details, SIGNAL(Closed()), 0, 0);
	connect(details, SIGNAL(Closed()), this, SIGNAL(pageClosed()));

	details->showPage();
}

void AudioSource::valueReceivedAudioSource(const DeviceValues &values_list)
{
	if (values_list.contains(SourceDevice::DIM_AREAS_UPDATED))
	{
		bool status = left_button->getStatus();
		bool active = dev->isActive(area);

		if (status == active)
			return;

		left_button->setStatus(active);
		emit sourceStateChanged(active);
	}
}


AuxSource::AuxSource(const QString &area, SourceDevice *dev, const QString &description) :
	AudioSource(area, dev)
{
	center_icon = new TextOnImageLabel(0, description);
	center_icon->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_TEXT));

	initBanner(bt_global::skin->getImage("turned_on"), bt_global::skin->getImage("turn_on"), bt_global::skin->getImage("previous"),
		   bt_global::skin->getImage("next"), QString());
	center_icon->setBackgroundImage(bt_global::skin->getImage("source_background"));

	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(center_icon);
	hbox->addWidget(center_right_button);
	hbox->addStretch(1);
}


MediaSource::MediaSource(const QString &area, VirtualSourceDevice *dev, const QString &description, Page *details) :
	AudioSource(area, dev, details)
{
	center_icon = new TextOnImageLabel(0, description);
	center_icon->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_TEXT));

	initBanner(bt_global::skin->getImage("turned_on"), bt_global::skin->getImage("turn_on"), bt_global::skin->getImage("previous"),
		   bt_global::skin->getImage("next"), bt_global::skin->getImage("details"));
	center_icon->setBackgroundImage(bt_global::skin->getImage("source_background"));

	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(center_icon);
	hbox->addWidget(center_right_button);
	hbox->addWidget(right_button);
	hbox->addStretch(1);

	right_button->hide();

	connect(this, SIGNAL(sourceStateChanged(bool)), SLOT(sourceStateChanged(bool)));
}

void MediaSource::sourceStateChanged(bool active)
{
	right_button->setVisible(active);
}


RadioSource::RadioSource(const QString &area, RadioSourceDevice *dev, Page *details) :
	AudioSource(area, dev, details)
{
	radio_info = new RadioInfo(bt_global::skin->getImage("source_background"));

	initBanner(bt_global::skin->getImage("turned_on"), bt_global::skin->getImage("turn_on"), bt_global::skin->getImage("previous"),
		   bt_global::skin->getImage("next"), bt_global::skin->getImage("details"));

	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(radio_info);
	hbox->addWidget(center_right_button);
	hbox->addWidget(right_button);
	hbox->addStretch(1);

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
