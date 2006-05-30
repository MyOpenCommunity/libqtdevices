
#include "device_cache.h"

// Constructor
device_cache::device_cache() : QMap<QString, deviceptr>() 
{
    clear();
}

// Get device given key , create it if it doesn't exist
deviceptr device_cache::get_device(QString k)
{
    qDebug("device_cache::get_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new device(key_to_who(k), key_to_where(k));
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
    }
    // Increment device reference count
    out->get();
    qDebug("device_cache::get_device() returning %p", out);
    return out;
}

// Get light device given address
deviceptr device_cache::get_light(QString w)
{
    QString k = get_device_key(QString("1"), w);
    qDebug("device_cache::get_light(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new light(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
    }
    out->get();
    qDebug("device_cache::get_light() returning %p", out);
    return out;
}

// Destroy device given key
void device_cache::put_device(QString k)
{
    qDebug("device_cache::put_device(%s)", k.ascii());
    deviceptr d = (*this)[k];
    if(!d) 
	goto end;
    qDebug("device reference count is 0, deleting device");
    erase(k);
    if(!d->put())
	// Reference count is zero, delete object
	delete d;
 end:
    qDebug("device_cache::put_device(), end");
}

// Add already existing device to cache
deviceptr device_cache::add_device(deviceptr p)
{
    qDebug("device_cache::add_device(%p)", p);
    QString k = p->get_key();
    qDebug("key is %s", k.ascii());
    if((*this)[k]) { 
	qDebug("device is already there (%p), deleting %p", (*this)[k], p);
	delete p;
	p = (*this)[k];
	goto end;
    }
    qDebug("device %p inserted", p);
    (*this)[k] = p;
 end:
    qDebug("device_cache::add_device() returning");
    return p;
}

// Destructor
device_cache::~device_cache()
{
    QMap<QString, deviceptr>::Iterator it;
    for ( it = begin(); it != end(); ++it ) {
	erase(it);
	delete *it;
    }    
}


// Extern functions
QString get_device_key(QString who, QString where)
{
    return who + "*" + where ;
}

QString get_device_key(char *who, char *where)
{
    return get_device_key(QString(who), QString(where));
}

QString key_to_where(QString k)
{
    return k.left(k.find('*'));
}

QString key_to_who(QString k)
{
    return k.right(k.length() - k.find('*'));
}

// Global device cache: FIXME, MOVE THIS SOMEWHERE ELSE
device_cache btouch_device_cache;
