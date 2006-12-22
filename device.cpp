// Device implementation
#include <qstring.h>
#include <qptrlist.h>
#include <qobject.h>

#include "openclient.h"
#include "device.h"
#include "frame_interpreter.h"
#include "device_cache.h"

// Status variable implementation
stat_var::stat_var(stat_var::type _t, int _val, int _min, int _max, int _step,
		   bool _i)
{
    t = _t;
    val = _val;
    min = _min;
    max = _max;
    step = _step;
    _initialized = _i;
}

stat_var::stat_var(stat_var::type _t)
{
    t = _t;
    val = 0;
    min = 0;
    max = 0xffffffff;
    step = 1;
    _initialized = false;
}

void stat_var::get_val(int& out)
{
    out = val;
}

int stat_var::get_val(void)
{
    return val;
}

void stat_var::set_val(int& in)
{
    if(in > max) in = max;
    if(in < min) in = min;
    _initialized = true ;
    val = in;
#if 0
    qDebug("stat_var::min = %d, stat_var::max = %d, stat_var::val = %d", 
	   min, max, val);
#endif
}

void stat_var::get_min(int& out)
{
    out = min;
}

void stat_var::set_min(int& in)
{
    //if(in > max) in = max;
    min = in;
}

void stat_var::get_max(int& out)
{
    out = max;
}

void stat_var::set_max(int& in)
{
    //if(in < min) in = max;
    max = in;
}

void stat_var::get_step(int& out)
{
    out = step;
}

int stat_var::get_step(void)
{
    return step;
}

void stat_var::set_step(int& in)
{
#if 0
    if(in > (max - min))
	in = max - min;
#endif
    step = in;
}

bool stat_var::initialized(void)
{
    return _initialized;
}

void stat_var::invalidate(void)
{
    _initialized = false ;
}

void stat_var::force_initialized(void)
{
    _initialized = true;
}

stat_var::~stat_var()
{
}

// Device status implementation
device_status::device_status(device_status::type _t)
{
    t = _t;
    vars.clear();
    vars.setAutoDelete(true);
    _initialized = false;
    _init_requested = false;
}

device_status::type device_status::get_type(void)
{
    return t;
}

void device_status::set_type(device_status::type& _t)
{
    t = _t;
}

bool device_status::add_var(int index, stat_var *v)
{
    return vars.insert(index, v);
}

int device_status::read(int index, stat_var& out)
{
    stat_var *ptr = vars.at(index);
    if(!ptr) return -1;
    out = *ptr;
    return 0;
}

int device_status::write_val(int index, stat_var& in)
{
    stat_var *ptr = vars.at(index);
#if 0
    qDebug("device_status::write_val(%d)", index);
#endif
    if(!ptr) return -1;
    int v ; in.get_val(v);
#if 0
    qDebug("input value = %d", v);
#endif
    ptr->set_val(v);
    in.set_val(v);
    if(_initialized) 
	return 0;
    // Fixme: do this better
    QPtrListIterator<stat_var> *svi = 
	new QPtrListIterator<stat_var>(vars);
    svi->toFirst();
    stat_var *sv ; 
    while( ( sv = svi->current() ) != 0) {
      if(!sv->initialized()) {
	  delete svi;
	  return 0;
      }
      ++(*svi);
    }
    qDebug("device_status::write_val(): all device status initialized!!");
    _initialized = true ;
    _init_requested = false ;
    delete svi;
    return 0;
}

bool device_status::initialized(void)
{
    return _initialized;
}

bool device_status::init_requested(void)
{
    return _init_requested;
}

void device_status::mark_init_requested(void)
{
    _init_requested = true;
}

void device_status::invalidate(void)
{
    QPtrListIterator<stat_var> *svi = 
	new QPtrListIterator<stat_var>(vars);
    svi->toFirst();
    stat_var *sv ; 
    while( ( sv = svi->current() ) != 0) {
	sv->invalidate();
	++(*svi);
    }
    delete svi;
}

void device_status::force_initialized(void)
{
    QPtrListIterator<stat_var> *svi = 
	new QPtrListIterator<stat_var>(vars);
    svi->toFirst();
    stat_var *sv ; 
    while( ( sv = svi->current() ) != 0) {
	sv->force_initialized();
	++(*svi);
    }
    _initialized = true;
    delete svi;
}

device_status::~device_status()
{
    // Val list is in auto-delete mode, nothing to do
}

// Device status for lights
device_status_light::device_status_light() : device_status(LIGHTS)
{
    add_var((int)device_status_light::ON_OFF_INDEX,
	    new stat_var(stat_var::ON_OFF, 0, 0, 1, 1));
}

// Device status for old dimmers
device_status_dimmer::device_status_dimmer() : device_status(DIMMER)
{
    add_var((int)device_status_dimmer::LEV_INDEX,
	    new stat_var(stat_var::LEV, 0, 0, 100, 10));
    add_var((int)device_status_dimmer::OLD_LEV_INDEX,
	    new stat_var(stat_var::OLD_LEV, 0, 0, 100, 10));
    add_var((int)device_status_dimmer::FAULT_INDEX,
	    new stat_var(stat_var::FAULT, 0, 0, 1, 1));
}

// Device status for dimmer100
device_status_dimmer100::device_status_dimmer100() : device_status(DIMMER100)
{
    add_var((int)device_status_dimmer100::LEV_INDEX,
	    new stat_var(stat_var::LEV, 0, 0, 100, 5));
    add_var((int)device_status_dimmer100::OLD_LEV_INDEX,
	    new stat_var(stat_var::OLD_LEV, 0, 0, 100, 5));
    add_var((int)device_status_dimmer100::SPEED_INDEX,
	    new stat_var(stat_var::SPEED, 1, 1, 255, 1));
    add_var((int)device_status_dimmer100::FAULT_INDEX,
	    new stat_var(stat_var::FAULT, 0, 0, 1, 1));
}

// Device status for new timed devices
device_status_new_timed::device_status_new_timed() : device_status(NEWTIMED)
{
    add_var((int)device_status_new_timed::HH_INDEX,
	    new stat_var(stat_var::HH, 0, 0, 23, 1));
    add_var((int)device_status_new_timed::MM_INDEX,
	    new stat_var(stat_var::MM, 0, 0, 59, 1));
    add_var((int)device_status_new_timed::SS_INDEX,
	    new stat_var(stat_var::SS, 0, 0, 59, 1));
}

// Device status for autom. devices
device_status_autom::device_status_autom() :
    device_status(AUTOM)
{
    add_var((int)device_status_autom::STAT_INDEX,
	    new stat_var(stat_var::STAT, 0, 0, 2, 1));
}

// Device status for temperature probe devices
device_status_temperature_probe::device_status_temperature_probe() : 
    device_status(TEMPERATURE_PROBE)
{
    add_var((int)device_status_temperature_probe::TEMPERATURE_INDEX,
	    new stat_var(stat_var::TEMPERATURE, 0, -999, 999, 1));
}

// Device status for amplifiers
device_status_amplifier::device_status_amplifier() : 
    device_status(AMPLIFIER)
{
    add_var((int)device_status_amplifier::ON_OFF_INDEX,
	    new stat_var(stat_var::ON_OFF, 0, 0, 1, 1));
    add_var((int)device_status_amplifier::AUDIO_LEVEL_INDEX,
	    new stat_var(stat_var::AUDIO_LEVEL, 0, 0, 31, 1));
}

// Device status for radios
device_status_radio::device_status_radio() :
    device_status(RADIO)
{
    add_var((int)device_status_radio::FREQ_INDEX,
	    new stat_var(stat_var::FREQ, 0, 0, 0x7fffffff, 1));
    add_var((int)device_status_radio::STAZ_INDEX,
	    new stat_var(stat_var::STAZ, 0, 0, 0xf, 1));
    add_var((int)device_status_radio::RDS0_INDEX,
	    new stat_var(stat_var::RDS0, 0, 0, 0x7fffffff, 1));
    add_var((int)device_status_radio::RDS1_INDEX,
	    new stat_var(stat_var::RDS1, 0, 0, 0x7fffffff, 1));
}

//! Device status for doorphone devices
device_status_doorphone::device_status_doorphone() :
    device_status(DOORPHONE)
{
    add_var((int)device_status_doorphone::PENDING_CALL_INDEX,
	    new stat_var(stat_var::PENDING_CALL, 0, 0, 1, 1));
}

// Device status for anti-intrusion system
device_status_impanti::device_status_impanti() : 
  device_status(IMPANTI)
{
    add_var((int)device_status_impanti::ON_OFF_INDEX,
	    new stat_var(stat_var::ON_OFF, 0, 0, 1, 1));
}

// Device status for anti-intrusion system (single area)
device_status_zonanti::device_status_zonanti() : 
  device_status(ZONANTI)
{
    add_var((int)device_status_zonanti::ON_OFF_INDEX,
	    new stat_var(stat_var::ON_OFF, 0, 0, 1, 1));
}

// Device status for thermal regulator
device_status_thermr::device_status_thermr() :
    device_status(THERMR)
{
    add_var((int)device_status_thermr::STAT_INDEX,
	    new stat_var(stat_var::STAT, 0, 0, 5, 1));
    add_var((int)device_status_thermr::LOCAL_INDEX,
	    new stat_var(stat_var::LOCAL, 0, 0, 13, 1));
    add_var((int)device_status_thermr::SP_INDEX,
	    new stat_var(stat_var::SP, 0, 0, 0x7fffffff, 1));
}

// Device status for modscen
device_status_modscen::device_status_modscen() :
    device_status(MODSCEN)
{
    add_var((int)device_status_modscen::STAT_INDEX,
	    new stat_var(stat_var::STAT, 0, 
			 device_status_modscen::PROGRAMMING_START, 
			 device_status_modscen::FULL, 1));
}

// Device status for sound matrix
device_status_sound_matr::device_status_sound_matr() :
    device_status(SOUNDMATR)
{
    add_var((int)device_status_sound_matr::AMB1_INDEX,
	    new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
    add_var((int)device_status_sound_matr::AMB2_INDEX,
	    new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
    add_var((int)device_status_sound_matr::AMB3_INDEX,
	    new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
    add_var((int)device_status_sound_matr::AMB4_INDEX,
	    new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
    add_var((int)device_status_sound_matr::AMB5_INDEX,
	    new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
    add_var((int)device_status_sound_matr::AMB6_INDEX,
	    new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
    add_var((int)device_status_sound_matr::AMB7_INDEX,
	    new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
    add_var((int)device_status_sound_matr::AMB8_INDEX,
	    new stat_var(stat_var::ACTIVE_SOURCE, 0, 1, 4, 1, false));
}

// Device implementation

device::device(QString _who, QString _where, bool p, int g)
{
    who = _who;
    where = _where;
    pul = p;
    group = g;
    refcount = 0;
    stat = new QPtrList<device_status>;
#if 0
    qDebug("device::device(), who = %s, where = %s", who.ascii(),
	   where.ascii());
#endif
}

void device::init(void)
{
    qDebug("device::init()");
    // True if all device has already been initialized
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*stat);
    dsi->toFirst();
    device_status *ds ;
    QPtrList<device_status> dsl;
    dsl.clear();
    while( ( ds = dsi->current() ) != 0) {
	//QString msg = "";
	QStringList msgl;
	msgl.clear();
	qDebug("ds = %p", ds);
	if(ds->initialized()) {
	    qDebug("device status hal already been initialized");
	    emit(initialized(ds));
	    dsl.append(ds);
	} else if(ds->init_requested()) {
	    qDebug("device status init already requested");
	} else {
	    qDebug("getting init message");
	    interpreter->get_init_messages(ds, msgl);
	    for ( QStringList::Iterator it = msgl.begin(); 
		  it != msgl.end(); ++it ) {
		qDebug("init message is %s", (*it).ascii());
		if((*it) != "")
		    emit(send_frame((char *)((*it).ascii())));
	    }
	    //ds->force_initialized();
	    ds->mark_init_requested();
	}
	++(*dsi);
    }
    if(!dsl.isEmpty())
	emit(status_changed(dsl));
    delete dsi;
    qDebug("device::init() end");
}

void device::init_requested_handler(QString msg)
{
    qDebug("device::init_requested_handler()");
    if(msg != "")
	emit(send_frame((char *)msg.ascii()));
}


void device::set_where(QString w)
{
    qDebug("device::set_where(%s)", w.ascii());
    where = w;
    if(interpreter)
	interpreter->set_where(w);
}

void device::set_pul(bool p)
{
    pul = p;
}

void device::set_group(int g)
{
    group = g;
}

void device::add_device_status(device_status *_ds)
{
    qDebug("device::add_device_status()");
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*stat);
    dsi->toFirst();
    device_status *ds ;
    while( ( ds = dsi->current() ) != 0) {
	if(ds->get_type() == _ds->get_type()) {
	    qDebug("Status already there, skip");
	    return;
	}
	++(*dsi);
    }
    stat->append(_ds);
    delete dsi;
}

QString device::get_key(void)
{
    return get_device_key(who, where);
}

device::~device()
{
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*stat);
    dsi->toFirst();
    device_status *ds ;
    while( ( ds = dsi->current() ) != 0) {
	delete ds;
	++(*dsi);
    }
    delete stat;
    delete dsi;
}

void device::frame_rx_handler(char *s)
{
    qDebug("device::frame_rx_handler");
    emit(handle_frame(s, stat));
}

void device::frame_event_handler(QPtrList<device_status> ds)
{
    qDebug("device::frame_event_handler");
    emit(status_changed(ds));
}

void device::set_frame_interpreter(frame_interpreter *fi)
{
    interpreter = fi;
    // Pass init_requested signals 
    connect(fi, SIGNAL(init_requested(QString)), 
	    this, SLOT(init_requested_handler(QString)));
}

void device::get(void)
{
    refcount++;
}

int device::put(void)
{
    refcount--;
    return refcount;
}

// Light implementation
light::light(QString w, bool p, int g) : device(QString("1"), w, p, g) 
{
    interpreter = new frame_interpreter_lights(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_light());
#if 0
    stat->append(new device_status_dimmer());
    stat->append(new device_status_new_timed());
    stat->append(new device_status_dimmer100());
#endif
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Dimmer implementation
dimm::dimm(QString w, bool p, int g) : device(QString("1"), w, p, g)
{
    interpreter = new frame_interpreter_lights(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_dimmer());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)),
            interpreter,
            SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this,
            SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Autom implementation
autom::autom(QString w, bool p, int g) :
device(QString("2"), w, p, g)
{
    interpreter = new frame_interpreter_autom(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_autom());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Temperature probe implementation
temperature_probe::temperature_probe(QString w, bool p, int g) : 
device(QString("4"), w, p, g) 
{
    interpreter = new frame_interpreter_temperature_probe(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_temperature_probe());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Sound device implementation
sound_device::sound_device(QString w, bool p, int g) : 
device(QString("16"), w, p, g) 
{
    interpreter = new frame_interpreter_sound_device(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_amplifier());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Radio device implementation
radio_device::radio_device(QString w, bool p, int g) : 
device(QString("16"), w, p, g) 
{
    interpreter = new frame_interpreter_radio_device(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_radio());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Sound matrix device implementation
sound_matr::sound_matr(QString w, bool p, int g) :
device(QString("16"), QString("1000"), p, g)
{
    interpreter = new frame_interpreter_sound_matr_device(
	QString("1000"), p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_sound_matr());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Doorphone device implementation
doorphone_device::doorphone_device(QString w, bool p, int g) : 
device(QString("6"), w, p, g)
{
    qDebug("doorphone_device::doorphone_device()");
    interpreter = new frame_interpreter_doorphone_device(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_doorphone());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Imp.anti device
impanti_device::impanti_device(QString w, bool p, int g) :
device(QString("16"), w, p, g)
{
    qDebug("impanti_device::impanti_device()");
    interpreter = new frame_interpreter_impanti_device(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_impanti());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Zon.anti device
zonanti_device::zonanti_device(QString w, bool p, int g) :
device(QString("5"), w, p, g)
{
    qDebug("zonanti_device::impanti_device()");
    interpreter = new frame_interpreter_zonanti_device(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_zonanti());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// thermal regulator device
thermr_device::thermr_device(QString w, bool p, int g) :
device(QString("4"), w, p, g)
{
    qDebug("thermr_device::thermr_device()");
    interpreter = new frame_interpreter_thermr_device(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_thermr());
    stat->append(new device_status_temperature_probe());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}

// modscen device
modscen_device::modscen_device(QString w, bool p, int g) :
device(QString("0"), w, p, g)
{
    qDebug("modscen_device::modscen_device()");
    interpreter = new frame_interpreter_modscen_device(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_modscen());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
	    SLOT(frame_event_handler(QPtrList<device_status>)));
}
