//! This class describes a device cache

#ifndef __DEVICE_CACHE_H__
#define __DEVICE_CACHE_H__

#include <qmap.h>
#include <qstring.h>
#include "device.h"
#include "frame_interpreter.h"


typedef device *deviceptr;

class device_cache : public QMap<QString, deviceptr> {
 private:
 public:
    //! Constructor
    device_cache();
    //! Get device given key , create it if it doesn't exist
    device *get_device(QString k = "");
    //! Get light device
    device *get_light(QString where);
    //! Destroy device given key
    void put_device(QString k);
    //! Add already created device to cache. Key is device's who+where
    deviceptr add_device(deviceptr d);
    //! Destructor
    ~device_cache();
};

// Global stuff

/*
 * Devices' cache
 */
extern device_cache btouch_device_cache;

// Extern functions

extern QString get_device_key(QString who, QString where);
extern QString get_device_key(char *who, char *where);
extern QString key_to_where(QString where);
extern QString key_to_who(QString who);


#endif // __DEVICE_CACHE_H__
