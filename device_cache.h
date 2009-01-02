//! This class describes a device cache

#ifndef __DEVICE_CACHE_H__
#define __DEVICE_CACHE_H__

#include "device_status.h"

#include <QMap>
#include <QString>

class Client;
class device;


class device_cache : public QMap<QString, device*>
{
private:
	//! Connect comm signals
	void connect_comm(device *);
	//! Disconnect comm signals
	void disconnect_comm(device *);

	Client *client_comandi;
	Client *client_monitor;
	Client *client_richieste;
public:
	//! Constructor
	device_cache();
	//! Inits all devices
	void init_devices();
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
	/// Get a thermal regulator device
	device *get_thermal_regulator(QString where, thermo_type_t type);

	device *get_poweramplifier_device(QString w);

	//! Get a controlled temperature probe
	device *get_temperature_probe_controlled(QString where, thermo_type_t type,
		bool fancoil, QString ind_centrale, QString indirizzo);
	//! Get stand-alone or external temperature probe
	device *get_temperature_probe(QString w, bool external);
	//! Get modscen
	device *get_modscen_device(QString where);
	//! Get modscen
	device *get_mci_device(QString where);
	//! Destroy device given key
	void put_device(QString k);
	//! Add already created device to cache. Key is device's who+where
	device * add_device(device * d);
	//! Set pointers to clients
	void set_clients(Client* com, Client* mon, Client* ri);
	//! Destructor
	~device_cache();
};


// Extern functions

QString get_device_key(QString who, QString where);
QString get_device_key(char *who, char *where);


#endif // __DEVICE_CACHE_H__
