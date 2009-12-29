#include "devices_cache.h"
#include "thermal_device.h"
#include "probe_device.h"

#include <QWidget>
#include <QDebug>


static inline QString key_to_where(QString k)
{
	return k.left(k.indexOf('*'));
}

static inline QString key_to_who(QString k)
{
	return k.right(k.length() - k.indexOf('*'));
}


// Constructor
DevicesCache::DevicesCache() : QMap<QString, device*>()
{
	clear();
}

void DevicesCache::init_devices()
{
	qDebug("initializing devices");
	DevicesCache::Iterator it;
	for (it = begin(); it != end(); ++it)
		it.value()->init();
}

// Get doorphone device given address
device *DevicesCache::get_doorphone_device(QString w)
{
	QString k = get_device_key(QString("6"), w);
	qDebug() << "DevicesCache::get_doorphone_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new doorphone_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("DevicesCache::get_doorphone_device() returning %p", out);
	return out;
}

// Get amplifier
device *DevicesCache::get_sound_device(QString w)
{
	QString k = get_device_key(QString("16"), w);
	qDebug() << "DevicesCache::get_sound_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new sound_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("DevicesCache::get_sound_device() returning %p", out);
	return out;
}

// Get sound matrix device
device *DevicesCache::get_sound_matr_device()
{
	QString k = get_device_key(QString("16"), QString("1000"));
	qDebug() << "DevicesCache::get_sound_matr_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new sound_matr(QString("1000"));
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("DevicesCache::get_sound_matr_device() returning %p", out);
	return out;
}

// Get radio device
device *DevicesCache::get_radio_device(QString w)
{
	// Radio devices are different, we build the key based only on 
	// the least significant digit
	int wh = w.toInt() ;
	while(wh >= 100)
		wh -= 100;
	while(wh >= 10)
		wh -= 10;
	w = QString::number(wh);
	qDebug() << "Modified w to " << w;
	QString k = get_device_key(QString("16"), w);
	qDebug() << "DevicesCache::get_radio_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new radio_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("DevicesCache::get_radio_device() returning %p", out);
	return out;
}

// Get anti-intrusion system
device *DevicesCache::get_impanti_device()
{
	QString k = get_device_key(QString("16"), QString("0"));
	qDebug() << "DevicesCache::get_impanti_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new impanti_device(QString("0"));
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("DevicesCache::get_impanti_device() returning %p", out);
	return out;
}

// Get anti-intrusion system (single zone)
device *DevicesCache::get_zonanti_device(QString w)
{
	QString k = get_device_key(QString("16"), w);
	qDebug() << "DevicesCache::get_zonanti_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new zonanti_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("DevicesCache::get_zonanti_device() returning %p", out);
	return out;
}

device *DevicesCache::get_thermal_regulator(QString where, thermo_type_t type)
{
	QString k = get_device_key(QString("4"), where);
	device *out = (*this)[k];
	if (!out)
	{
		switch(type)
		{
			case THERMO_Z4:
				out = new ThermalDevice4Zones(where);
				break;
			case THERMO_Z99:
				out = new ThermalDevice99Zones(where);
				break;
		}
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;

	}
	out->get();
	qDebug("DevicesCache::get_thermal_regulator() returning %p", out);
	return out;
}


device *DevicesCache::get_temperature_probe_controlled(QString w, thermo_type_t type,
		bool fancoil, QString ind_centrale, QString indirizzo)
{
	QString k = get_device_key(QString("4"), w);
	qDebug() << "DevicesCache::get_temperature_probe_controlled(), key=" << k
		<< " type=" << type << " fancoil=" << (fancoil ? "yes" : "no");
	device *out = (*this)[k];
	if (!out)
	{
		out = new ControlledProbeDevice(w, ind_centrale, indirizzo,
						static_cast<ControlledProbeDevice::CentralType>(type),
						fancoil ? ControlledProbeDevice::FANCOIL : ControlledProbeDevice::NORMAL);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("DevicesCache::get_temperature_probe_controlled() returning %p", out);
	return out;
}

// Get temperature probe device
device *DevicesCache::get_temperature_probe(QString w, bool external)
{
	QString k = get_device_key(QString("4"), w);
	qDebug() << "DevicesCache::get_temperature_probe(), key=" << k
		<< " external=" << (external ? "true" : "false");
	device *out = (*this)[k];
	if (!out)
	{
		out = new NonControlledProbeDevice(w, external ? NonControlledProbeDevice::EXTERNAL : NonControlledProbeDevice::INTERNAL);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("DevicesCache::get_temperature_probe() returning %p", out);
	return out;
}


// Get MCI device
device *DevicesCache::get_mci_device(QString w)
{
	QString k = get_device_key(QString("18"), w);
	qDebug() << "DevicesCache::get_mci_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new mci_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("DevicesCache::get_mci_device() returning %p", out);
	return out;
}

// Destroy device given key
void DevicesCache::put_device(QString k)
{
	qDebug() << "DevicesCache::put_device(" << k << ")";
	device *d = (*this)[k];
	if (!d)
	{
		qDebug("DevicesCache::put_device(), end");
		return;
	}
	qDebug("device reference count is 0, deleting device");
	remove(k);
	if (!d->put()) // Reference count is zero, delete object
		delete d;
	qDebug("DevicesCache::put_device(), end");
}

// Destructor
DevicesCache::~DevicesCache()
{
	QMap<QString, device *>::Iterator it;
	for (it = begin(); it != end(); ++it)
	{
		erase(it);
		delete *it;
	}
}

// Extern functions
QString get_device_key(QString who, QString where)
{
	return who + "*" + where;
}

QString get_device_key(char *who, char *where)
{
	return get_device_key(QString(who), QString(where));
}

// The global definition of devices_cache
DevicesCache bt_global::devices_cache;

