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


#include "soundsources.h"
#include "media_device.h"
#include "page.h"
#include "state_button.h"
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h" // bt_global::skin
#include "btbutton.h"
#include "media_device.h" // SourceDevice, VirtualSourceDevice, RadioSourceDevice
#include "labels.h" // TextOnImageLabel
#include "radio.h" // RadioInfo
#include "devices_cache.h"
#include "multimedia_filelist.h"
#include "multimedia.h"
#include "radio.h" // RadioPage

#include <QDomNode>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QDebug>


AudioSource::AudioSource(const QString &_area, SourceDevice *_dev, Page *_details) :
	BannerNew(0)
{
	left_button = new StateButton;
	center_left_button = new StateButton;
	center_right_button = new StateButton;
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
	center_left_button->setOffImage(center_left);
	center_right_button->setOffImage(center_right);
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


RadioSource::RadioSource(const QString &area, RadioSourceDevice *dev, RadioPage *details) :
	AudioSource(area, dev, details)
{
	radio_info = new RadioInfo(bt_global::skin->getImage("source_background"), area, dev);

	initBanner(bt_global::skin->getImage("turned_on"), bt_global::skin->getImage("turn_on"),
		bt_global::skin->getImage("previous"), bt_global::skin->getImage("next"),
		bt_global::skin->getImage("details"));

	center_left_button->setDisabledImage(bt_global::skin->getImage("previous_disabled"));
	center_right_button->setDisabledImage(bt_global::skin->getImage("next_disabled"));
	center_left_button->setStatus(StateButton::DISABLED);
	center_left_button->disable();
	center_right_button->setStatus(StateButton::DISABLED);
	center_right_button->disable();

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

}

void RadioSource::showDetails()
{
	static_cast<RadioPage*>(details)->setArea(area);
	AudioSource::showDetails();
}

void RadioSource::sourceStateChanged(bool active)
{
	right_button->setVisible(active);
	if (active)
	{
		center_left_button->setStatus(StateButton::OFF);
		center_left_button->enable();
		center_right_button->setStatus(StateButton::OFF);
		center_right_button->enable();
	}
	else
	{
		center_left_button->setStatus(StateButton::DISABLED);
		center_left_button->disable();
		center_right_button->setStatus(StateButton::DISABLED);
		center_right_button->disable();
	}
}

void RadioSource::sourceHidden()
{
	radio_info->isShown(false);
}

void RadioSource::sourceShowed()
{
	radio_info->isShown(true);
}


SoundSources::SoundSources(const QString &source_address, const QString &area, const QList<SourceDescription> &src)
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setSpacing(10);

#ifdef LAYOUT_TS_3_5
	l->setContentsMargins(0, 0, 0, 5);
#endif

#ifdef LAYOUT_TS_10
	l->setContentsMargins(10, 0, 17, 10);

	BtButton *cycle = new BtButton(bt_global::skin->getImage("cycle"));
	connect(cycle, SIGNAL(clicked()), SLOT(sourceCycle()));
	l->addWidget(cycle);
#endif

	sources = new QStackedWidget;
	l->addWidget(sources);

	foreach (const SourceDescription &s, src)
	{
		SkinContext ctx(s.cid);
		AudioSource *w = NULL;

		if (s.type == SourceDescription::RADIO)
		{
			RadioSourceDevice *dev = bt_global::add_device_to_cache(new RadioSourceDevice(s.where));
			if (!s.details)
				s.details = new RadioPage(dev);

			w = new RadioSource(area, dev, static_cast<RadioPage*>(s.details));
		}
		else
		{
			// If the type is Multimedia but the address is not the BTouch one we
			// load a generic AuxSource combined with a SourceDevice (because the
			// aux source can manage all the remote commands of a multimedia remote source).
			if (s.type == SourceDescription::MULTIMEDIA && s.where == source_address)
			{
				if (!s.details)
				{
					MultimediaFileListFactory *factory = new MultimediaFileListFactory(TreeBrowser::DIRECTORY,
						EntryInfo::DIRECTORY | EntryInfo::AUDIO);
					s.details = new MultimediaSectionPage(getPageNode(MULTIMEDIA), MultimediaSectionPage::ITEMS_AUDIO, factory);
				}

				VirtualSourceDevice *dev = bt_global::add_device_to_cache(new VirtualSourceDevice(s.where));

				w = new MediaSource(area, dev, s.descr, s.details);
			}
			else
			{
				SourceDevice *dev = bt_global::add_device_to_cache(new SourceDevice(s.where));

				w = new AuxSource(area, dev, s.descr);
			}
		}

		sources->addWidget(w);
		connect(w, SIGNAL(sourceStateChanged(bool)), SLOT(sourceStateChanged(bool)));
		connect(w, SIGNAL(pageClosed()), SIGNAL(pageClosed()));
	}


}

void SoundSources::hideEvent(QHideEvent *)
{
	for (int i = 0; i < sources->count(); ++i)
		static_cast<AudioSource*>(sources->widget(i))->sourceHidden();
}

void SoundSources::showEvent(QShowEvent *)
{
	for (int i = 0; i < sources->count(); ++i)
		static_cast<AudioSource*>(sources->widget(i))->sourceShowed();
}

void SoundSources::sourceCycle()
{
	int index = sources->currentIndex();
	int next = (index + 1) % sources->count();

	sources->setCurrentIndex(next);
}

void SoundSources::sourceStateChanged(bool active)
{
	if (!active)
		return;

	AudioSource *source = static_cast<AudioSource*>(sender());

	sources->setCurrentWidget(source);
}

