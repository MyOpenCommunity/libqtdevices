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
		it.value()->deleteLater();

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

