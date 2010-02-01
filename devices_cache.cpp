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

