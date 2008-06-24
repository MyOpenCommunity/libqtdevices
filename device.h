//! Header file for device classes

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "device_status.h"

#include <qstring.h>
#include <qptrlist.h>
#include <qobject.h>

class frame_interpreter;
class Client;

//! Generic device
class device : public QObject {
Q_OBJECT

public:
	//! Constructor
	device(QString who, QString where, bool p=false, int g=-1);
	//! Init device: send messages initializing data
	void init(bool force = false);
	//! Set frame interpreter
	void set_frame_interpreter(frame_interpreter *fi);
	//! Set where
	void set_where(QString);
	//! Set pul status
	void set_pul(bool);
	//! Set group
	void set_group(int);
	//! Add device status
	void add_device_status(device_status *);
	//! Increment reference count
	void get();
	//! Decrement reference count, return reference count after decrement
	int put();
	//! Returns cache key
	QString get_key(void);

	void setClients(Client *comandi, Client *monitor, Client *richieste);

	//! Destructor
	virtual ~device();

	void sendFrame(char *frame);
	void sendInit(char *frame);

signals:
	//! Status changed
	void status_changed(QPtrList<device_status>);
	//! Invoked after successful initialization
	void initialized(device_status *);
	//! We want to send a frame
	void send_frame(char *);
	//! We want a frame to be handled
	void handle_frame(char *, QPtrList<device_status> *);
public slots:
	//! receive a frame
	void frame_rx_handler(char *);
	//! Deal with frame event
	void frame_event_handler(QPtrList<device_status>);
	//! Initialization requested by frame interpreter
	void init_requested_handler(QString msg);

protected:
	//! Interpreter
	frame_interpreter *interpreter;
	//! List of device stats
	QPtrList<device_status> *stat;
private:
	//! Node's who
	QString who;
	//! Node's where
	QString where;
	//! Pul status
	bool pul;
	//! Device's group
	int group;
	//! Number of users
	int refcount;

	Client *client_comandi;
	Client *client_monitor;
	Client *client_richieste;
};

/********************* Specific class device children classes **********************/

//! Light (might be a simple light, a dimmer or a dimmer 100)
class light : public device
{
Q_OBJECT
public:
	//! Constructor
	light(QString, bool p=false, int g=-1);
	//! Destructor
	//virtual ~light();
};

//! Dimmer
class dimm : public device
{
Q_OBJECT
public:
	//! Constructor
	dimm(QString, bool p=false, int g=-1);
};

//! Dimmer
class dimm100 : public device
{
Q_OBJECT
public:
	//! Constructor
	dimm100(QString, bool p=false, int g=-1);
};

//! Automatismo
class autom : public device
{
	Q_OBJECT
	public:
		//! Constructor
		autom(QString, bool p=false, int g=-1);
};

/**
 * Controlled temperature probe device.
 */
class temperature_probe_controlled : public device
{
Q_OBJECT
public:
	//! Constructor
	temperature_probe_controlled(QString, thermo_type_t, bool fancoil,
		const char *ind_centrale, const char *indirizzo, bool p=false, int g=-1);
};

/**
 * Not controlled temperature probe device (external or internal).
 */
class temperature_probe_notcontrolled : public device
{
Q_OBJECT
public:
	//! Constructor
	temperature_probe_notcontrolled(QString, bool external, bool p=false, int g=-1);
};

//! Sound device (ampli)
class sound_device : public device
{
Q_OBJECT
public:
	//! Constructor
	sound_device(QString, bool p=false, int g=-1);
};

//! Radio
class radio_device : public device
{
Q_OBJECT
public:
	//! Constructor
	radio_device(QString, bool p=false, int g=-1);
};

//! Sound matrix
class sound_matr : public device
{
Q_OBJECT
public:
	//! Constructor
	sound_matr(QString, bool p=false, int g=-1);
};

//! Doorphone device
class doorphone_device : public device
{
Q_OBJECT
public:
	//! Constructor
	doorphone_device(QString, bool p=false, int g=-1);
};

//! ??
class impanti_device : public device
{
Q_OBJECT
public:
	//! Constructor
	impanti_device(QString, bool p=false, int g=-1);
};

//! ??
class zonanti_device : public device
{
Q_OBJECT
public:
	//! Constructor
	zonanti_device(QString, bool p=false, int g=-1);
};

//! Modscen device
class modscen_device : public device
{
Q_OBJECT
public:
	//! Constructor
	modscen_device(QString, bool p=false, int g=-1);
};

#endif //__DEVICE_H__
