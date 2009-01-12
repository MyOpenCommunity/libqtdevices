#include "device_cache.h"
#include "device.h"
#include "poweramplifier_device.h"

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
device_cache::device_cache() : QMap<QString, device*>()
{
	clear();
}

void device_cache::init_devices(void)
{
	qDebug("initializing devices");
	device_cache::Iterator it;
	for (it = begin(); it != end(); ++it)
		it.value()->init();
}

// Get light device given address
device *device_cache::get_light(QString w)
{
	QString k = get_device_key(QString("1"), w);
	qDebug() << "device_cache::get_light(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new light(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_light() returning %p", out);
	return out;
}

// Get old dimmer given key
device *device_cache::get_dimmer(QString w)
{
	QString k = get_device_key(QString("1"), w);
	qDebug() << "device_cache::get_dimmer(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new light(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	out->add_device_status(new device_status_dimmer());
	qDebug("device_cache::get_dimmer() returning %p", out);
	return out;
}

// Get new dimmer given key
device *device_cache::get_dimmer100(QString w)
{
	QString k = get_device_key(QString("1"), w);
	qDebug() << "device_cache::get_dimmer100(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new light(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	out->add_device_status(new device_status_dimmer100());
	qDebug("device_cache::get_dimmer100() returning %p", out);
	return out;
}

// Get new timed device given key
device *device_cache::get_newtimed(QString w)
{
	QString k = get_device_key(QString("1"), w);
	qDebug() << "device_cache::get_newtimed(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new light(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	out->add_device_status(new device_status_new_timed());
	qDebug("device_cache::get_newtimed() returning %p", out);
	return out;
}

// Get doorphone device given address
device *device_cache::get_doorphone_device(QString w)
{
	QString k = get_device_key(QString("6"), w);
	qDebug() << "device_cache::get_doorphone_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new doorphone_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_doorphone_device() returning %p", out);
	return out;
}

// Get autom device given address
device *device_cache::get_autom_device(QString w)
{
	QString k = get_device_key(QString("2"), w);
	qDebug() << "device_cache::get_autom_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new autom(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_autom_device() returning %p", out);
	return out;
}

// Get amplifier
device *device_cache::get_sound_device(QString w)
{
	QString k = get_device_key(QString("16"), w);
	qDebug() << "device_cache::get_sound_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new sound_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_sound_device() returning %p", out);
	return out;
}

// Get sound matrix device
device *device_cache::get_sound_matr_device()
{
	QString k = get_device_key(QString("16"), QString("1000"));
	qDebug() << "device_cache::get_sound_matr_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new sound_matr(QString("1000"));
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_sound_matr_device() returning %p", out);
	return out;
}

// Get radio device
device *device_cache::get_radio_device(QString w)
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
	qDebug() << "device_cache::get_radio_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new radio_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_radio_device() returning %p", out);
	return out;
}

// Get anti-intrusion system
device *device_cache::get_impanti_device()
{
	QString k = get_device_key(QString("16"), QString("0"));
	qDebug() << "device_cache::get_impanti_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new impanti_device(QString("0"));
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_impanti_device() returning %p", out);
	return out;
}

// Get anti-intrusion system (single zone)
device *device_cache::get_zonanti_device(QString w)
{
	QString k = get_device_key(QString("16"), w);
	qDebug() << "device_cache::get_zonanti_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new zonanti_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_zonanti_device() returning %p", out);
	return out;
}

device *device_cache::get_thermal_regulator(QString where, thermo_type_t type)
{
	QString k = get_device_key(QString("4"), where);
	device *out = (*this)[k];
	if (!out)
	{
		switch(type)
		{
			case THERMO_Z4:
				out = new thermal_regulator_4z(where);
				break;
			case THERMO_Z99:
				out = new thermal_regulator_99z(where);
				break;
		}
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;

	}
	out->get();
	qDebug("device_cache::get_thermal_regulator() returning %p", out);
	return out;
}

device *device_cache::get_poweramplifier_device(QString w)
{
	QString k = get_device_key(QString("22"), w);
	qDebug() << "device_cache::get_poweramplifier_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new poweramplifier_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_poweramplifier_device() returning %p", out);
	return out;
}

device *device_cache::get_temperature_probe_controlled(QString w, thermo_type_t type,
		bool fancoil, QString ind_centrale, QString indirizzo)
{
	QByteArray buf_centrale = ind_centrale.toAscii();
	QByteArray buf_indirizzo = indirizzo.toAscii();

	QString k = get_device_key(QString("4"), w);
	qDebug() << "device_cache::get_temperature_probe_controlled(), key=" << k
		<< " type=" << type << " fancoil=" << (fancoil ? "yes" : "no");
	device *out = (*this)[k];
	if (!out)
	{
		out = new temperature_probe_controlled(w, type, fancoil, buf_centrale.constData(), buf_indirizzo.constData());
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_temperature_probe_controlled() returning %p", out);
	return out;
}

// Get temperature probe device
device *device_cache::get_temperature_probe(QString w, bool external)
{
	QString k = get_device_key(QString("4"), w);
	qDebug() << "device_cache::get_temperature_probe(), key=" << k
		<< " external=" << (external ? "true" : "false");
	device *out = (*this)[k];
	if (!out)
	{
		out = new temperature_probe_notcontrolled(w, external);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_temperature_probe() returning %p", out);
	return out;
}


// Get modscen device
device *device_cache::get_modscen_device(QString w)
{
	QString k = get_device_key(QString("0"), w);
	qDebug() << "device_cache::get_modscen_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new modscen_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_modscen_device() returning %p", out);
	return out;
}

// Get MCI device
device *device_cache::get_mci_device(QString w)
{
	QString k = get_device_key(QString("18"), w);
	qDebug() << "device_cache::get_mci_device(" << k << ")";
	device *out = (*this)[k];
	if (!out)
	{
		out = new mci_device(w);
		qDebug("device is not there, creating device %p", out);
		(*this)[k] = out;
	}
	out->get();
	qDebug("device_cache::get_mci_device() returning %p", out);
	return out;
}

// Destroy device given key
void device_cache::put_device(QString k)
{
	qDebug() << "device_cache::put_device(" << k << ")";
	device *d = (*this)[k];
	if (!d)
	{
		qDebug("device_cache::put_device(), end");
		return;
	}
	qDebug("device reference count is 0, deleting device");
	remove(k);
	if (!d->put()) // Reference count is zero, delete object
		delete d;
	qDebug("device_cache::put_device(), end");
}

// Add already existing device to cache
device *device_cache::add_device(device *p)
{
	qDebug("device_cache::add_device(%p)", p);
	QString k = p->get_key();
	qDebug() << "key is " << k;
	if ((*this)[k])
	{
		qDebug("device is already there (%p), deleting %p", (*this)[k], p);
		delete p;
		p = (*this)[k];
	}
	else
	{
		qDebug("device %p inserted", p);
		(*this)[k] = p;
	}

	qDebug("device_cache::add_device() returning");
	return p;
}

// Destructor
device_cache::~device_cache()
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

