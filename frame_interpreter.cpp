//! Implementation of frame interpreter classes

#include <qstring.h>
#include <qptrlist.h>
#include <qobject.h>
#include <stdlib.h>

#include "openclient.h"
#include "device.h"
#include "frame_interpreter.h"
#include "genericfunz.h"

// Openwebnet where class
openwebnet_where::openwebnet_where(char *s) : QString(s) 
{
}

openwebnet_where::openwebnet_where(QString s) : QString(s)
{
}

void openwebnet_where::lev(int& l)
{
    // Look for # from position 1
    int i = find('#', 1);
    l = i>=0 ? at(i + 1).digitValue() : 3;
    qDebug("*** level is %d", l);
}

void openwebnet_where::interf(int& i)
{
    //  Look for # from position 1
    int j = find('#', 1);
    if(j<0) {
	i = -1;
	return;
    }
    int k = find('#', j+1);
    i = k>=0 ? right(length() - k - 1).toInt() : -1;
    qDebug("*** interface is %d", i);
}

bool openwebnet_where::pp(int& addr)
{
    int trash;
    if(gen(trash, trash) || amb(trash, trash, trash) || gro(trash))
	return false;
    // Check this
    addr = strtoul(ascii(), NULL, 10);
    return true;
}

bool openwebnet_where::gen(int& l, int& i)
{
    l = -1;
    if((at(0)=='0' && length() == 1) || 
       (at(0)=='0' && length() >= 3 && length() <= 6)) {
	lev(l);
	interf(i);
	return true;
    }
    return false;
}
bool openwebnet_where::amb(int& a, int& l, int& i)
{
    qDebug("openwebnet_where:amb. Where is %s", ascii());
    if(*this == "00") { 
	// Diagnostic messages
	a = 0; 
	lev(l);
	interf(i);
	qDebug("amb00 !! (%d, %d)", l, i);
	return true;
    }
    //qDebug("amb(), this->ascii() = %s", ascii());
    //qDebug("at(0) = %d, length() = %d", at(0).digitValue(), length());
    if((at(0) >= '1') && (at(0) <= '9') && length() == 1) {
	a = QChar(at(0)) - '0';
	l = 3;
	i = -1;
	qDebug("amb ! (%d, %d, %d)", a, l, i);
	return true;
    }
    if((at(0) >= '1') && (at(0) <= '9') && (at(1) == '#')) {
	a = QChar(at(0)) - '0';
	lev(l);
	interf(i);
	qDebug("amb !! (%d, %d, %d)", a, l, i);
	return true;
    }
    return false;
}

bool openwebnet_where::gro(int& g)
{
    if((at(0) == '#') && (at(1) >= '1') && (at(1) <= '9')) {
	g = QChar(at(1)) - 0x30;
	return true;
    }
    return false;
}

bool openwebnet_where::extended(void)
{
    return find('#', 1) >= 0;
}

// Openwebnet extended class implementation
// Public methods

openwebnet_ext::openwebnet_ext() : openwebnet()
{
}

bool openwebnet_ext::is_target(frame_interpreter *fi, QString who, 
			       bool& request_status)
{
    request_status = false;
    if(fi->get_who() != who) return false;
    if(fi->get_pul()) return false;
    int l = 0, i = 0;
    if(gen(l, i)) {
	qDebug("gen!!");
	if((!extended() && l == 3) ||
	   (extended() && l == 3 && l == fi->get_lev()) || 
	   ((l == 4) && fi->get_lev() == 4 && i == fi->get_interface())) {
	    request_status = true;
	    return true;
	} else
	    return false;
    }
    int addr;
    if(pp(addr)) {
	qDebug("pp (%d)", addr);
	return fi->get_where() == get_where();
    }
    int a;
    if(amb(a, l, i)) {
	qDebug("amb(%d)", a);
	if(((l == 3 && l == fi->get_lev() && a == fi->get_amb()) || 
	    (l == 4 && a == fi->get_amb() && i == fi->get_interface()))) {
	    request_status = true;
	    return true;
	}
	return false;
    }
    int g;
    gro(g);
    qDebug("gro(%d)", g);
    // FIXME!!! : FIX belongs_to_group
    //request_status = true;
    return fi->belongs_to_group(g);
}

bool openwebnet_ext::is_target(frame_interpreter *fi, QString who, QString wh,
			       bool& request_status)
{
    request_status = false;
    if(fi->get_who() != who) return false;
    if(fi->get_pul()) return false;
    int l, i;
    if(gen(l, i)) {
	if((l == 3 && l == fi->get_lev()) || ((l == 4) && fi->get_lev() == 4 && 
			fi->get_interface() == i))
	    request_status = true;
	return true;
    }
    int addr;
    if(pp(addr)) 
	return fi->get_where() == wh;
    int a;
    if(amb(a, l, i)) {
	if((l == 3 && l == fi->get_lev() && a == fi->get_amb()) ||
	   (l == 4 && a == fi->get_amb() && i == fi->get_interface())) {
	    request_status = true;
	    return true;
	}
    }
    int g;
    gro(g);
    // FIXME!!! : FIX belongs_to_group
    request_status = true;
    //return fi->belongs_to_group(g);
    return true;
}

bool openwebnet_ext::is_target(frame_interpreter *fi, bool& request_status)
{
    return is_target(fi, QString(Extract_chi()), request_status);
}

QString openwebnet_ext::get_where(void)
{
    return estesa ? QString(Extract_dove()) + QString("#") + 
	QString(Extract_livello()) + QString("#") + 
	QString(Extract_interfaccia()) : QString(Extract_dove()) ;
}


// Private methods
bool openwebnet_ext::gen(int& l, int& i)
{
    openwebnet_where w(get_where());
    return w.gen(l, i);
}

bool openwebnet_ext::amb(int& a, int& l, int& i)
{
    openwebnet_where w(get_where());
    return w.amb(a, l, i);
}

bool openwebnet_ext::gro(int& g)
{
    openwebnet_where w(get_where());
    return w.gro(g);
}

bool openwebnet_ext::pp(int& addr)
{
    openwebnet_where w(get_where());
    return w.pp(addr);
}

bool openwebnet_ext::extended(void)
{
    openwebnet_where w(get_where());
    return w.extended();
}

// Generic frame interpreter

frame_interpreter::frame_interpreter(QString _who, QString _where, 
				     bool p, int g)
{
    who = _who;
    where = _where;
    pul = p;
    group = g;
    connect(&deferred_timer, SIGNAL(timeout()), this, 
	    SLOT(deferred_request_init()));
    qDebug("frame_interpreter::frame_interpreter()");
    qDebug("who = %s, where = %s", who.ascii(), where.ascii());
}

QString frame_interpreter::get_who(void)
{
    return who;
}

QString frame_interpreter::get_where(void)
{
    return where;
}

bool frame_interpreter::get_pul(void)
{
    return pul;
}

int frame_interpreter::get_amb(void)
{
    qDebug("frame_interpreter::get_amb");
    int a, l, i, trash ;
    openwebnet_where w(where);
    if(where == "00") {
	w.amb(a, l, i);
	return a;
    }
    if(w.gro(trash))
	return -1;
    if(w.gen(l, i))
	return -1;
    return where.at(0).digitValue();
}

int frame_interpreter::get_lev(void)
{
    int l ;
    openwebnet_where w(where);
    w.lev(l);
    return l;
}

int frame_interpreter::get_interface(void)
{
    int i;
    openwebnet_where w(where);
    w.interf(i);
    return i;
}

bool frame_interpreter::belongs_to_group(int g)
{
    // Group is not supported at the moment
    if(group < 0)
	return false;
    return (group & (1 << g));
}

bool frame_interpreter::is_frame_ours(openwebnet_ext m, bool& request_status)
{
    qDebug("frame_interpreter::is_frame_ours");
    qDebug("who = %s, where = %s", who.ascii(), where.ascii());
    qDebug("msg who = %s, msg where = %s", m.Extract_chi(), 
	   m.get_where().ascii());
    bool out = m.is_target(this, request_status);
    if(out) 
	qDebug("FRAME IS OURS !!");
    return out;
}

void frame_interpreter::request_init(device_status *ds, int delay)
{
    if(delay) {
      qDebug("delayed init request (%d) for %p", delay, ds);
      deferred_list_element *de = new deferred_list_element;
      de->ds = ds;
      deferred_list.append(de);
      de->expires = (QTime::currentTime()).addMSecs(delay);
      if(!deferred_timer.isActive()) {
	deferred_timer_expires = de->expires;
	deferred_timer.start(delay, TRUE);
	return;
      }
      if(de->expires < deferred_timer_expires) {
	deferred_timer_expires = de->expires;
	deferred_timer.changeInterval(delay);
      }
      return;
    }
    // No deferred timeout
    QStringList msgl;
    msgl.clear();
    get_init_messages(ds, msgl);
    for ( QStringList::Iterator it = msgl.begin(); it != msgl.end(); ++it ) {
	emit(init_requested(*it));
    }
}

void frame_interpreter::deferred_request_init(void)
{
    qDebug("frame_interpreter::deferred_request_init()");
    QPtrListIterator<deferred_list_element> *dli = 
	new QPtrListIterator<deferred_list_element>(deferred_list);
    device_status *ds;
    deferred_list_element *de;
    bool restart = false;
    int ms = -1;
    do {
      dli->toFirst();
      // Build an invalid time
      QTime next_expires = QTime(25, 61);
      while( ( de = dli->current() ) != 0) {
	ds = de->ds;
	if(QTime::currentTime().msecsTo(de->expires) <= 0) {
	  qDebug("requesting status");
	  request_init(de->ds, false);
	  deferred_list.remove(de);
	  delete de;
	  continue;
	}
	if((!next_expires.isValid()) || de->expires < next_expires)
	  next_expires = de->expires;
	++(*dli);
      }
      if(!next_expires.isValid()) {
	qDebug("no more deferred status requests");
	ms = -1;
      } else
	ms = QTime::currentTime().msecsTo(next_expires);
      // One more round if more deferred requests are ready
      restart = (ms <= 0) ;
    } while(!restart);
    if(ms > 0) {
      qDebug("restarting deferred request timer with %d ms delay", ms);
      deferred_timer.start(ms);
    }
    delete dli;
}

// This is reimplemented by children
void frame_interpreter::get_init_message(device_status *s, QString& out)
{
    out = "";
}

// This is reimplemented by some children only (those requiring more than 
// one init message
void frame_interpreter::get_init_messages(device_status *s, QStringList& out)
{
    out.clear();
    QString m;
    get_init_message(s, m);
    out.append(m);
}

// This is reimplemented by children
void frame_interpreter::handle_frame_handler(char *f, 
					      QPtrList<device_status> *sl)
{
    qDebug("frame_interpreter::handle_frame_handler");
}

// Set where
void frame_interpreter::set_where(QString s)
{
    qDebug("frame_interpreter::set_where(%s)", s.ascii());
    where = s;
}

// Lights frame interpreter

// Public methods
frame_interpreter_lights::frame_interpreter_lights(QString w, bool p, int g) :
    frame_interpreter(QString("1"), w, p, g)
{
}

void frame_interpreter_lights::get_init_message(device_status *s, QString& out)
{
    QString head = "*#1*";
    QString end ;
    switch(s->get_type()) {
    case device_status::LIGHTS:
    case device_status::DIMMER:
	end = "##";
	break;
    case device_status::DIMMER100:
	end = "*1##";
	break;
    case device_status::NEWTIMED:
	end = "*2##";
	break;
    default:
	out = "";
    }
    out = head + where + end;
}

void frame_interpreter_lights::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_lights::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    // Walk through list of device_status' and pass frame to relevant 
    // handler method
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    bool light_request = false;
    bool dimmer_request = false;
    device_status *light_ds;
    device_status *dimmer_ds;
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    // Frame could be ours, but we need to check device status 
	    // and see if it really changed
	    // Lights and old dimmers have the same status request message
	    // Just avoid sending the same request twice
	    if((ds->get_type() == device_status::LIGHTS) && !dimmer_request) {
		light_request = true;
		light_ds = ds;
	    }
	    if(ds->get_type() == device_status::DIMMER) {
		dimmer_request = true;
		light_request = false;
		dimmer_ds = ds;
	    }
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::LIGHTS:
		qDebug("**** LIGHTS *****");
		handle_frame(msg_open, (device_status_light *)ds);
		break;
	    case device_status::DIMMER:
		qDebug("**** DIMMER *****");
		handle_frame(msg_open, (device_status_dimmer *)ds);
		break;
	    case device_status::DIMMER100:
		qDebug("**** DIMMER100 *****");
		handle_frame(msg_open, (device_status_dimmer100 *)ds);
		break;
	    case device_status::NEWTIMED:
		qDebug("**** NEWTIMED *****");
		handle_frame(msg_open, (device_status_new_timed *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	++(*dsi);
    }
    if(dimmer_request) 
	request_init(dimmer_ds, dimmer_ds->init_request_delay());
    else if(light_request)
	request_init(light_ds, light_ds->init_request_delay());
    if(!evt_list.isEmpty()) {
	qDebug("Event list is not empty, invoking emit(frame_event())");
	emit(frame_event(evt_list));
    } else
	qDebug("**** NO event generated");
    delete dsi;
    qDebug("frame_interpreter_lights::handle_frame_handler, end");
}

// Private methods
void frame_interpreter_lights::set_status(device_status_light *ds, int s)
{
    qDebug("frame_interpreter_lights::set_status(device_status_ligh, %d)",
	   s);
    stat_var curr_stat(stat_var::ON_OFF);
    bool do_event = false;
    // Read current status of ON OFF variable
    ds->read((int)device_status_light::ON_OFF_INDEX, curr_stat);
    if(!curr_stat.initialized()) {
	qDebug("initializing status var!!\n");
	curr_stat.set_val(s);
	ds->write_val((int)device_status_light::ON_OFF_INDEX, 
		      curr_stat);
	do_event = true;
    }  else if(s && (!curr_stat.get_val())) {
	int v = 1;
	curr_stat.set_val(v);
	qDebug("setting light status to on");
	ds->write_val((int)device_status_light::ON_OFF_INDEX, 
		      curr_stat);
	do_event = true;
    } else if(!s && curr_stat.get_val()) {
	int v = 0;
	curr_stat.set_val(v);
	qDebug("setting light status to off");
	ds->write_val((int)device_status_light::ON_OFF_INDEX, 
		      curr_stat);
	do_event = true;
    }
    if(do_event) {
	qDebug("frame_interpreter_lights::set_status() (light), "
	       "appending evt");
	evt_list.append(ds);
    }
}


void frame_interpreter_lights::handle_frame(openwebnet_ext m, 
					    device_status_light *ds)
{
    qDebug("frame_interpreter_lights::handle_frame(), light");
    if(m.IsNormalFrame()) {
	int cosa = atoi(m.Extract_cosa());
	switch(cosa) {
	case 0:
	    // OFF 
	    set_status(ds, 0);
	    break;
	case 1:
	    // ON
	    set_status(ds, 1);
	    break;
	case 19:
	    // FAULT !!
	    qDebug("DIMMER FAULT, IGNORING BECAUSE THIS IS A LIGHT STATUS");
	    break;
	default:
	    // Dimmer, timed, ... FIXME: CHECK THESE
	    if((cosa >=2) && (cosa <= 31))
		set_status(ds, 1);
	    break;
	} 
    } else if(m.IsMeasureFrame()) {
	// *#1*where*1*lev*speed##
	int code = atoi(m.Extract_grandezza());
	int lev, hh, mm, ss;
	qDebug("frame_interpreter_lights::handle_frame, light, meas frame");
	switch(code) {
	case 1:
	    lev = atoi(m.Extract_valori(0)) - 100;
	    qDebug("new dimmer frame, level is %d", lev);
	    set_status(ds, lev);
	    break;
	case 2:
	    hh = atoi(m.Extract_valori(0));
	    mm = atoi(m.Extract_valori(1));
	    ss = atoi(m.Extract_valori(2));
	    qDebug("new timed frame (%d:%d:%d)", hh, mm, ss);
	    // FIXME: WHAT DO I DO WHEN hh==mm==ss==0 ?
	    if(((hh) && (mm) && (ss)))
		// Light is on
		set_status(ds, 1);
	    break;
	default:
	    qDebug("frame_interpreter_lights::handle_frame, light"
		   ", meas frame, code = %d", code);
	    break;
	}
    } else if(m.IsWriteFrame()) {
	qDebug("frame_interpreter_lights::handle_frame, light, write frame");
	int lev = atoi(m.Extract_valori(0)) - 100;
	set_status(ds, (lev > 0 ? 1 : 0));
    }
}

void frame_interpreter_lights::set_status(device_status_dimmer *ds, int lev)
{
    bool do_event = false;
    bool reinit = false;
    int fault_on = 1, fault_off = 0;
    // Read current status of variables
    stat_var curr_lev(stat_var::LEV), curr_old_lev(stat_var::OLD_LEV), 
      curr_fault(stat_var::FAULT);
    ds->read((int)device_status_dimmer::LEV_INDEX, curr_lev);
    ds->read((int)device_status_dimmer::OLD_LEV_INDEX, curr_old_lev);
    ds->read((int)device_status_dimmer::FAULT_INDEX, curr_fault);
    if((lev < 0) && !curr_fault.get_val()) {
      // FAULT
      qDebug("dimmer FAULT");
      curr_fault.set_val(fault_on);
      ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
      do_event = true;
      goto end;
    }
    if((lev >= 0) && curr_fault.get_val()) {
      // FAULT RECOVERY
      qDebug("dimmer FAULT RECOVERY");
      curr_fault.set_val(fault_off);
      ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
      reinit = true;
    }
    if((!ds->initialized() && (lev!=1)) || reinit) {
	curr_lev.set_val(lev);
	curr_old_lev.set_val(lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	ds->write_val((int)device_status_dimmer::OLD_LEV_INDEX, curr_old_lev);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	qDebug("initializing dimmer");
	do_event = true;
	goto end;
    }
    if(((lev == 1) && !curr_lev.get_val())) {
	// ON, restore old val
	qDebug("dimmer ON, restoring old value for level");
	int old_lev = curr_old_lev.get_val();
	curr_lev.set_val(old_lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	do_event = true ;
    }
    if(lev != curr_lev.get_val()) {
	int old_lev = curr_lev.get_val();
	curr_old_lev.set_val(old_lev);
	curr_lev.set_val(lev);
	qDebug("setting dimmer status to %d", lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	do_event = true ;
    }
 end:
    if(do_event) {
	qDebug("frame_interpreter_lights::set_status() (dimmer), "
	       "appending evt");
	evt_list.append(ds);
    }
}

void frame_interpreter_lights::handle_frame(openwebnet_ext m, 
					    device_status_dimmer *ds)
{
    qDebug("frame_interpreter_lights::handle_frame(), dimmer");
    stat_var sv(stat_var::LEV);
    if(m.IsNormalFrame()) {
	int cosa = atoi(m.Extract_cosa());
	switch(cosa) {
	case 0:
	    // OFF , 
	    set_status(ds, 0);
	    break;
	case 1:
	    // ON
	    if(ds->initialized()) {
		stat_var curr_lev(stat_var::LEV);
		ds->read((int)device_status_dimmer100::LEV_INDEX, curr_lev);
		if(curr_lev.get_val())
		    set_status(ds, 1);
		else 
		    request_init(ds);
	    } else {
		qDebug("emit(request_init(ds))");
		request_init(ds);
	    }
	    break;
	case 19:
	    // FAULT
	    set_status(ds, -1);
	    break;
	case 30:
	    // UP
	    ds->read((int)device_status_dimmer::LEV_INDEX, sv);
	    if(ds->initialized())
		set_status(ds, sv.get_val() + sv.get_step());
	    else
		request_init(ds);
	    break;
	case 31:
	    // DOWN
	    ds->read((int)device_status_dimmer::LEV_INDEX, sv);
	    if(ds->initialized())
		set_status(ds, sv.get_val() - sv.get_step());
	    else
		request_init(ds);
	    break;
	default:
	  if((cosa >=2) && (cosa <= 10)) {
		// Dimmer level
#if 0
		set_status(ds, (cosa - 1) * 10);
#else
	        qDebug("setting dimmer level to %d", cosa *10);
	        set_status(ds, cosa * 10);
#endif
	    } else if((cosa >= 11) && (cosa <= 19)) {
		// What shall we do here ?                
	        set_status(ds, 1);
	    } else if((cosa >= 20) && (cosa <= 29)) {
		// What shall we do here ?
	        set_status(ds, 1);
	    }
	    break;
	} 
    } else if(m.IsMeasureFrame()) {
	// *#1*where*1*lev*speed##
	int code = atoi(m.Extract_grandezza());
	int lev, hh, mm, ss;
	qDebug("frame_interpreter_lights::handle_frame, dimmer, meas frame");
	switch(code) {
	case 1:
	{
	    lev = atoi(m.Extract_valori(0)) - 100;
	    qDebug("lev = %d", lev);
	    // FIXME: CONVERT D100 LEVELS TO D10 LEVELS
	    int lev10 = lev/10;
	    if((lev%10) >= 5)
		lev10++;
	    qDebug("setting level = %d\n", lev10*10);
	    set_status(ds, lev10*10);
	    break;
	}
	case 2:
	    hh = atoi(m.Extract_valori(0));
	    mm = atoi(m.Extract_valori(1));
	    ss = atoi(m.Extract_valori(2));
	    qDebug("new timed frame (%d:%d:%d)", hh, mm, ss);
	    // FIXME: WHAT DO I DO WHEN hh==mm==ss==0 ?
	    if(((hh) && (mm) && (ss)))
		// Light is on
		set_status(ds, 1);
            qDebug("emit(request_init(ds))");
            request_init(ds);
	    break;
	default:
	    qDebug("frame_interpreter_lights::handle_frame, light"
		   ", meas frame, code = %d", code);
	    break;
	}
    } else if(m.IsWriteFrame()) {
	qDebug("frame_interpreter_lights::handle_frame, light, write frame");
	int lev = atoi(m.Extract_valori(0)) - 100;
    }
}

void frame_interpreter_lights::set_status(device_status_dimmer100 *ds,
					  int lev, int speed = -1)
{
    bool do_event = false;
    bool reinit = false;
    int fault_on = 1, fault_off = 0;
    qDebug("frame_interpreter_lights::set_status, dimmer100");
    stat_var curr_lev(stat_var::LEV), curr_old_lev(stat_var::OLD_LEV);
    stat_var curr_speed(stat_var::SPEED), curr_fault(stat_var::FAULT);
    // Read current status of status variables
    ds->read((int)device_status_dimmer100::LEV_INDEX, curr_lev);
    ds->read((int)device_status_dimmer100::OLD_LEV_INDEX, curr_old_lev);
    ds->read((int)device_status_dimmer100::SPEED_INDEX, curr_speed);
    ds->read((int)device_status_dimmer100::FAULT_INDEX, curr_fault);
    if((lev < 0) && !curr_fault.get_val()) {
      // FAULT
      qDebug("dimmer 100 FAULT");
      curr_fault.set_val(fault_on);
      ds->write_val((int)device_status_dimmer100::FAULT_INDEX, curr_fault);
      do_event = true;
      goto end;
    }
    if((lev >= 0) && curr_fault.get_val()) {
      // FAULT RECOVERY
      qDebug("dimmer 100 FAULT RECOVERY");
      curr_fault.set_val(fault_off);
      ds->write_val((int)device_status_dimmer100::FAULT_INDEX, curr_fault);
      reinit = true;
    }
    if(!curr_lev.initialized()) {
	curr_lev.set_val(lev);
	curr_old_lev.set_val(lev);
	ds->write_val((int)device_status_dimmer100::LEV_INDEX, curr_lev);
	ds->write_val((int)device_status_dimmer100::OLD_LEV_INDEX, 
		      curr_old_lev);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	do_event = true;
    }
    if(!curr_speed.initialized() && speed >= 0) {
	curr_speed.set_val(speed);
	ds->write_val((int)device_status_dimmer100::SPEED_INDEX, curr_speed);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	qDebug("setting speed to %d", speed);
	do_event = true;
    }
    if((lev == 1) && !curr_lev.get_val()) {
	// ON, restore old val
	int old_lev = curr_old_lev.get_val();
	qDebug("on, restoring old lev to %d", old_lev);
	curr_lev.set_val(old_lev);
	ds->write_val((int)device_status_dimmer100::LEV_INDEX, curr_lev);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	do_event = true ;
    }
    if(lev != curr_lev.get_val()) {
	int old_lev = curr_lev.get_val();
	curr_old_lev.set_val(old_lev);
	curr_lev.set_val(lev);
	qDebug("setting dimmer100 level to %d", lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	do_event = true ;
    }
    if((speed >= 0) && (speed != curr_speed.get_val())) {
	curr_speed.set_val(speed);
	qDebug("setting dimmer100 speed to %d", speed);
	ds->write_val((int)device_status_dimmer100::SPEED_INDEX, curr_speed);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	do_event = true ;
    }
 end:
    if(do_event) {
	qDebug("frame_interpreter_lights::set_status() (dimmer100), "
	       "appending evt");
	evt_list.append(ds);
    }
}

void frame_interpreter_lights::handle_frame(openwebnet_ext m, 
					    device_status_dimmer100 *ds)
{
    qDebug("frame_interpreter_lights::handle_frame(), dimmer 100");
    stat_var lev(stat_var::LEV), speed(stat_var::SPEED);
    if(m.IsNormalFrame()) {
	int cosa = atoi(m.Extract_cosa());
	switch(cosa) {
	case 0:
	    // OFF , 
	    set_status(ds, 0);
	    break;
	case 1:
	    // ON
	    if(ds->initialized()) {
		stat_var curr_lev(stat_var::LEV);
		ds->read((int)device_status_dimmer100::LEV_INDEX, curr_lev);
		if(curr_lev.get_val())
		    set_status(ds, 1);
		else 
		    request_init(ds);
	    } else {
		qDebug("emit(request_init(ds))");
		request_init(ds);
	    }
	    break;
	case 19:
	    // FAULT
	    set_status(ds, -1);
	    break;
	case 30:
	    // UP
	    if(ds->initialized()) {
		ds->read((int)device_status_dimmer::LEV_INDEX, lev);
		set_status(ds, lev.get_val() + lev.get_step());
	    } else 
		request_init(ds);
	    break;
	case 31:
	    // DOWN
	    if(ds->initialized()) {
		ds->read((int)device_status_dimmer::LEV_INDEX, lev);
		set_status(ds, lev.get_val() - lev.get_step());
	    } else 
		request_init(ds);
	    break;
	default:
	    if(cosa == 2)
		// Dimmer level
         	set_status(ds, 1);
            else if((cosa >= 3) && (cosa <= 8))
                // Dimmer level
                set_status(ds, (cosa - 2) * 10 );
            else if(cosa == 9)
                // Dimmer level
                set_status(ds, 75);
            else if(cosa == 10)
                // Dimmer level
                set_status(ds, 100);
	    else if((cosa >= 11) && (cosa <= 19)) {
		// What shall we do here ?
		set_status(ds, 1);
	    } else if((cosa >= 20) && (cosa <= 29)) {
		// What shall we do here ?
		set_status(ds, 1);
	    }
	    break;
	} 
    } else if(m.IsMeasureFrame()) {
	// *#1*where*1*lev*speed##
	int code = atoi(m.Extract_grandezza());
	int lev, speed, hh, mm, ss;
	qDebug("frame_interpreter_lights::handle_frame, dimmer, meas frame");
	switch(code) {
	case 1:
	    lev = atoi(m.Extract_valori(0)) - 100;
	    speed = atoi(m.Extract_valori(1));
	    set_status(ds, lev, speed);
	    break;
	case 2:
	    hh = atoi(m.Extract_valori(0));
	    mm = atoi(m.Extract_valori(1));
	    ss = atoi(m.Extract_valori(2));
	    qDebug("new timed frame (%d:%d:%d)", hh, mm, ss);
	    // FIXME: WHAT DO I DO WHEN hh==mm==ss==0 ?
	    if(((hh) && (mm) && (ss)))
		// Light is on
		set_status(ds, 1);
	    break;
	default:
	    qDebug("frame_interpreter_lights::handle_frame, light"
		   ", meas frame, code = %d", code);
	    break;
	}
    } else if(m.IsWriteFrame()) {
	qDebug("frame_interpreter_lights::handle_frame, light, write frame");
	int lev = atoi(m.Extract_valori(0));
	int speed = atoi(m.Extract_valori(1));
	set_status(ds, lev, speed);
    }
}

void frame_interpreter_lights::set_status(device_status_new_timed *ds, 
					  int hh, int mm, int ss, 
					  int on)
{
    qDebug("frame_interpreter_lights::set_status");
    qDebug("hh = %d, mm = %d, ss = %d", hh, mm, ss);
    bool do_event = false;
    stat_var curr_hh(stat_var::HH), curr_mm(stat_var::MM), 
	curr_ss(stat_var::SS);
#if 0
    if(!hh && !mm && !ss) {
	qDebug("Not an interesting message");
	goto end ;
    }
#endif
    // Read current status
    ds->read((int)device_status_new_timed::HH_INDEX, curr_hh);
    ds->read((int)device_status_new_timed::MM_INDEX, curr_mm);
    ds->read((int)device_status_new_timed::SS_INDEX, curr_ss);
    if(hh==-1 && mm==-1 && ss==-1) {
	// Time is invalid, ignore
	qDebug("hh, mm, ss, invalid: ignoring");
	goto end;
    }
    if(!curr_hh.initialized()) {
	qDebug("hh, mm, ss not yet initialized, initializing now");
	curr_hh.set_val(hh);
	curr_mm.set_val(mm);
	curr_ss.set_val(ss);
	do_event = true;
	goto end;
    }
    if((on != -1) && !curr_hh.get_val() && !curr_mm.get_val() && 
       !curr_ss.get_val()) {
	// On/off status change, with no time information
	curr_hh.set_val(hh);
	curr_mm.set_val(mm);
	curr_ss.set_val(ss);
	do_event = true ;
    }
    if((curr_hh.get_val() != hh) || (curr_mm.get_val() != mm) ||
       (curr_ss.get_val() != ss)) {
	qDebug("setting timing info to %d:%d:%d", hh, mm, ss);
	curr_hh.set_val(hh);
	curr_mm.set_val(mm);
	curr_ss.set_val(ss);
	do_event = true ;
    }
    qDebug("current hh:mm:ss = %02d:%02d:%02d", curr_hh.get_val(),
	   curr_mm.get_val(), curr_ss.get_val());
 end:
    if(do_event) {
	qDebug("timed device event");
	ds->write_val((int)device_status_new_timed::HH_INDEX, curr_hh);
	ds->write_val((int)device_status_new_timed::MM_INDEX, curr_mm);
	ds->write_val((int)device_status_new_timed::SS_INDEX, curr_ss);	
	qDebug("frame_interpreter_lights::set_status() (new timed), "
	       "appending evt");
	evt_list.append(ds);
    }
    qDebug("frame_interpreter_lights::set_status, end");
}

void frame_interpreter_lights::handle_frame(openwebnet_ext m, 
					    device_status_new_timed *ds)
{
    qDebug("frame_interpreter_lights::handle_frame(), new timed device");
    stat_var hh(stat_var::HH), mm(stat_var::MM), ss(stat_var::SS);
    if(m.IsNormalFrame()) {
	int cosa = atoi(m.Extract_cosa());
	switch(cosa) {
	case 0:
	    // OFF , 
	    set_status(ds, -1, -1, -1, 0);
	    break;
	case 1:
	    // ON
	    set_status(ds, -1, -1, -1, 1);
	    if(!ds->initialized())
		request_init(ds);
	    break;
	case 30:
	    // UP
	    set_status(ds, -1, -1, -1, 1);
	    if(!ds->initialized())
		request_init(ds);
	    break;
	case 31:
	    // DOWN
	    set_status(ds, -1, -1, -1, 1);
	    if(!ds->initialized())
		request_init(ds);
	    break;
	case 19:
	    // FAULT !!
	    qDebug("DIMMER FAULT, IGNORING BECAUSE THIS IS A LIGHT STATUS");
	    break;
	default:
	    if((cosa >=2) && (cosa <= 10))
		// Dimmer level
		set_status(ds, -1, -1, -1, 1);
	    else if((cosa >= 11) && (cosa < 19)) {
		// What shall we do here ?
		set_status(ds, -1, -1, -1, 1);
	    } else if((cosa >= 20) && (cosa <= 29)) {
		// What shall we do here ?
		set_status(ds, -1, -1, -1, 1);
	    }
	    if(!ds->initialized())
		request_init(ds);
	    break;
	} 
    } else if(m.IsMeasureFrame()) {
	// *#1*where*1*lev*speed##
	int code = atoi(m.Extract_grandezza());
	int lev, speed, hh, mm, ss;
	qDebug("frame_interpreter_lights::handle_frame, timed, meas frame");
	switch(code) {
	case 1:
	    set_status(ds, -1, -1, -1, 1);
	    if(!ds->initialized())
		request_init(ds);
	    break;
	case 2:
	    hh = atoi(m.Extract_valori(0));
	    mm = atoi(m.Extract_valori(1));
	    ss = atoi(m.Extract_valori(2));
	    qDebug("new timed frame (%d:%d:%d)", hh, mm, ss);
	    set_status(ds, hh, mm, ss, 1);
	    break;
	default:
	    qDebug("frame_interpreter_lights::handle_frame, light"
		   ", meas frame, code = %d", code);
	    break;
	}
    } else if(m.IsWriteFrame()) {
	qDebug("frame_interpreter_lights::handle_frame, light, write frame");
	int lev = atoi(m.Extract_valori(0));
	set_status(ds, -1, -1, -1, (lev > 0 ? 1 : 0));
	if(!ds->initialized())
	    request_init(ds);
    }
}


// Dimmers frame interpreter

// Public methods
frame_interpreter_dimmer::frame_interpreter_dimmer(QString w, bool p, int g) :
    frame_interpreter(QString("1"), w, p, g)
{
}

void frame_interpreter_dimmer::get_init_message(device_status *s, QString& out)
{
  out = "*#1*" + where + "##";
}

void frame_interpreter_dimmer::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_dimmer::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    // Walk through list of device_status' and pass frame to relevant 
    // handler method
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    bool dimmer_request = false;
    device_status *dimmer_ds;
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    // Frame could be ours, but we need to check device status 
	    // and see if it really changed
	    // Lights and old dimmers have the same status request message
	    // Just avoid sending the same request twice
	    if(ds->get_type() == device_status::DIMMER) {
		dimmer_request = true;
		dimmer_ds = ds;
	    }
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::DIMMER:
		qDebug("**** DIMMER *****");
		handle_frame(msg_open, (device_status_dimmer *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	++(*dsi);
    }
    if(dimmer_request) 
	request_init(dimmer_ds, dimmer_ds->init_request_delay());
	if(!evt_list.isEmpty()) {
	qDebug("Event list is not empty, invoking emit(frame_event())");
	emit(frame_event(evt_list));
    } else
	qDebug("**** NO event generated");
    delete dsi;
    qDebug("frame_interpreter_lights::handle_frame_handler, end");
}

// Private methods
void frame_interpreter_dimmer::set_status(device_status_dimmer *ds, int lev)
{
    bool do_event = false;
    bool reinit = false;
    int fault_on = 1, fault_off = 0;
    // Read current status of variables
    stat_var curr_lev(stat_var::LEV), curr_old_lev(stat_var::OLD_LEV), 
      curr_fault(stat_var::FAULT);
    ds->read((int)device_status_dimmer::LEV_INDEX, curr_lev);
    ds->read((int)device_status_dimmer::OLD_LEV_INDEX, curr_old_lev);
    ds->read((int)device_status_dimmer::FAULT_INDEX, curr_fault);
    if((lev < 0) && !curr_fault.get_val()) {
      // FAULT
      qDebug("dimmer FAULT");
      curr_fault.set_val(fault_on);
      ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
      do_event = true;
      goto end;
    }
    if((lev >= 0) && curr_fault.get_val()) {
      // FAULT RECOVERY
      qDebug("dimmer FAULT RECOVERY");
      curr_fault.set_val(fault_off);
      ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
      reinit = true;
    }
    if((!ds->initialized() && (lev!=1)) || reinit) {
	curr_lev.set_val(lev);
	curr_old_lev.set_val(lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	ds->write_val((int)device_status_dimmer::OLD_LEV_INDEX, curr_old_lev);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	qDebug("initializing dimmer");
	do_event = true;
	goto end;
    }
    if(((lev == 1) && !curr_lev.get_val())) {
	// ON, restore old val
	qDebug("dimmer ON, restoring old value for level");
	int old_lev = curr_old_lev.get_val();
	curr_lev.set_val(old_lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	do_event = true ;
    }
    if(lev != curr_lev.get_val()) {
	int old_lev = curr_lev.get_val();
	curr_old_lev.set_val(old_lev);
	curr_lev.set_val(lev);
	qDebug("setting dimmer status to %d", lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	curr_fault.set_val(fault_off);
	ds->write_val((int)device_status_dimmer::FAULT_INDEX, curr_fault);
	do_event = true ;
    }
 end:
    if(do_event) {
	qDebug("frame_interpreter_lights::set_status() (dimmer), "
	       "appending evt");
	evt_list.append(ds);
    }
}

void frame_interpreter_dimmer::handle_frame(openwebnet_ext m, 
					    device_status_dimmer *ds)
{
    qDebug("frame_interpreter_dimmer::handle_frame(), dimmer");
    stat_var sv(stat_var::LEV);
    if(m.IsNormalFrame()) {
	int cosa = atoi(m.Extract_cosa());
	switch(cosa) {
	case 0:
	    // OFF , 
	    set_status(ds, 0);
	    break;
	case 1:
	    // ON
	    if(ds->initialized()) {
		stat_var curr_lev(stat_var::LEV);
		ds->read((int)device_status_dimmer100::LEV_INDEX, curr_lev);
		if(curr_lev.get_val())
		    set_status(ds, 1);
		else 
		    request_init(ds);
	    } else {
		qDebug("emit(request_init(ds))");
		request_init(ds);
	    }
	    break;
	case 19:
	    // FAULT
	    set_status(ds, -1);
	    break;
	case 30:
	    // UP
	    ds->read((int)device_status_dimmer::LEV_INDEX, sv);
	    if(ds->initialized())
		set_status(ds, sv.get_val() + sv.get_step());
	    else
		request_init(ds);
	    break;
	case 31:
	    // DOWN
	    ds->read((int)device_status_dimmer::LEV_INDEX, sv);
	    if(ds->initialized())
		set_status(ds, sv.get_val() - sv.get_step());
	    else
		request_init(ds);
	    break;
	default:
	  if((cosa >=2) && (cosa <= 10)) {
		// Dimmer level
#if 0
		set_status(ds, (cosa - 1) * 10);
#else
	        qDebug("setting dimmer level to %d", cosa *10);
	        set_status(ds, cosa * 10);
#endif
	    } else if((cosa >= 11) && (cosa <= 19)) {
		// What shall we do here ?                
	        set_status(ds, 1);
	    } else if((cosa >= 20) && (cosa <= 29)) {
		// What shall we do here ?
	        set_status(ds, 1);
	    }
	    break;
	} 
    } else if(m.IsMeasureFrame()) {
	// *#1*where*1*lev*speed##
	int code = atoi(m.Extract_grandezza());
	int lev, hh, mm, ss;
	qDebug("frame_interpreter_dimmer::handle_frame, dimmer, meas frame");
	qDebug("emit(request_init(ds))");
	request_init(ds);
    } else if(m.IsWriteFrame()) {
	qDebug("frame_interpreter_dimmer::handle_frame, light, write frame");
	qDebug("emit(request_init(ds))");
	request_init(ds);

    }
}

// Temperature probe frame interpreter

// Public methods
frame_interpreter_temperature_probe::frame_interpreter_temperature_probe(QString w, bool p, int g) :
    frame_interpreter(QString("4"), w, p, g)
{
}

void frame_interpreter_temperature_probe::get_init_message(device_status *s, QString& out)
{
    QString head = "*#4*";
    QString end = "##";
    out = head + where + end;
}

// Private methods
void frame_interpreter_temperature_probe::set_status(device_status_temperature_probe *ds, int t)
{
    qDebug("frame_interpreter_temperature_probe::set_status"
	   "(device_status_temperature_probe, %d)", t);
    stat_var curr_temp(stat_var::TEMPERATURE);
    bool do_event = false;
    // Read current temperature
    ds->read((int)device_status_temperature_probe::TEMPERATURE_INDEX, 
	     curr_temp);
    if(!curr_temp.initialized()) {
	qDebug("Initializing temperature");
	curr_temp.set_val(t);
	ds->write_val((int)device_status_temperature_probe::TEMPERATURE_INDEX,
		      curr_temp);
	do_event = true;
	goto end;
    }
    qDebug("curr temp is %d\n", curr_temp.get_val());
    if(t != curr_temp.get_val()) {
	qDebug("setting temperature to %d", t);
	curr_temp.set_val(t);
	ds->write_val((int)device_status_temperature_probe::TEMPERATURE_INDEX, 
		      curr_temp);
	do_event = 1;
	qDebug("new temp is %d\n", curr_temp.get_val());
    }
 end:
    if(do_event)
	evt_list.append(ds);
}

void frame_interpreter_temperature_probe::handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status = false;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_temperature_probe::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    evt_list.clear();
    device_status *ds = dsi->current();
    if(request_status) {
	// Frame could be ours, but we need to check device status 
	// and see if it really changed
	request_init(ds);
	goto end;
    }
    // Just one device status at the moment
    if(msg_open.IsMeasureFrame()) {
	// *#4*where*what*???##
	int code = atoi(msg_open.Extract_grandezza());
	switch(code) {
	case 0:
	    int temperature;
	    char t[20]; strcpy(t, msg_open.Extract_valori(0));
	    temperature = t[0]=='1' ? -atoi(&t[1]) : atoi(t);
	    qDebug("temperature probe frame, t is %d", temperature);
	    set_status((device_status_temperature_probe *)ds, temperature);
	    break;
	default:
	    qDebug("temperature probe frame with unknown code %d", code);
	}
     } else
	 qDebug("unknown temperature frame");
 end:
     if(!evt_list.isEmpty())
	emit(frame_event(evt_list));
     delete dsi;
}

bool frame_interpreter_temperature_probe::is_frame_ours(openwebnet_ext m, bool& request_status)
{
  // FIXME: IS THIS OK ?
  qDebug("frame_interpreter_temperature_probe::is_frame_ours");
  qDebug("who = %s, where = %s", who.ascii(), where.ascii());
  qDebug("msg who = %s, msg where = %s", m.Extract_chi(), m.get_where().ascii());
  request_status = false;
  if (strcmp(m.Extract_chi(),"4"))
    return false;
  char dove[30];
  strcpy(dove, m.Extract_dove());
  if (dove[0]=='#')
    strcpy(&dove[0], &dove[1]);
  if(!strcmp(dove, "0"))
  {
    char pippo[50];
    // Richiesta via centrale
    memset(pippo,'\000',sizeof(pippo));
    strcat(pippo,"*#4*#");
    strcat(pippo, where.ascii());
    strcat(pippo,"##");
    emit init_requested(QString(pippo));
  }
  if(!strcmp(dove, where.ascii()))
  {
    qDebug("FRAME IS OURS !!");
    return true;
  }
  else
    return false;
}

// Autom frame interpreter

// Public methods
frame_interpreter_autom::
frame_interpreter_autom(QString w, bool p, int g) :
    frame_interpreter(QString("2"), w, p, g)
{
}

void frame_interpreter_autom::
get_init_message(device_status *s, QString& out)
{
    QString head = "*#2*";
    QString end = "##";
    out = head + where + end;
}

// Private methods
void frame_interpreter_autom::
set_status(device_status_autom *ds, int t)
{
    qDebug("frame_interpreter_autom::set_status (device_status_autom, %d)", t);
    stat_var curr_stat(stat_var::STAT);
    bool do_event = false;
    // Read current status
    ds->read((int)device_status_autom::STAT_INDEX, curr_stat);
    if(!curr_stat.initialized()) {
	qDebug("Initializing autom stat");
	curr_stat.set_val(t);
	ds->write_val((int)device_status_autom::STAT_INDEX, curr_stat);
	do_event = true;
	goto end;
    }
    qDebug("curr stat is %d\n", curr_stat.get_val());
    if(t != curr_stat.get_val()) {
	qDebug("setting status to %d", t);
	curr_stat.set_val(t);
	ds->write_val((int)device_status_autom::STAT_INDEX, curr_stat);
	do_event = 1;
    }
 end:
    if(do_event)
	evt_list.append(ds);
}

void frame_interpreter_autom::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_autom::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    evt_list.clear();
    device_status *ds = dsi->current();
    if(request_status) {
      if(strcmp(msg_open.Extract_dove(), "0") == 0)
		{
			request_init(ds, ds->init_request_delay());
		}
		else
			request_init(ds);
	goto end;
    }
    {
	int cosa = atoi(msg_open.Extract_cosa());
	set_status((device_status_autom *)ds, cosa);
	if(!evt_list.isEmpty())
	    emit(frame_event(evt_list));
    }
 end:
    delete dsi;
}



// Sound device frame interpreter

// Public methods
frame_interpreter_sound_device::
frame_interpreter_sound_device(QString w, bool p, int g) :
    frame_interpreter(QString("16"), w, p, g)
{
}

void frame_interpreter_sound_device::
get_init_messages(device_status *s, QStringList& out)
{
    QString head = "*#16*";
    QString end = "##";
    QString what ;

    out.clear();
    what = "*1";
    out.append(head + where + what + end);
    what = "*5";
    out.append(head + where + what + end);
}

// Private methods
void frame_interpreter_sound_device::
set_status(device_status_amplifier *ds, int l, int _on)
{
    qDebug("frame_interpreter_sound_device::set_status"
	   "(device_status_amplifier, %d %d)", l, _on);
    stat_var curr_lev(stat_var::AUDIO_LEVEL);
    stat_var curr_status(stat_var::ON_OFF);
    bool do_event = false;
    if(_on != -1) {
	int on = _on ? 1 : 0;
	// Read current level and status
	ds->read((int)device_status_amplifier::ON_OFF_INDEX, curr_status);
	if(!curr_status.initialized()) {
	    qDebug("Initializing ampli status");
	    curr_status.set_val(on);
	    ds->write_val((int)device_status_amplifier::ON_OFF_INDEX, 
			  curr_status);
	    do_event = true;
	} else {
	    if(curr_status.get_val() != on) {
		qDebug("Ampli status changed to %d", on);
		curr_status.set_val(on);
		ds->write_val((int)device_status_amplifier::ON_OFF_INDEX, 
			      curr_status);
		do_event = true;
	    }
	}
    }
    if(l >= 0) {
	ds->read((int)device_status_amplifier::AUDIO_LEVEL_INDEX, curr_lev);
	if(!curr_lev.initialized()) {
	    qDebug("Initializing audio level");
	    curr_lev.set_val(l);
	    ds->write_val((int)device_status_amplifier::AUDIO_LEVEL_INDEX,
			  curr_lev);
	    do_event = true;
	    goto end;
	}
	qDebug("curr audio level is %d\n", curr_lev.get_val());
	if(l != curr_lev.get_val()) {
	    qDebug("setting audio level to %d", l);
	    curr_lev.set_val(l);
	    ds->write_val((int)device_status_amplifier::AUDIO_LEVEL_INDEX, 
			  curr_lev);
	    do_event = 1;
	}
    }
 end:
    if(do_event)
	evt_list.append(ds);
}

void frame_interpreter_sound_device::handle_frame(openwebnet_ext m, 
						  device_status_amplifier *ds)
{
    qDebug("frame_interpreter_sound_device::handle_frame");
    int lev;
    if(m.IsMeasureFrame()) {
	// *#16*where*1*vol##
	int code = atoi(m.Extract_grandezza());
	switch(code) {
	case 1:
	    lev = atoi(m.Extract_valori(0));
	    // Only set level, don't touch status
	    set_status(ds, lev, -1);
	    break;
	default:
	    qDebug("Unknown sound device meas frame, ignoring");
	    break;
	}
    } else {
	int cosa = atoi(m.Extract_cosa());
	switch(cosa) {
	case 13:
	    set_status(ds, -1, 0);
	    break;
	case 0:
	case 3:
	    set_status(ds, -1, 1);
	    break;
	default:
	    qDebug("Unknown sound device norm frame, ignoring");
	    break;
	}
    }
}

void frame_interpreter_sound_device::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status = false;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_sound_device::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    request_init(ds);
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::AMPLIFIER:
		handle_frame(msg_open, (device_status_amplifier *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	++(*dsi);
    }    
    if(!evt_list.isEmpty())
	emit(frame_event(evt_list));
    delete dsi;
}

// Sound matrix frame interpreter
frame_interpreter_sound_matr_device::
frame_interpreter_sound_matr_device(QString w, bool p, int g) :
    frame_interpreter(QString("16"), QString("1000"), p, g)
{
}

void frame_interpreter_sound_matr_device::
get_init_message(device_status *ds, QString& out)
{
  out = QString("*#16*1000*11##");
}

void frame_interpreter_sound_matr_device::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status = false;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_sound_matr_device::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    request_init(ds);
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::SOUNDMATR:
		handle_frame(msg_open, (device_status_sound_matr *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	++(*dsi);
    }    
    if(!evt_list.isEmpty())
	emit(frame_event(evt_list));
    delete dsi;
}

void frame_interpreter_sound_matr_device::
handle_frame(openwebnet_ext m, device_status_sound_matr *ds)
{
    int act;
    bool do_event = false;
    qDebug("frame_interpreter_sound_matr_device::handle_frame");
    if(m.IsMeasureFrame()) {
	// *#1*where*1*lev*speed##
	int code = atoi(m.Extract_grandezza());
	qDebug("frame_sound_matr_device::handle_frame, meas frame");
	switch(code) {
	case 11:
	    for(int i=device_status_sound_matr::AMB1_INDEX; 
		i<=device_status_sound_matr::AMB8_INDEX; i++) {
		stat_var curr_act(stat_var::ACTIVE_SOURCE);
		ds->read(i, curr_act);
		qDebug("Curr active source for amb %d = %d", i, 
		       curr_act.get_val());
		act = atoi(m.Extract_valori(i));
		qDebug("New active source = %d", act);
		if(act != curr_act.get_val()) {
		    curr_act.set_val(act);
		    ds->write_val(i, curr_act);
		    do_event = true;
		}
	    }
	    break;
	default:
	    qDebug("frame_sound_matr_device::handle_frame"
		   ", meas frame, code = %d", code);
	    break;
	}
    }
    if(do_event)
	evt_list.append(ds);
}


// Radio device frame interpreter

// Public methods
frame_interpreter_radio_device::
frame_interpreter_radio_device(QString w, bool p, int g) :
    frame_interpreter(QString("16"), w, p, g)
{
}

void frame_interpreter_radio_device::
get_init_message(device_status *s, QString& out)
{
    QString head = "*#16*";
    QString end = "##";
    // Fixme: e` giusto questo ?
    QString what = "*6";
    out = head + where + what + end;
}

bool frame_interpreter_radio_device::is_frame_ours(openwebnet_ext m, 
						   bool& request_status)
{
    qDebug("frame_interpreter_radio_device::is_frame_ours");
    qDebug("who = %s, where = %s", who.ascii(), where.ascii());
    qDebug("msg who = %s, msg where = %s", m.Extract_chi(), 
	   m.get_where().ascii());
    if(strcmp(who.ascii(), m.Extract_chi()))
      return false;
    QString w = m.get_where();
    // Consider only "least significant part" of where, here
    int wh = w.toInt() ;
    while(wh >= 100)
	wh -= 100;
    while(wh >= 10)
	wh -= 10;
    bool out = m.is_target(this, get_who(), QString::number(wh, 10), 
			   request_status);
    if(out) 
	qDebug("FRAME IS OURS !!");
    return out;
}

// Private methods
void frame_interpreter_radio_device::handle_frame(openwebnet_ext m, 
						  device_status_radio *ds)
{
    qDebug("frame_interpreter_radio_device::handle_frame");
    int freq, staz;
    bool do_event = false;
    stat_var curr_freq(stat_var::FREQ);
    stat_var curr_staz(stat_var::STAZ);
    stat_var curr_rds0(stat_var::RDS0);
    stat_var curr_rds1(stat_var::RDS1);
    if(m.IsMeasureFrame()) {
	int code = atoi(m.Extract_grandezza());
	switch(code) {
	case 6:
	    qDebug("Frequenza nel messaggio = %s", m.Extract_valori(1));
	    freq = strtol(m.Extract_valori(1), NULL, 10);
	    ds->read((int)device_status_radio::FREQ_INDEX, curr_freq);
	    curr_freq.set_val(freq);
	    qDebug("Setting frequency to %d", freq);
	    ds->write_val((int)device_status_radio::FREQ_INDEX, curr_freq);
	    do_event = true;
	    break;
	case 7:
	    staz = atoi(m.Extract_valori(1));
	    ds->read((int)device_status_radio::STAZ_INDEX, curr_staz);
	    curr_staz.set_val(staz);
	    qDebug("Setting staz to %d", staz);
	    ds->write_val((int)device_status_radio::STAZ_INDEX, curr_staz);
	    do_event = true;
	    break;
	case 8:
	    union {
		char rds[8];
		int tmp[2];
	    } tmpu;
	    for(int idx=0;idx<8;idx++) {
		tmpu.rds[idx] = strtol(m.Extract_valori(idx), NULL, 10);
		qDebug("RDS: char[%d] = %c", idx, tmpu.rds[idx]);
	    }
	    ds->read((int)device_status_radio::RDS0_INDEX, curr_rds0);
	    qDebug("setting rds0 to 0x%08x", tmpu.tmp[0]);
	    curr_rds0.set_val(tmpu.tmp[0]);
	    ds->write_val((int)device_status_radio::RDS0_INDEX, curr_rds0);
	    ds->read((int)device_status_radio::RDS1_INDEX, curr_rds1);
	    qDebug("setting rds1 to 0x%08x", tmpu.tmp[1]);
	    curr_rds1.set_val(tmpu.tmp[1]);
	    ds->write_val((int)device_status_radio::RDS1_INDEX, curr_rds1);
	    do_event = true;
	    break;
	default:
	    qDebug("Unknown radio device meas frame, ignoring");
	    break;
	}
    }
    if(do_event)
	evt_list.append(ds);
}

void frame_interpreter_radio_device::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status = false;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_radio_device::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    request_init(ds);
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::RADIO:
		handle_frame(msg_open, (device_status_radio *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	++(*dsi);
    }    
    if(!evt_list.isEmpty())
	emit(frame_event(evt_list));
    delete dsi;
}



// Doorphone device frame interpreter

// Public methods
frame_interpreter_doorphone_device::
frame_interpreter_doorphone_device(QString w, bool p, int g) :
    frame_interpreter(QString("6"), w, p, g)
{
    qDebug("frame_interpreter_doorphone_device::"
	   "frame_interpreter_doorphone_device()");
}

void frame_interpreter_doorphone_device::
get_init_message(device_status *s, QString& out)
{
    out = QString("");
}

bool frame_interpreter_doorphone_device::is_frame_ours(openwebnet_ext m, 
						       bool& request_status)
{
    request_status = false;
    qDebug("frame_interpreter_doorphone_device::is_frame_ours");
    qDebug("who = %s, where = %s", who.ascii(), where.ascii());
    char *c = m.Extract_chi();
    qDebug("msg who = %s, msg where = %s", c, m.Extract_dove());
    QString cosa = m.Extract_cosa();
    qDebug("cosa = %s", cosa.ascii());
    if(!cosa.startsWith("9")) return false;
    // I posti esterni hanno chi = 8
    if(strcmp(c, "8")) return false;
    if(where == "ANY") return true;
    int d ;
    QString w = &m.Extract_dove()[1];
    qDebug("w = %s, w.toInt = %d", w.ascii(), w.toInt());
    int p;
    if((p = w.find('#')) >= 0) {
	// FIXME: CHECK THIS
	d = 4000 + w.left(p).toInt();
	char tmp[20];
	sprintf(tmp, "%d%s", d, w.right(w.length() - p).ascii());
	qDebug("BAH: %s", tmp);
	return !where.compare(tmp) ;
    } else {
	d = w.toInt() + 4000;
	qDebug("d = %d, where.toInt() = %d", d, where.toInt());
	return (d == where.toInt());
    }
    // NEVER REACHED
}

// Private methods
void frame_interpreter_doorphone_device::
set_status(device_status_doorphone *ds, int s)
{
    qDebug("frame_interpreter_doorphone_device::set_status"
	   "(device_status_doorphone, %d)", s);
    // When we get here, we surely got a call. So trigger a frame event
    evt_list.append(ds);
}

void frame_interpreter_doorphone_device::
handle_frame(openwebnet_ext m, device_status_doorphone *ds)
{
    qDebug("frame_interpreter_doorphone_device::handle_frame");
    // FIXME: REALLY HANDLE FRAME
    evt_list.append(ds);
}

void frame_interpreter_doorphone_device::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status = false;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_doorphone_device::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
#if 1
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status)) {
	// Discard frame if not ours
	qDebug("discarding frame");
	return;
    }
#else
    // PER DEBUG !!!!!!
    //frame = "*6*9*4000#2##"
    //frame = "*8*9#1#1*20#2##";
    frame = "*8*9#1#1*21#2##" ;
    //frame = "*8*1#1#4*11##";
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open))
	return;
#endif
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    request_init(ds);
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::DOORPHONE:
		handle_frame(msg_open, (device_status_doorphone *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	++(*dsi);
    }    
    if(!evt_list.isEmpty())
	emit(frame_event(evt_list));
    delete dsi;
}


// Anti-intrusion system device frame interpreter

// Public methods
frame_interpreter_impanti_device::
frame_interpreter_impanti_device(QString w, bool p, int g) :
    frame_interpreter(QString("5"), w, p, g)
{
    qDebug("frame_interpreter_impanti_device::"
	   "frame_interpreter_impanti_device()");
}

void frame_interpreter_impanti_device::
get_init_message(device_status *s, QString& out)
{
    out = QString("*#5*0##");
}

bool frame_interpreter_impanti_device::is_frame_ours(openwebnet_ext m, 
						     bool& request_status)
{
    // FIXME: IS THIS OK ?
    request_status = false;
    qDebug("frame_interpreter_impanti_device::is_frame_ours");
    qDebug("who = %s, where = %s", who.ascii(), where.ascii());
    char *c = m.Extract_chi();
    qDebug("msg who = %s, msg where = %s", c, m.Extract_dove());
    int l, i;
    if(QString(c) != who)
      return false;
    return m.gen(l,i) || QString(m.Extract_dove()) == "";
}

// Private methods
void frame_interpreter_impanti_device::
handle_frame(openwebnet_ext m, device_status_impanti *ds)
{
    qDebug("frame_interpreter_impanti_device::handle_frame");
    stat_var curr_stat(stat_var::ON_OFF);
    int stat;
    // Read current status of ON OFF variable
    ds->read((int)device_status_impanti::ON_OFF_INDEX, curr_stat);
    qDebug("curr status is %d\n", curr_stat.get_val());
    int cosa = atoi(m.Extract_cosa());
    switch(cosa) {
    case 8:
	stat = 1;
	break;
    case 9:
	stat = 0;
	break;
    default:
	qDebug("Imp anti frame with unknown what");
	stat = -1;
	break;
    }
    if(stat < 0) return;
    if(ds->initialized() && (stat == curr_stat.get_val()))
	return;
    curr_stat.set_val(stat);
    ds->write_val((int)device_status_impanti::ON_OFF_INDEX, curr_stat);
    evt_list.append(ds);
}

void frame_interpreter_impanti_device::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_impanti_device::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    request_init(ds);
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::IMPANTI:
		handle_frame(msg_open, (device_status_impanti *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	++(*dsi);
    }    
    if(!evt_list.isEmpty())
	emit(frame_event(evt_list));
    delete dsi;
}


// Anti-intrusion system device frame interpreter (single zone)

// Public methods
frame_interpreter_zonanti_device::
frame_interpreter_zonanti_device(QString w, bool p, int g) :
    frame_interpreter(QString("5"), w, p, g)
{
    qDebug("frame_interpreter_zonanti_device::"
	   "frame_interpreter_zonanti_device()");
}

void frame_interpreter_zonanti_device::
get_init_message(device_status *s, QString& out)
{
    QString head = "*#5*";
    QString end = "##";
    out = head + where + end;
}

bool frame_interpreter_zonanti_device::is_frame_ours(openwebnet_ext m,
						     bool& request_status)
{
    // FIXME: IS THIS OK ?
    request_status = false;
    qDebug("frame_interpreter_impanti_device::is_frame_ours");
    qDebug("who = %s, where = %s", who.ascii(), where.ascii());
    char *c = m.Extract_chi();
    char *d = m.Extract_dove();
    qDebug("msg who = %s, msg where = %s", c, d);
    return ((QString(c) == who) && (QString(d) == where));
}

// Private methods
void frame_interpreter_zonanti_device::
handle_frame(openwebnet_ext m, device_status_zonanti *ds)
{
    qDebug("frame_interpreter_zonanti_device::handle_frame");
    stat_var curr_stat(stat_var::ON_OFF);
    int stat;
    // Read current status of ON OFF variable
    ds->read((int)device_status_zonanti::ON_OFF_INDEX, curr_stat);
    qDebug("curr status is %d\n", curr_stat.get_val());
    int cosa = atoi(m.Extract_cosa());
    switch(cosa) {
    case 11:
	stat = 1;
	break;
    case 18:
	stat = 0;
	break;
    default:
	qDebug("Zona anti frame with unknown what");
	stat = -1;
	break;
    }
    if(stat < 0) return;
    if(ds->initialized() && (stat == curr_stat.get_val()))
	return;
    curr_stat.set_val(stat);
    ds->write_val((int)device_status_zonanti::ON_OFF_INDEX, curr_stat);
    qDebug("status changed, appending evt");
    evt_list.append(ds);
}

void frame_interpreter_zonanti_device::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status = false;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_zonanti_device::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    request_init(ds);
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::ZONANTI:
		handle_frame(msg_open, (device_status_zonanti *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	++(*dsi);
    }    
    if(!evt_list.isEmpty())
	emit(frame_event(evt_list));
    delete dsi;
}


// Thermal regulator device frame interpreter

// Public methods
frame_interpreter_thermr_device::
frame_interpreter_thermr_device(QString w, bool p, int g) :
    frame_interpreter(QString("4"), w, p, g)
{
    qDebug("frame_interpreter_thermr_device::"
	   "frame_interpreter_thermr_device()");
}

bool frame_interpreter_thermr_device::is_frame_ours(openwebnet_ext m, 
						    bool& request_status)
{
    // FIXME: IS THIS OK ?
    request_status = false;
    if (strcmp(m.Extract_chi(),"4")) return false;
    char dove[30];
    strcpy(dove, m.Extract_dove());
    if (dove[0]=='#')
            strcpy(&dove[0], &dove[1]);
    if(!strcmp(dove, "0")) {
	char pippo[50];
	// Richiesta via centrale
	memset(pippo,'\000',sizeof(pippo));
	strcat(pippo,"*#4*#");
	strcat(pippo, where.ascii());
	strcat(pippo,"##");
	emit init_requested(QString(pippo));
    }
    return !strcmp(dove, where.ascii());
}

void frame_interpreter_thermr_device::
get_init_message(device_status *s, QString& out)
{
    QString head, end;
    switch(s->get_type()) {
    case device_status::TEMPERATURE_PROBE:
    {
      head = "*#4*";
      end = "##";
      out = head + where + end;
      break;
    }
    case device_status::THERMR:
    {
      head = "*#4*#";
      end = "##";
      out = head + where + end;
      break;
    }
    default:
      out = "";
      break;
    }
}

// Private methods
void frame_interpreter_thermr_device::
handle_frame(openwebnet_ext m, device_status_thermr *ds)
{
    qDebug("frame_interpreter_thermr_device::handle_frame");
    stat_var curr_stat(stat_var::STAT);
    stat_var curr_local(stat_var::LOCAL);
    stat_var curr_sp(stat_var::SP);
    int stat;
    bool do_event = false;
    char pippo[50];
    // Read current status
    ds->read((int)device_status_thermr::STAT_INDEX, curr_stat);
    ds->read((int)device_status_thermr::LOCAL_INDEX, curr_local);
    ds->read((int)device_status_thermr::SP_INDEX, curr_sp);
    qDebug("curr status is %d", curr_stat.get_val());
    qDebug("curr local is %d", curr_local.get_val());
    qDebug("curr sp is %d", curr_sp.get_val());
    int cosa = atoi(m.Extract_cosa());
    qDebug("cosa = %d", cosa);
    /*if((curr_stat.get_val() != device_status_thermr::S_MAN) &&
       (curr_stat.get_val() != device_status_thermr::S_AUTO) && 
       m.Extract_dove()[0] == '#') {
      qDebug("Ignoring frame (status is %d)", curr_stat.get_val());
      return;
    }*/
    switch(cosa) {
    case 110:
    case 210:
    case 310:
	// MANUALE
	if(!ds->initialized() || (curr_stat.get_val() != 
				  device_status_thermr::S_MAN)) {
	    do_event = true;
	    stat = device_status_thermr::S_MAN;
	}
        //Richiesta set-point
        memset(pippo,'\000',sizeof(pippo));
        strcat(pippo,"*#4*");
        strcat(pippo,m.Extract_dove()+1);
        strcat(pippo,"*14##");
        emit init_requested(QString(pippo));
	elaborato = true;
	break;
    case 111:
    case 211:
    case 311:
	// AUTOMATICO
	if(!ds->initialized() || (curr_stat.get_val() != 
				  device_status_thermr::S_AUTO)) {
	    do_event = true;
	    stat = device_status_thermr::S_AUTO;
            memset(pippo,'\000',sizeof(pippo));
            strcat(pippo,"*#4*");
            strcat(pippo,m.Extract_dove()+1);
            strcat(pippo,"##");
            emit init_requested(QString(pippo));
	}
	elaborato = true;
	break;
    case 102:
	// PROT. ANTIGELO
	if(!ds->initialized() || (curr_stat.get_val() != 
				  device_status_thermr::S_ANTIGELO)) {
	    do_event = true;
	    stat = device_status_thermr::S_ANTIGELO;
	}
	elaborato = true;
	break;
    case 202:
	// PROT TERMICA
	if(!ds->initialized() || (curr_stat.get_val() != 
				  device_status_thermr::S_TERM)) {
	    do_event = true;
	    stat = device_status_thermr::S_TERM;
	}
	elaborato = true;
	break;
    case 302:
	// PROT GENERICA
	if(!ds->initialized() || (curr_stat.get_val() != 
				  device_status_thermr::S_GEN)) {
	    do_event = true;
	    stat = device_status_thermr::S_GEN;
	}
	elaborato = true;
	break;
    case 103:
    case 203:
    case 303:
	// OFF
	if(!ds->initialized() || (curr_stat.get_val() != 
				  device_status_thermr::S_OFF)) {
	    do_event = true;
	    stat = device_status_thermr::S_OFF;
	}
	elaborato = true;
	break;
#if 0
    case 0:
    case 1:
      {
	if(!m.IsNormalFrame())
	  break;
	stat = device_status_thermr::S_MAN;
	do_event = true;
	elaborato = true;
        QString head = "*#4*#";
        QString end = "##";
	QString m = head + where + end;
	emit(init_requested(m));
	break;
      }
#endif
    default:
	// Do nothing
	break;
    }
    if(do_event) {
        qDebug("setting new stat is %d", stat);
	curr_stat.set_val(stat);
	ds->write_val((int)device_status_thermr::STAT_INDEX, curr_stat);
	evt_list.append(ds);
    }
    if(elaborato)
	return;
    int g = atoi(m.Extract_grandezza());
    qDebug("g = %d", g);
    int loc, sp;
    switch(g) {
    case 13:
	loc = atoi(m.Extract_valori(0));
	if(!ds->initialized() || (curr_local.get_val() != loc)) {
	    curr_local.set_val(loc);
	    ds->write_val((int)device_status_thermr::LOCAL_INDEX, curr_local);
	    evt_list.append(ds);
            if(!ds->initialized() || 
             ((curr_stat.get_val() != device_status_thermr::S_AUTO) && 
              (curr_stat.get_val() != device_status_thermr::S_MAN) &&
             (loc == 13))) {
                do_event = true;
                memset(pippo,'\000',sizeof(pippo));
                stat = device_status_thermr::S_MAN;
                strcat(pippo,"*#4*#");
                strcat(pippo,m.Extract_dove());
                strcat(pippo,"##");
                emit init_requested(QString(pippo));
            }
	}
	elaborato = true;
	break;
//    case 12:
    case 14:
	sp = atoi(m.Extract_valori(0));
	qDebug("sp = %d", sp);
	if(!ds->initialized() || (curr_sp.get_val() != sp)) {
	    qDebug("setting new sp");
	    curr_sp.set_val(sp);
	    ds->write_val((int)device_status_thermr::SP_INDEX, curr_sp);
	    evt_list.append(ds);
	}
	elaborato = true;
	break;
    default:
	// Do nothing
	break;
    }
}

void frame_interpreter_thermr_device::
handle_frame(openwebnet_ext m, device_status_temperature_probe *ds)
{
    qDebug("frame_interpreter_thermr_device::handle_frame, temp status");
    stat_var curr_temp(stat_var::TEMPERATURE);
    int stat;
    bool do_event = false;
    if(m.IsNormalFrame()) {
	qDebug("Normal frame, discarding");
	return;
    }
    // Read current status
    ds->read((int)device_status_temperature_probe::TEMPERATURE_INDEX, 
	     curr_temp);
    int g = atoi(m.Extract_grandezza());
    qDebug("g = %d", g);
    if(g) 
	return;
    int t = atoi(m.Extract_valori(0));
    qDebug("t = %d", t);
    if(ds->initialized() && t == curr_temp.get_val()) 
	return;
    qDebug("setting temp to %d", t);
    curr_temp.set_val(t);
    ds->write_val((int)device_status_temperature_probe::TEMPERATURE_INDEX, 
		  curr_temp);
    evt_list.append(ds);
}

void frame_interpreter_thermr_device::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_thermr_device::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    elaborato = false;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    request_init(ds);
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::THERMR:
		handle_frame(msg_open, (device_status_thermr *)ds);
		break;
	    case device_status::TEMPERATURE_PROBE:
		handle_frame(msg_open, (device_status_temperature_probe *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	if(elaborato)
	    break;
	++(*dsi);
    }    
    if(!evt_list.isEmpty()) {
	qDebug("emit(evt_list)");
	emit(frame_event(evt_list));
    }
    delete dsi;
}



// Modscen device frame interpreter

// Public methods
frame_interpreter_modscen_device::
frame_interpreter_modscen_device(QString w, bool p, int g) :
    frame_interpreter(QString("0"), w, p, g)
{
    qDebug("frame_interpreter_modscen_device::"
	   "frame_interpreter_modscen_device()");
}

#if 0
// FIXME: CHECK WHETHER THIS IS NEEDED OR NOT
bool frame_interpreter_modscen_device::is_frame_ours(openwebnet_ext m,
						     bool& request_status)
{
    request_status = false;
    if (strcmp(m.Extract_chi(),"0")) return false;

}
#endif

void frame_interpreter_modscen_device::
get_init_message(device_status *s, QString& out)
{
    QString head = "*#0*";
    QString end = "##";
    out = head + where + end;
}

// Private methods
void frame_interpreter_modscen_device::
handle_frame(openwebnet_ext m, device_status_modscen *ds)
{
    qDebug("frame_interpreter_modscen_device::handle_frame");
    stat_var curr_stat(stat_var::STAT);
    int stat;
    bool do_event = false;
    // Read current status
    ds->read((int)device_status_modscen::STAT_INDEX, curr_stat);
    qDebug("curr status is %d\n", curr_stat.get_val());
    int cosa = atoi(m.Extract_cosa());
    if(ds->initialized() && (curr_stat.get_val() == cosa)) 
	return;
    if((cosa != 41) && (cosa != 40) && (cosa != 44) && (cosa != 43))
	return;
    curr_stat.set_val(cosa);
    qDebug("new status is %d\n", curr_stat.get_val());
    ds->write_val((int)device_status_modscen::STAT_INDEX, curr_stat);
    evt_list.append(ds);
}

void frame_interpreter_modscen_device::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    bool request_status = false;
    openwebnet_ext msg_open;
    rearmWDT();
    qDebug("frame_interpreter_modscen_device::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open, request_status))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    evt_list.clear();
    while( ( ds = dsi->current() ) != 0) {
	if(request_status) {
	    request_init(ds);
	    goto next;
	}
	{
	    device_status::type type = ds->get_type();
	    switch (type) {
	    case device_status::MODSCEN:
		handle_frame(msg_open, (device_status_modscen *)ds);
		break;
	    default:
		// Do nothing
		break;
	    }
	}
    next:
	++(*dsi);
    }    
    if(!evt_list.isEmpty())
	emit(frame_event(evt_list));
    delete dsi;
}
