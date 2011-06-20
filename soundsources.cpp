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
#ifdef LAYOUT_TS_10
#include "multimedia_sectionpage.h"
#else
#include "multimedia_container.h"
#endif
#include "radio.h" // RadioPage
#ifdef LAYOUT_TS_3_5
#include "aux.h" // AuxPage
#endif

#include <QDomNode>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QDebug>


AudioSource::AudioSource(const QString &_area, SourceDevice *_dev, Page *_details) :
	BannerNew(0)
{
	details = _details;
	area = _area;
	dev = _dev;
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceivedAudioSource(DeviceValues)));

	left_button = new StateButton;
	right_button = new BtButton;
	connect(left_button, SIGNAL(clicked()), SLOT(turnOn()));
	connect(right_button, SIGNAL(clicked()), SLOT(showDetails()));

#ifdef LAYOUT_TS_3_5
	cycle_button = new BtButton(bt_global::skin->getImage("cycle"));
	connect(cycle_button, SIGNAL(clicked()), SIGNAL(sourceCycle()));
#else
	center_left_button = new StateButton;
	center_right_button = new StateButton;
	connect(center_left_button, SIGNAL(clicked()), dev, SLOT(prevTrack()));
	connect(center_right_button, SIGNAL(clicked()), dev, SLOT(nextTrack()));
#endif
}

#ifdef LAYOUT_TS_3_5
void AudioSource::drawBanner(const QString &description)
{
	left_button->setOffImage(bt_global::skin->getImage("turn_on"));
	right_button->setImage(bt_global::skin->getImage("details"));

	QGridLayout *grid = new QGridLayout(this);
	grid->setContentsMargins(0, 0, 0, 0);
	// We set no spacing because we want the text as large as the banner can.
	grid->setSpacing(0);
	grid->addWidget(left_button, 0, 0, 2, 1, Qt::AlignBottom);

	QLabel *l = new ScrollingLabel(description);
	l->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_TEXT));
	grid->addWidget(l, 0, 1, 1, 1, Qt::AlignHCenter);
	grid->addWidget(cycle_button, 1, 1, 1, 1, Qt::AlignHCenter);
	grid->addWidget(right_button, 0, 2, 2, 1, Qt::AlignBottom);
	grid->setColumnStretch(1, 1);
	grid->setColumnMinimumWidth(2, right_button->width());
	right_button->hide();
}
#else
void AudioSource::drawBanner(QWidget *central_widget)
{
	left_button->setOnImage(bt_global::skin->getImage("turned_on"));
	left_button->setOffImage(bt_global::skin->getImage("turn_on"));
	center_left_button->setOffImage(bt_global::skin->getImage("previous"));
	center_right_button->setOffImage(bt_global::skin->getImage("next"));
	right_button->setImage(bt_global::skin->getImage("details"));

	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->setSpacing(5);
	hbox->addWidget(left_button);
	hbox->addWidget(center_left_button);
	hbox->addWidget(central_widget);
	hbox->addWidget(center_right_button);
	hbox->addWidget(right_button);
	hbox->addStretch(1);

	right_button->hide();
}
#endif

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

#ifdef LAYOUT_TS_3_5
AuxSource::AuxSource(const QString &area, SourceDevice *dev, const QString &description, Page *details) :
	AudioSource(area, dev, details)
{
	drawBanner(description);
	connect(this, SIGNAL(sourceStateChanged(bool)), SLOT(sourceStateChanged(bool)));
}

void AuxSource::sourceStateChanged(bool active)
{
	right_button->setVisible(active);
}
#else
AuxSource::AuxSource(const QString &area, SourceDevice *dev, const QString &description) :
	AudioSource(area, dev)
{
	TextOnImageLabel *center_icon = new TextOnImageLabel(0, description);
	center_icon->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_TEXT));
	center_icon->setBackgroundImage(bt_global::skin->getImage("source_background"));

	drawBanner(center_icon);

}
#endif


MediaSource::MediaSource(const QString &area, VirtualSourceDevice *dev, const QString &description, Page *details) :
	AudioSource(area, dev, details)
{
#ifdef LAYOUT_TS_3_5
	drawBanner(description);
#else
	TextOnImageLabel *center_icon = new TextOnImageLabel(0, description);
	center_icon->setFont(bt_global::font->get(FontManager::AUDIO_SOURCE_TEXT));
	center_icon->setBackgroundImage(bt_global::skin->getImage("source_background"));
	drawBanner(center_icon);
#endif
	connect(this, SIGNAL(sourceStateChanged(bool)), SLOT(sourceStateChanged(bool)));
}

void MediaSource::sourceStateChanged(bool active)
{
	right_button->setVisible(active);
}


RadioSource::RadioSource(const QString &area, RadioSourceDevice *dev, const QString &description, RadioPage *details) :
	AudioSource(area, dev, details)
{
#ifdef LAYOUT_TS_3_5
	drawBanner(description);
#else
	Q_UNUSED(description);
	radio_info = new RadioInfo(area, dev);
	radio_info->setBackgroundImage(bt_global::skin->getImage("source_background"));

	drawBanner(radio_info);

	center_left_button->setDisabledImage(bt_global::skin->getImage("previous_disabled"));
	center_right_button->setDisabledImage(bt_global::skin->getImage("next_disabled"));
	center_left_button->setStatus(StateButton::DISABLED);
	center_left_button->disable();
	center_right_button->setStatus(StateButton::DISABLED);
	center_right_button->disable();
#endif
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
#ifdef LAYOUT_TS_10
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
#endif
}

#ifdef LAYOUT_TS_10
void RadioSource::sourceHidden()
{
	radio_info->isShown(false);
}

void RadioSource::sourceShowed()
{
	radio_info->isShown(true);
}
#endif


SoundSources::SoundSources(const QString &source_address, const QString &area, const QList<SourceDescription> &src)
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setSpacing(10);

#ifdef LAYOUT_TS_3_5
	l->setContentsMargins(0, 0, 0, 5);
#else
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
		AudioSource *w = 0;

		if (s.type == SourceDescription::RADIO)
		{
			RadioSourceDevice *dev = bt_global::add_device_to_cache(new RadioSourceDevice(s.where));
			if (!s.details)
				s.details = new RadioPage(dev);

			w = new RadioSource(area, dev, s.descr, static_cast<RadioPage*>(s.details));
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
#ifdef LAYOUT_TS_10
					s.details = new MultimediaSectionPage(getPageNode(MULTIMEDIA), MultimediaSectionPage::ITEMS_AUDIO, factory);
#else
					s.details = new MultimediaContainer(getPageNode(MULTIMEDIA));
#endif
				}

				VirtualSourceDevice *dev = bt_global::add_device_to_cache(new VirtualSourceDevice(s.where));

				w = new MediaSource(area, dev, s.descr, s.details);
			}
			else
			{
				SourceDevice *dev = bt_global::add_device_to_cache(new SourceDevice(s.where));
#ifdef LAYOUT_TS_3_5
				if (!s.details)
					s.details = new AuxPage(dev, s.descr);
				w = new AuxSource(area, dev, s.descr, s.details);
#else
				w = new AuxSource(area, dev, s.descr);
#endif
			}
		}

		sources->addWidget(w);
		connect(w, SIGNAL(sourceStateChanged(bool)), SLOT(sourceStateChanged(bool)));
		connect(w, SIGNAL(pageClosed()), SIGNAL(pageClosed()));
#ifdef LAYOUT_TS_3_5
		connect(w, SIGNAL(sourceCycle()), SLOT(sourceCycle()));
#endif
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

