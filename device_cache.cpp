#include <qwidget.h>
#include "device_cache.h"

// Constructor
device_cache::device_cache() : QMap<QString, deviceptr>() 
{
    clear();
}

void device_cache::init_devices(void)
{
    qDebug("initializing devices");
    device_cache::Iterator it;
    for ( it = begin(); it != end(); ++it ) {
	it.data()->init();
    }
}

void device_cache::connect_comm(deviceptr dev)
{
    client_monitor->connect(client_monitor, SIGNAL(frameIn(char *)),
			    dev, SLOT(frame_rx_handler(char *)));
    client_comandi->connect(dev, SIGNAL(send_frame(char *)),
			    client_comandi, 
			    SLOT(ApriInviaFrameChiudi(char *)));
}

void device_cache::disconnect_comm(deviceptr dev)
{
    client_monitor->disconnect(client_monitor, SIGNAL(frameIn(char *)),
			       dev, SLOT(frame_rx_handler(char *)));
    client_comandi->disconnect(dev, SIGNAL(send_frame(char *)),
			       client_comandi, 
			       SLOT(ApriInviaFrameChiudi(char *)));
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
	// Connect to comm clients
	connect_comm(out);
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
	connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_light() returning %p", out);
    return out;
}

// Get old dimmer given key
deviceptr device_cache::get_dimmer(QString w)
{
    QString k = get_device_key(QString("1"), w);
    qDebug("device_cache::get_light(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new light(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    out->add_device_status(new device_status_dimmer());
    qDebug("device_cache::get_dimmer() returning %p", out);
    return out;
}

// Get new dimmer given key
deviceptr device_cache::get_dimmer100(QString w)
{
    QString k = get_device_key(QString("1"), w);
    qDebug("device_cache::get_light(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new light(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    out->add_device_status(new device_status_dimmer100());
    qDebug("device_cache::get_dimmer100() returning %p", out);
    return out;
}

// Get new timed device given key
deviceptr device_cache::get_newtimed(QString w)
{
    QString k = get_device_key(QString("1"), w);
    qDebug("device_cache::get_light(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new light(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    out->add_device_status(new device_status_new_timed());
    qDebug("device_cache::get_newtimed() returning %p", out);
    return out;
}

// Get doorphone device given address
deviceptr device_cache::get_doorphone_device(QString w)
{
    QString k = get_device_key(QString("6"), w);
    qDebug("device_cache::get_doorphone_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new doorphone_device(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	//connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_doorphone_device() returning %p", out);
    return out;
}

// Get autom device given address
deviceptr device_cache::get_autom_device(QString w)
{
    QString k = get_device_key(QString("2"), w);
    qDebug("device_cache::get_autom_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new autom(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_autom_device() returning %p", out);
    return out;
}

// Get amplifier
deviceptr device_cache::get_sound_device(QString w)
{
    QString k = get_device_key(QString("16"), w);
    qDebug("device_cache::get_sound_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new sound_device(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_sound_device() returning %p", out);
    return out;
}

// Get sound matrix device
deviceptr device_cache::get_sound_matr_device()
{
    QString k = get_device_key(QString("16"), QString("1000"));
    qDebug("device_cache::get_sound_matr_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new sound_matr(QString("1000"));
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_sound_matr_device() returning %p", out);
    return out;
}

// Get radio device
deviceptr device_cache::get_radio_device(QString w)
{
    // Radio devices are different, we build the key based only on 
    // the least significant digit
    int wh = w.toInt() ;
    while(wh >= 100)
	wh -= 100;
    while(wh >= 10)
	wh -= 10;
    w = QString::number(wh);
    qDebug("Modified w to %s", w.ascii());
    QString k = get_device_key(QString("16"), w);
    qDebug("device_cache::get_radio_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new radio_device(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_radio_device() returning %p", out);
    return out;
}

// Get anti-intrusion system
deviceptr device_cache::get_impanti_device()
{
    QString k = get_device_key(QString("16"), QString("0"));
    qDebug("device_cache::get_impanti_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new impanti_device(QString("0"));
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_impanti_device() returning %p", out);
    return out;
}

// Get anti-intrusion system (single zone)
deviceptr device_cache::get_zonanti_device(QString w)
{
    QString k = get_device_key(QString("16"), w);
    qDebug("device_cache::get_impanti_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new zonanti_device(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_zonanti_device() returning %p", out);
    return out;
}

// Get thermal regulator device
deviceptr device_cache::get_thermr_device(QString w)
{
    QString k = get_device_key(QString("4"), w);
    qDebug("device_cache::get_thermr_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new thermr_device(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_thermr_device() returning %p", out);
    return out;
}

// Get modscen device
deviceptr device_cache::get_modscen_device(QString w)
{
    QString k = get_device_key(QString("0"), w);
    qDebug("device_cache::get_modscen_device(%s)", k.ascii());
    deviceptr out = (*this)[k];
    if(!out) {
	out = new modscen_device(w);
	qDebug("device is not there, creating device %p", out);
	(*this)[k] = out;
	connect_comm(out);
    }
    out->get();
    qDebug("device_cache::get_modscen_device() returning %p", out);
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
    disconnect_comm(d);
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
	disconnect_comm(p);
	delete p;
	p = (*this)[k];
	goto end;
    }
    qDebug("device %p inserted", p);
    (*this)[k] = p;
 end:
    qDebug("device_cache::add_device() returning");
    connect_comm(p);
    return p;
}

void device_cache::set_clients(Client* com, Client* mon, Client* ri)
{
    client_comandi = com;
    client_monitor = mon;
    client_richieste = ri;
}

Client *device_cache::get_client_comandi()
{
    return client_comandi;
}

Client *device_cache::get_client_monitor()
{
    return client_monitor;
}

// Destructor
device_cache::~device_cache()
{
    QMap<QString, deviceptr>::Iterator it;
    for ( it = begin(); it != end(); ++it ) {
	erase(it);
	disconnect_comm(*it);
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
