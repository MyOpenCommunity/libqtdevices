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


#include "devices_cache.h"


/*
 * Most of the time there will only be a single device in the system with a given where; however,
 * there can be multiple devices with the same where in the system with different classes; this happens
 * most frequently with device groups (eg. a ligh group might contain a dimmer) or with advanced scenarios
 * (eg. a scenario on amplifier status that uses a power amplifier).
 *
 * In this case the device cache will contain two instances, one per class type.  To avoid sending duplicate
 * frames either in initialization or in response to anoter frame, output frames are sent with a small delay,
 * and the client socket sends a frame at most once every time it flushes its internal queue.
 */

void DevicesCache::devicesCreated()
{
	devices_created = true;
}

void DevicesCache::setLazyUpdate(bool enable)
{
	qDebug("DevicesCache::setLazyUpdate(), set lazy update to = %d", enable);
}

void DevicesCache::checkLazyUpdate(int group)
{
	if (!lazy_update_list.isEmpty())
	{
		do
		{
			if(lazy_update_list.takeFirst()->smartInit(device::DEFERRED_INIT))
				group--;
		} while ((group > 0) && (!lazy_update_list.isEmpty()));
	}
}

void DevicesCache::initOpenserverDevices(int openserver_id)
{
	if (!devices_created)
	{
		qDebug("Delaying DevicesCache::initOpenserverDevices(), openserver_id = %d until loading completes", openserver_id);

		return;
	}

	qDebug("DevicesCache::initOpenserverDevices(), openserver_id = %d", openserver_id);

	FrameSender::delayFrames(true);
	foreach (const QString &frame, init_frames[openserver_id])
		FrameSender::sendCommandFrame(openserver_id, frame);

	if (!lazy_update_list.isEmpty())
		lazy_update_list.clear();

	for (QHash<QString, device*>::Iterator it = cache.begin(); it != cache.end(); ++it)
	{
		device *dev = it.value();
		if (dev->openserverId() == openserver_id)
		{
			if (!dev->smartInit(device::NORMAL_INIT))
			{
				lazy_update_list << dev;
			}
		}
	}
	FrameSender::delayFrames(false);
}

void DevicesCache::addInitCommandFrame(int openserver_id, const QString &frame)
{
	init_frames[openserver_id].append(frame);
}

void DevicesCache::removeInitCommandFrame(int openserver_id, const QString &frame)
{
	init_frames[openserver_id].removeOne(frame);
}

DevicesCache::DevicesCache()
{
	devices_created = false;
}

DevicesCache::~DevicesCache()
{
	clear();
}

void DevicesCache::clear()
{
	QMutableHashIterator<QString, device*> it(cache);
	while (it.hasNext())
	{
		it.next();
		it.value()->disconnect();
		delete it.value();
	}
	cache.clear();
}

device *DevicesCache::get(QString key) const
{
	return cache[key];
}

void DevicesCache::insert(QString key, device* d)
{
	cache.insert(key, d);
}


bool DevicesCache::contains(QString key) const
{
	return cache.contains(key);
}


// The global definition of devices_cache
DevicesCache bt_global::devices_cache;

// This object is a cache of devices, just like the bt_global::devices_cache.
// The difference is that the init methods of these devices is not call in this
// case.
DevicesCache DevicesCachePrivate::devices_cache_no_init;

