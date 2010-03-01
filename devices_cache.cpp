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


void DevicesCache::init_devices()
{
	qDebug("initializing devices");
	for (QHash<QString, device*>::Iterator it = cache.begin(); it != cache.end(); ++it)
		it.value()->init();
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

