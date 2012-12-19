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


/*!
	\ingroup Core
	\brief The type of the initialization to be performed for the device.
*/
enum DevicesCacheInitType
{
	INIT,      /*!< The standard initialization. */
	NO_INIT    /*!< Do not perform the initialization. */
};


/*!
	\ingroup Core
	\brief Describes a cache for the devices.
	\warning Do not use this class directly, instead use the add_device_to_cache() function.
*/
class DevicesCache
{
template<class T> friend T* DevicesCachePrivate::add_device_to_cache(T *device, DevicesCache &cache);

friend class TestScenEvoDevicesCond;
friend class TestVideoDoorEntryDevice;
friend class BtMain;
friend class TestDevice;
friend class TestBtObject;
friend class TestDevicesCache;
friend class TestDisplayControl;
public:
	DevicesCache();
	~DevicesCache();

	void initOpenserverDevices(int openserver_id);

	void addInitCommandFrame(int openserver_id, const QString &frame);

	// call this after all devices have been created
	void devicesCreated();

	// lazy update
	void setLazyUpdate(bool enable);
	void checkLazyUpdate(int group);

private:
	QHash<QString, device*> cache;
	QHash<int, QList<QString> > init_frames;
	bool devices_created;

	void clear();
	bool contains(QString key) const;
	device *get(QString key) const;
	void insert(QString key, device* d);

	QList<device*> lazy_update_list;
};


namespace DevicesCachePrivate
{
	template<class T> T* add_device_to_cache(T *device, DevicesCache &cache)
	{
		QString orig_class_name = device->metaObject()->className();
		QString key = device->get_key();
		if (cache.contains(key))
		{
			T *cached_device = static_cast<T*>(cache.get(key));
			if (cached_device != device)
				delete device;
			device = cached_device;
		}
		else
			cache.insert(key, device);

		return device;
	}

	extern DevicesCache devices_cache_no_init;
}


namespace bt_global
{
	// The global declaration of devices_cache instance
	extern DevicesCache devices_cache;

	/*!
		\ingroup Core
		\brief Add an already created device to cache.

		All the devices should be used this function, to avoid wasting memory and
		to ensure the proper initialization. The key for the cache is obtained
		using the method device::get_key()  and usually is composed by:
		\code
		openserver_id-who#where*class_name
		\endcode
		Devices added with second parameter as DevicesCacheInitType::NO_INIT
		are not initialized through the device::init() method.
		\warning be aware that the \a device argument can be destroyed if the device
		for the key already exists.
	*/
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
