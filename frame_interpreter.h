//! Frame interpreter classes

#ifndef __FRAME_INTERPRETER_H__
#define __FRAME_INTERPRETER_H__

#include "openclient.h"
#include "device_status.h"

#include <QString>
#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <QList>

#include <stdlib.h>


class frame_interpreter;

//! Openwebnet where
class openwebnet_where : private QString
{
private:
	bool estesa;
public:
	//! Constructor
	openwebnet_where(char *);
	//! Constructor with string input
	openwebnet_where(QString);
	//! Returns true if where is a point to point one, outputs address
	bool pp(int& addr);
	//! Returns true if where is a general one
	bool gen(int& l, int& i);
	//! Returns true if where is an amb. one. Outputs amb number
	bool amb(int& a, int&l, int& i);
	//! Returns true if where is a gro. one. Outputs gro number
	bool gro(int& g);
	//! Returns level
	void lev(int& l);
	//! Returns interface
	void interf(int& l);
	//! Returns true if where is extended
	bool extended();
};


//! Extended openwebnet class, adds some useful methods to base openwebnet
class openwebnet_ext : public openwebnet
{
public:
	//! Constructor
	openwebnet_ext();
	//! Returns true if message is a general one. Also sets level and interf.
	bool gen(int& l, int& i);
	//! Returns true if message is an ambient one. Outputs amb. number, lev and interf.
	bool amb(int& a, int& l, int& i);
	//! Returns true if message is a group one. Outputs gr. number
	bool gro(int& g);
	//! Returns true if message is a point-to-point one. Outputs address
	bool pp(int& out);
	//! Returns target's extended where
	QString get_where();
	//! Returns true if frame_interpreter is a target for this message
	bool is_target(frame_interpreter *, bool& request_status);
	//! As above, but receives who also (i.e. doesn't read it from fr.int)
	bool is_target(frame_interpreter *fi, QString who, bool& request_status);
	//! As above, but discards message's where and considers wh instead
	bool is_target(frame_interpreter *fi, QString who, QString wh, bool& request_status);
	//! Returns true if message is extended
	bool extended();
};

struct deferred_list_element {
	QTime expires;
	device_status *ds;
};

//! Generic frame interpreter
class frame_interpreter : public QObject {
Q_OBJECT
protected:
	//! Who
	QString who;
	//! Where
	QString where;
	//! Pul status
	bool pul;
	//! Device's group
	int group;
private:
	//! List of device status for which init request must be deferred
	QList<deferred_list_element> deferred_list;
	//! Timer for deferred status request
	QTimer deferred_timer;
	//! Timer expiration time
	QTime deferred_timer_expires;
protected:
	//! Returns true when frame is ours
	virtual bool is_frame_ours(openwebnet_ext, bool& request_status);
	//! List of device_status objects to be passed on to clients
	// NOTE: The device_status are owned by device, so do not call delete on these!
	QList<device_status*> evt_list;

public:
	//! Constructor
	frame_interpreter(QString who, QString where, bool pul, int group);
	//! Returns who
	QString get_who();
	//! Returns where
	QString get_where();
	//! Returns pul
	bool get_pul();
	//! Returns group mask
	int get_group();
	//! Returns level
	int get_lev();
	//! Returns interface
	int get_interface();
	//! Returns init messages given device status
	virtual void get_init_messages(device_status *, QStringList&);
	//! Returns single init message
	virtual void get_init_message(device_status *, QString&);
	//! Set where
	void set_where(QString);
	//! Returns true if this f.i. belongs to group g
	bool belongs_to_group(int g);
	//! Returns relevant ambient
	int get_amb();
signals:
	//! An event was generated by this frame
	void frame_event(QList<device_status*>);
	//! Request device init
	void init_requested(QString);
protected slots:
	//! Request initialization
	void request_init(device_status *, int delay = 0);
	//! Request initialization after deferred time
	void deferred_request_init();
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Lights frame interpreter
class frame_interpreter_lights : public frame_interpreter
{
Q_OBJECT
private:
	//! Set status, lights
	void set_status(device_status_light *ds, int s);
	//! Analyze a frame for a pl status
	void handle_frame(openwebnet_ext, device_status_light *);
	//! Set status, dimmer
	void set_status(device_status_dimmer *ds, int s);
	//! Analyze a frame for a dimmer status
	void handle_frame(openwebnet_ext, device_status_dimmer *);
	//! Set status, dimmer 100
	void set_status(device_status_dimmer100 *ds, int lev, int speed);
	//! Analyze a frame for a dimmer 100 status
	void handle_frame(openwebnet_ext, device_status_dimmer100 *);
	//! Set status, new timed device
	void set_status(device_status_new_timed *ds, int hh, int mm, int ss, int on = -1);
	//! Analyze a frame for a ...
	void handle_frame(openwebnet_ext, device_status_new_timed *);
public:
	//! Constructor
	frame_interpreter_lights(QString, bool, int);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Dimmer frame interpreter
class frame_interpreter_dimmer : public frame_interpreter
{
Q_OBJECT
private:
	//! Set status, dimmer
	void set_status(device_status_dimmer *ds, int s);
	//! Analyze a frame for a dimmer status
	void handle_frame(openwebnet_ext, device_status_dimmer *);
public:
	//! Constructor
	frame_interpreter_dimmer(QString, bool, int);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Autom frame interpreter
class frame_interpreter_autom : public frame_interpreter
{
Q_OBJECT
private:
	//! Set status
	void set_status(device_status_autom *ds, int s);
	//! Analyze a frame for a pl status
	void handle_frame(openwebnet_ext, device_status_autom *);
public:
	//! Constructor
	frame_interpreter_autom(QString, bool, int);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Temperature probe frame interpreter
class frame_interpreter_temperature_probe : public frame_interpreter
{
Q_OBJECT
private:
	//! Set status, lights
	void set_status(device_status_temperature_probe *ds, int s);
	//! Analyze a frame for a pl status
	void handle_frame(openwebnet_ext, device_status_temperature_probe *);

	//! Whether the probe is of external (uncontrolled) type.
	bool external;
protected:
	//! Returns true when frame is ours (reimplemented for thermr, device
	bool is_frame_ours(openwebnet_ext, bool& request_status);
public:
	/**
	 * Constructor
	 * if \a external is true, the probe is of external (uncontrolled) type.
	 */
	frame_interpreter_temperature_probe(QString, bool external, bool, int);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Sound device frame interpreter
class frame_interpreter_sound_device : public frame_interpreter
{
Q_OBJECT
private:
	//! Set status, amplifier
	void set_status(device_status_amplifier *ds, int s, int on);
	//! Analyze a frame
	void handle_frame(openwebnet_ext, device_status_amplifier *);
public:
	//! Constructor
	frame_interpreter_sound_device(QString, bool, int);
	//! Returns init messages (more than one !!) given device status
	void get_init_messages(device_status *, QStringList&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Radio frame interpreter
class frame_interpreter_radio_device : public frame_interpreter
{
Q_OBJECT
private:
	//! Analyze a frame
	void handle_frame(openwebnet_ext, device_status_radio *);
public:
	//! Constructor
	frame_interpreter_radio_device(QString, bool, int);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
	//! Returns true if frame is ours
	bool is_frame_ours(openwebnet_ext, bool& request_status);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Sound matr interpreter
class frame_interpreter_sound_matr_device : public frame_interpreter
{
Q_OBJECT
private:
	//! Analyze a frame
	void handle_frame(openwebnet_ext, device_status_sound_matr *);
public:
	//! Constructor
	frame_interpreter_sound_matr_device(QString, bool, int);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
	//! Returns true if frame is ours
	//bool is_frame_ours(openwebnet_ext);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Doorphone device frame interpreter
class frame_interpreter_doorphone_device : public frame_interpreter
{
Q_OBJECT
private:
	//! Set status, doorphone
	void set_status(device_status_doorphone *ds, int s);
	//! Analyze a frame
	void handle_frame(openwebnet_ext, device_status_doorphone *);
public:
	//! Constructor
	frame_interpreter_doorphone_device(QString, bool, int);
	//! Returns true when frame is ours (reimplemented for doorphone dev.)
	bool is_frame_ours(openwebnet_ext m, bool& request_status);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Anti-intrusion system device frame interpreter
class frame_interpreter_impanti_device : public frame_interpreter
{
Q_OBJECT
private:
	//! Analyze a frame
	void handle_frame(openwebnet_ext, device_status_impanti *);
public:
	//! Constructor
	frame_interpreter_impanti_device(QString, bool, int);
	//! Returns true when frame is ours (reimplemented for impanti dev.)
	bool is_frame_ours(openwebnet_ext m, bool& request_status);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Anti-intrusion system device frame interpreter (single area)
class frame_interpreter_zonanti_device : public frame_interpreter
{
Q_OBJECT
private:
	//! Analyze a frame
	void handle_frame(openwebnet_ext, device_status_zonanti *);
public:
	//! Constructor
	frame_interpreter_zonanti_device(QString, bool, int);
	//! Returns true when frame is ours (reimplemented for zonanti dev.)
	bool is_frame_ours(openwebnet_ext m, bool& request_status);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Temperature probe device frame interpreter
class frame_interpreter_temperature_probe_controlled : public frame_interpreter
{
Q_OBJECT
private:
	//! Analyze a frame
	void handle_frame(openwebnet_ext, device_status_temperature_probe_extra *);
	//! As above, but for temperature status
	void handle_frame(openwebnet_ext, device_status_temperature_probe *);
	//! Analyze fancoil frame
	void handle_frame(openwebnet_ext m, device_status_fancoil *ds);

	//! True when frame has been analyzed
	bool elaborato;

	//! Type of thermal regulator
	thermo_type_t type;

	QString ind_centrale;
	QString indirizzo;

	/// Time to wait (in msec) before allowing another status request on the bus
	static const int TIMEOUT_TIME = 10000;
	/// This timer is used to compact all status requests done in TIMEOUT_TIME milliseconds in one frame only
	QTimer new_request_timer;
	/// If true, a new request frame can be sent
	bool new_request_allowed;
private slots:
	void timeoutElapsed();

protected:
	//! Returns true when frame is ours (reimplemented for thermr, device
	bool is_frame_ours(openwebnet_ext, bool& request_status);
public:
	//! Constructor
	frame_interpreter_temperature_probe_controlled(QString,
		thermo_type_t type,
		const char *ind_centrale, const char *indirizzo, bool, int);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! Modscen device frame interpreter
class frame_interpreter_modscen_device : public frame_interpreter
{
Q_OBJECT
private:
	//! Analyze a frame
	void handle_frame(openwebnet_ext, device_status_modscen *);
protected:
	//! Returns true when frame is ours (reimplemented for modscen device)
	//bool is_frame_ours(openwebnet_ext);
public:
	//! Constructor
	frame_interpreter_modscen_device(QString, bool, int);
	//! Returns init message given device status
	void get_init_message(device_status *, QString&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};

//! MCI frame interpreter
class frame_interpreter_mci : public frame_interpreter
{
Q_OBJECT
private:
	//! Analyze a frame for a dimmer status
	void handle_frame(openwebnet_ext, device_status_mci *);
public:
	//! Constructor
	frame_interpreter_mci(QString, bool, int);
	//! Returns init message given device status
  void get_init_messages(device_status *, QStringList&);
public slots:
	//! Receive a frame
	virtual void handle_frame_handler(char *, QList<device_status*>);
};


#endif //__FRAME_INTERPRETER_H__
