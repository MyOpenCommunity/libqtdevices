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


#ifndef __DEVICESCACHE_H__
#define __DEVICESCACHE_H__

#include <QHash>
#include <QString>

#include "device.h"

class DevicesCache;

namespace DevicesCachePrivate
{
	template<class T> T* add_device_to_cache(T *device, DevicesCache &cache);
}


enum DevicesCacheInitType
{
	INIT,
	NO_INIT
};

/**
 * This class describes a cache that must be used for all the devices of the
 * application, to avoid wasting memory and ensure the proper initialization.
 */
class DevicesCache
{
template<class T> friend T* DevicesCachePrivate::add_device_to_cache(T *device, DevicesCache &cache);

friend class TestScenEvoDevicesCond;
public:
	~DevicesCache();

	void initOpenserverDevices(int openserver_id);

	void addInitCommandFrame(int openserver_id, const QString &frame);

private:
	QHash<QString, device*> cache;
	QHash<int, QList<QString> > init_frames;

	void clear();
	bool contains(QString key) const;
	device *get(QString key) const;
	void insert(QString key, device* d);
};


namespace DevicesCachePrivate
{
	template<class T> T* add_device_to_cache(T *device, DevicesCache &cache)
	{
		QString orig_class_name = device->metaObject()->className();
		QString key = device->get_key();
		if (cache.contains(key))
		{
			delete device;
			device = static_cast<T*>(cache.get(key));
		}
		else
			cache.insert(key, device);

		QString current_class_name = device->metaObject()->className();
		Q_ASSERT_X(orig_class_name == current_class_name, "bt_global::add_device_to_cache",
				   "Device returned is different from the given one. Maybe two devices have the same address?");
		return device;
	}

	extern DevicesCache devices_cache_no_init;
}


namespace bt_global
{
	// The global declaration of devices_cache instance
	extern DevicesCache devices_cache;

	// Add an already created device to cache. The key is obtained using the methos
	// get_key of the device, and usually is composed by who + '*' + where.
	// NOTE: be aware that the device argument can be destroyed if the device for
	// the key already exists.
	template<class T> T* add_device_to_cache(T *device, DevicesCacheInitType init = INIT)
	{
		using DevicesCachePrivate::add_device_to_cache;
		if (init == INIT)
			return add_device_to_cache(device, devices_cache);
		else
			return add_device_to_cache(device, DevicesCachePrivate::devices_cache_no_init);

	}
}

#endif // __DEVICESCACHE_H__
