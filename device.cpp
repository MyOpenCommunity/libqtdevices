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
	if(!sv->initialized())
	    break;
	++(*svi);
    }
    if(!sv)
	_initialized = true ;
    delete svi;
    return 0;
}

bool device_status::initialized(void)
{
    return _initialized;
}

void device_status::invalidate(void)
{
    QPtrListIterator<stat_var> *svi = 
	new QPtrListIterator<stat_var>(vars);
    svi->toFirst();
    stat_var *sv ; 
    while( ( sv = svi->current() ) != 0)
	sv->invalidate();
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
    add_var((int)device_status_amplifier::AUDIO_LEVEL_INDEX,
	    new stat_var(stat_var::AUDIO_LEVEL, 1, 1, 31, 1));
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
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*stat);
    dsi->toFirst();
    device_status *ds ;
    while( ( ds = dsi->current() ) != 0) {
	QString msg;
	if(ds->initialized()) {
	    qDebug("device is already initialized");
	    emit(initialized(ds));
	} else {
	    qDebug("getting init message");
	    interpreter->get_init_message(ds, msg);
	    qDebug("init message is %s", msg.ascii());
	    emit(send_frame((char *)msg.ascii()));
	}
	++(*dsi);
    }
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
}

void device::frame_rx_handler(char *s)
{
    qDebug("device::frame_rx_handler");
    emit(handle_frame(s, stat));
}

void device::frame_event_handler(device_status *ds)
{
    qDebug("device::frame_event_handler");
    emit(status_changed(ds));
}

void device::set_frame_interpreter(frame_interpreter *fi)
{
    interpreter = fi;
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
    // Creare una luce con indirizzo 33
    interpreter = new frame_interpreter_lights(w, p, g);
    set_frame_interpreter(interpreter);
    stat->append(new device_status_light());
    stat->append(new device_status_dimmer());
    stat->append(new device_status_new_timed());
    stat->append(new device_status_dimmer100());
    connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)), 
	    interpreter, 
	    SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
    connect(interpreter, SIGNAL(frame_event(device_status *)), this, 
	    SLOT(frame_event_handler(device_status *)));
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
    connect(interpreter, SIGNAL(frame_event(device_status *)), this, 
	    SLOT(frame_event_handler(device_status *)));
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
    connect(interpreter, SIGNAL(frame_event(device_status *)), this, 
	    SLOT(frame_event_handler(device_status *)));
}
