//! This class describes a device cache

#ifndef __DEVICESCACHE_H__
#define __DEVICESCACHE_H__

#include "device_status.h"

#include <QMap>
#include <QString>

#include "device.h"


class DevicesCache : public QMap<QString, device*>
{
public:
	//! Constructor
	DevicesCache();
	//! Inits all devices
	void init_devices();
	//! Get radio
	device *get_radio_device(QString where);

	/// Get a thermal regulator device
	device *get_thermal_regulator(QString where, thermo_type_t type);

	//! Get a controlled temperature probe
	device *get_temperature_probe_controlled(QString where, thermo_type_t type,
		bool fancoil, QString ind_centrale, QString indirizzo);
	//! Get stand-alone or external temperature probe
	device *get_temperature_probe(QString w, bool external);
	//! Destroy device given key
	void put_device(QString k);
	//! Destructor
	~DevicesCache();
};


// Extern functions

QString get_device_key(QString who, QString where);
QString get_device_key(char *who, char *where);

namespace bt_global
{
	// The global declaration of devices_cache instance
	extern DevicesCache devices_cache;

	//! Add already created device to cache. Key is device's who+where
	// NOTE: be aware that the device argument can be destroyed if the device for
	// the key already exists.
	template<class T> T* add_device_to_cache(T *device)
	{
		QString orig_class_name = device->metaObject()->className();
		QString key = device->get_key();
		if (bt_global::devices_cache.contains(key))
		{
			delete device;
			device = static_cast<T*>(bt_global::devices_cache[key]);
		}
		else
			bt_global::devices_cache[key] = device;

		QString current_class_name = device->metaObject()->className();
		Q_ASSERT_X(orig_class_name == current_class_name, "bt_global::add_device_to_cache", "Device returned is different from the given one. Maybe two devices have the same address?");
		return device;
	}

}

#endif // __DEVICESCACHE_H__
