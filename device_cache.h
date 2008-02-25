//! This class describes a device cache

#ifndef __DEVICE_CACHE_H__
#define __DEVICE_CACHE_H__

#include <qmap.h>
#include <qstring.h>
#include "device.h"
#include "frame_interpreter.h"

class Client;

typedef device *deviceptr;

class device_cache : public QMap<QString, deviceptr>
{
private:
	Client * client_comandi;
	Client *  client_monitor;
	Client * client_richieste;
	//! Connect comm signals
	void connect_comm(device *);
	//! Disconnect comm signals
	void disconnect_comm(device *);
public:
	//! Constructor
	device_cache();
	//! Inits all devices
	void init_devices();
	//! Returns pointer to client_comandi
	Client *get_client_comandi();
	//! Returns pointer to client_monitor
	Client *get_client_monitor();
	//! Get device given key , create it if it doesn't exist
	device *get_device(QString k = "");
	//! Get simple light device
	device *get_light(QString where);
	//! Get old dimmer device
	device *get_dimmer(QString where);
	//! Get new dimmer device
	device *get_dimmer100(QString where);
	//! Get old timed device
	//device *get_oldtimed(QString where);
	//! Get new timed device
	device *get_newtimed(QString where);
	//! Get doorphone device
	device *get_doorphone_device(QString where);
	//! Get autom device
	device *get_autom_device(QString where);
	//! Get sound device
	device *get_sound_device(QString where);
	//! Get sound matr device
	device *get_sound_matr_device();
	//! Get radio
	device *get_radio_device(QString where);
	//! Get anti-intrusion system
	device *get_impanti_device();
	//! Get anti-intrusion system zone
	device *get_zonanti_device(QString where);
	//! Get thermal regulator
	device *get_thermr_device(QString where);
	//! Get modscen
	device *get_modscen_device(QString where);
	//! Destroy device given key
	void put_device(QString k);
	//! Add already created device to cache. Key is device's who+where
	deviceptr add_device(deviceptr d);
	//! Set pointers to clients
	void set_clients(Client* com, Client* mon, Client* ri);
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
