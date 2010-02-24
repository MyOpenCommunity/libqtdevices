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


namespace bt_global
{
	template<class T> T* add_device_to_cache(T *device);
}


/**
 * This class describes a cache that must be used for all the devices of the
 * application, to avoid wasting memory and ensure the proper initialization.
 */
class DevicesCache
{
template <class T> friend T* bt_global::add_device_to_cache(T *device);
friend class TestScenEvoDevicesCond;
public:
	~DevicesCache();

	//! Inits all devices
	void init_devices();

private:
	QHash<QString, device*> cache;
	void clear();
	bool contains(QString key) const;
	device *get(QString key) const;
	void insert(QString key, device* d);
};


namespace bt_global
{
	// The global declaration of devices_cache instance
	extern DevicesCache devices_cache;

	// Add an already created device to cache. The key is obtained using the methos
	// get_key of the device, and usually is composed by who + '*' + where.
	// NOTE: be aware that the device argument can be destroyed if the device for
	// the key already exists.
	template<class T> T* add_device_to_cache(T *device)
	{
		QString orig_class_name = device->metaObject()->className();
		QString key = device->get_key();
		if (bt_global::devices_cache.contains(key))
		{
			delete device;
			device = static_cast<T*>(bt_global::devices_cache.get(key));
		}
		else
			bt_global::devices_cache.insert(key, device);

		QString current_class_name = device->metaObject()->className();
		Q_ASSERT_X(orig_class_name == current_class_name, "bt_global::add_device_to_cache",
				   "Device returned is different from the given one. Maybe two devices have the same address?");
		return device;
	}
}

#endif // __DEVICESCACHE_H__
