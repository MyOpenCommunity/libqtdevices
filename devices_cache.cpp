#include "devices_cache.h"


void DevicesCache::init_devices()
{
	qDebug("initializing devices");
	for (DevicesCache::Iterator it = begin(); it != end(); ++it)
		it.value()->init();
}


DevicesCache::~DevicesCache()
{
	clear();
}

void DevicesCache::clear()
{
	for (QHash<QString, device *>::Iterator it = begin(); it != end(); ++it)
	{
		erase(it);
		delete *it;
	}
}


// The global definition of devices_cache
DevicesCache bt_global::devices_cache;

