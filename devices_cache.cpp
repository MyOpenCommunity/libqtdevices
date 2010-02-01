#include "devices_cache.h"

#include <QDebug>



// Constructor
DevicesCache::DevicesCache() : QMap<QString, device*>()
{
	clear();
}

void DevicesCache::init_devices()
{
	qDebug("initializing devices");
	for (DevicesCache::Iterator it = begin(); it != end(); ++it)
		it.value()->init();
}

// Destructor
DevicesCache::~DevicesCache()
{
	for (QMap<QString, device *>::Iterator it = begin(); it != end(); ++it)
	{
		erase(it);
		delete *it;
	}
}


// The global definition of devices_cache
DevicesCache bt_global::devices_cache;

