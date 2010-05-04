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


AudioSource::AudioSource(const QString &_area, SourceDevice *_dev, Page *_details) :
	BannerNew(0)
{
	details = _details;
	area = _area;
	dev = _dev;

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceivedAudioSource(DeviceValues)));
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
		bool active = dev->isActive(area);

		emit sourceStateChanged(active);
	}
}

