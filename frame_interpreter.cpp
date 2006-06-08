//! Implementation of frame interpreter classes

#include <qstring.h>
#include <qptrlist.h>
#include <qobject.h>
#include <stdlib.h>

#include "openclient.h"
#include "device.h"
#include "frame_interpreter.h"

// Generic frame interpreter

frame_interpreter::frame_interpreter(QString _who, QString _where, 
				     bool p, int g)
{
    who = _who;
    where = _where;
    pul = p;
    group = g;
    qDebug("frame_interpreter::frame_interpreter()");
    qDebug("who = %s, where = %s", who.ascii(), where.ascii());
}

bool frame_interpreter::belong_to_group(int g)
{
    // Group is not supported at the moment
    if(group < 0)
	return false;
    return (group & (1 << g));
}

bool frame_interpreter::is_frame_ours(openwebnet m)
{
    qDebug("frame_interpreter::is_frame_ours");
    qDebug("who = %s, where = %s", who.ascii(), where.ascii());
    qDebug("msg who = %s, msg where = %s", m.Extract_chi(), m.Extract_dove());
    if(strcmp(m.Extract_chi(), who.ascii())) return false ;
    if(!strcmp(m.Extract_dove(), where.ascii())) return true ;
    // BAH
    return (!pul && ((!strcmp(m.Extract_dove(), "0")) ||
		     ((strlen(m.Extract_dove())==1) && 
		      (!strncmp(m.Extract_dove(), where.ascii(), 1)) ) || 
		     ((!strncmp(m.Extract_dove(), "#", 1)) && 
		      belong_to_group(atoi(m.Extract_dove() + 1)))));
}

// This is reimplemented by children
void frame_interpreter::get_init_message(device_status *s, QString& out)
{
    out = "";
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
    QString end = "##";
    switch(s->get_type()) {
    case device_status::LIGHTS:
    case device_status::DIMMER:
    case device_status::DIMMER100:
    case device_status::NEWTIMED:
	out = head + where + end;
	break;
    default:
	out = "";
    }
}

void frame_interpreter_lights::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    openwebnet msg_open;
    qDebug("frame_interpreter_lights::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open))
	// Discard frame if not ours
	return;
    // Walk through list of device_status' and pass frame to relevant 
    // handler method
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    while( ( ds = dsi->current() ) != 0) {
	device_status::type type = ds->get_type();
	switch (type) {
	case device_status::LIGHTS:
	    handle_frame(msg_open, (device_status_light *)ds);
	    break;
	case device_status::DIMMER:
	    handle_frame(msg_open, (device_status_dimmer *)ds);
	    break;
	case device_status::DIMMER100:
	    handle_frame(msg_open, (device_status_dimmer100 *)ds);
	    break;
	case device_status::NEWTIMED:
	    handle_frame(msg_open, (device_status_new_timed *)ds);
	    break;
	default:
	    // Do nothing
	    break;
	}
	++(*dsi);
    }
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
    qDebug("curr status is %d\n", curr_stat.get_val());
    if(s && (!curr_stat.get_val())) {
	int v = 1;
	curr_stat.set_val(v);
	qDebug("setting light status to on");
	ds->write_val((int)device_status_light::ON_OFF_INDEX, 
		      curr_stat);
	do_event = 1;
    } else if(!s && curr_stat.get_val()) {
	int v = 0;
	curr_stat.set_val(v);
	qDebug("setting light status to off");
	ds->write_val((int)device_status_light::ON_OFF_INDEX, 
		      curr_stat);
	do_event = 1;
    }
    if(do_event || !curr_stat.initialized())
	emit(frame_event(ds));
}


void frame_interpreter_lights::handle_frame(openwebnet m, 
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
    stat_var curr_lev(stat_var::LEV), curr_old_lev(stat_var::OLD_LEV);
    // Read current status of variables
    ds->read((int)device_status_dimmer::LEV_INDEX, curr_lev);
    ds->read((int)device_status_dimmer::OLD_LEV_INDEX, curr_old_lev);
    if(!ds->initialized()) {
	curr_lev.set_val(lev);
	curr_old_lev.set_val(lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	ds->write_val((int)device_status_dimmer::OLD_LEV_INDEX, curr_old_lev);
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
	do_event = true ;
    }
    if(lev != curr_lev.get_val()) {
	int old_lev = curr_lev.get_val();
	curr_old_lev.set_val(old_lev);
	curr_lev.set_val(lev);
	qDebug("setting dimmer status to %d", lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	do_event = true ;
    }
 end:
    if(do_event)
	emit(frame_event(ds));
}

void frame_interpreter_lights::handle_frame(openwebnet m, 
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
	    set_status(ds, 1);
	    break;
	case 30:
	    // UP
	    ds->read((int)device_status_dimmer::LEV_INDEX, sv);
	    set_status(ds, sv.get_val() + sv.get_step());
	    break;
	case 31:
	    // DOWN
	    ds->read((int)device_status_dimmer::LEV_INDEX, sv);
	    set_status(ds, sv.get_val() - sv.get_step());
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
    stat_var curr_lev(stat_var::LEV), curr_old_lev(stat_var::OLD_LEV);
    stat_var curr_speed(stat_var::SPEED);
    // Read current status of status variables
    ds->read((int)device_status_dimmer100::LEV_INDEX, curr_lev);
    ds->read((int)device_status_dimmer100::OLD_LEV_INDEX, curr_old_lev);
    ds->read((int)device_status_dimmer100::SPEED_INDEX, curr_speed);
    if(!curr_lev.initialized()) {
	curr_lev.set_val(lev);
	curr_old_lev.set_val(lev);
	ds->write_val((int)device_status_dimmer100::LEV_INDEX, curr_lev);
	ds->write_val((int)device_status_dimmer100::OLD_LEV_INDEX, 
		      curr_old_lev);
	do_event = true;
    }
    if(!curr_speed.initialized() && speed >= 0) {
	curr_speed.set_val(speed);
	ds->write_val((int)device_status_dimmer100::SPEED_INDEX, curr_speed);
	do_event = true;
    }
    if((lev == 1) && !curr_lev.get_val()) {
	// ON, restore old val
	int old_lev = curr_old_lev.get_val();
	curr_lev.set_val(old_lev);
	ds->write_val((int)device_status_dimmer100::LEV_INDEX, curr_lev);
	do_event = true ;
    }
    if(lev != curr_lev.get_val()) {
	int old_lev = curr_lev.get_val();
	curr_old_lev.set_val(old_lev);
	curr_lev.set_val(lev);
	qDebug("setting dimmer status to %d", lev);
	ds->write_val((int)device_status_dimmer::LEV_INDEX, curr_lev);
	do_event = true ;
    }
    if((speed >= 0) && (speed != curr_speed.get_val())) {
	curr_speed.set_val(speed);
	qDebug("setting dimmer speed to %d", speed);
	ds->write_val((int)device_status_dimmer100::SPEED_INDEX, curr_speed);
	do_event = true ;
    }
    if(do_event)
    	emit(frame_event(ds));
}

void frame_interpreter_lights::handle_frame(openwebnet m, 
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
	    set_status(ds, 1);
	    break;
	case 30:
	    // UP
	    ds->read((int)device_status_dimmer::LEV_INDEX, lev);
	    set_status(ds, lev.get_val() + lev.get_step());
	    break;
	case 31:
	    // DOWN
	    ds->read((int)device_status_dimmer::LEV_INDEX, lev);
	    set_status(ds, lev.get_val() - lev.get_step());
	    break;
	default:
	    if((cosa >=2) && (cosa <= 10))
		// Dimmer level
#if 0
		set_status(ds, (cosa - 1) * 10);
#else
         	set_status(ds, cosa * 10);
#endif
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
    bool do_event = false;
    stat_var curr_hh(stat_var::HH), curr_mm(stat_var::MM), 
	curr_ss(stat_var::SS);
    // Read current status
    ds->read((int)device_status_new_timed::HH_INDEX, curr_hh);
    ds->read((int)device_status_new_timed::MM_INDEX, curr_mm);
    ds->read((int)device_status_new_timed::SS_INDEX, curr_ss);
    if(!curr_hh.initialized()) {
	qDebug("hh, mm, ss not yet initialized, initializing now");
	curr_hh.set_val(hh);
	curr_mm.set_val(mm);
	curr_ss.set_val(ss);
	do_event = true;
	goto end;
    }
    qDebug("current hh:mm:ss = %02d:%02d:%02d", curr_hh.get_val(),
	   curr_mm.get_val(), curr_ss.get_val());
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
 end:
    if(do_event) {
	qDebug("timed device event");
	ds->write_val((int)device_status_new_timed::HH_INDEX, curr_hh);
	ds->write_val((int)device_status_new_timed::MM_INDEX, curr_mm);
	ds->write_val((int)device_status_new_timed::SS_INDEX, curr_ss);	
    	emit(frame_event(ds));
    }
    qDebug("frame_interpreter_lights::set_status, end");
}

void frame_interpreter_lights::handle_frame(openwebnet m, 
					    device_status_new_timed *ds)
{
    qDebug("frame_interpreter_lights::handle_frame(), new timed device");
    stat_var hh(stat_var::HH), mm(stat_var::MM), ss(stat_var::SS);
    if(m.IsNormalFrame()) {
	int cosa = atoi(m.Extract_cosa());
	switch(cosa) {
	case 0:
	    // OFF , 
	    set_status(ds, 0, 0, 0, 0);
	    break;
	case 1:
	    // ON
	    set_status(ds, 1, 0, 0, 0);
	    break;
	case 30:
	    // UP
	    set_status(ds, 0, 0, 0, 1);
	    break;
	case 31:
	    // DOWN
	    set_status(ds, 0, 0, 0, 1);
	    break;
	default:
	    if((cosa >=2) && (cosa <= 10))
		// Dimmer level
		set_status(ds, 0, 0, 0, 1);
	    else if((cosa >= 11) && (cosa <= 19)) {
		// What shall we do here ?
		set_status(ds, 0, 0, 0, 1);
	    } else if((cosa >= 20) && (cosa <= 29)) {
		// What shall we do here ?
		set_status(ds, 0, 0, 0, 1);
	    }
	    break;
	} 
    } else if(m.IsMeasureFrame()) {
	// *#1*where*1*lev*speed##
	int code = atoi(m.Extract_grandezza());
	int lev, speed, hh, mm, ss;
	qDebug("frame_interpreter_lights::handle_frame, timed, meas frame");
	switch(code) {
	case 1:
	    set_status(ds, 0, 0, 0, 1);
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
	set_status(ds, 0, 0, 0, (lev > 0 ? 1 : 0));
    }
}

// Temperature probe frame interpreter

// Public methods
frame_interpreter_temperature_probe::
frame_interpreter_temperature_probe(QString w, bool p, int g) :
    frame_interpreter(QString("4"), w, p, g)
{
}

void frame_interpreter_temperature_probe::
get_init_message(device_status *s, QString& out)
{
    QString head = "*#4*";
    QString end = "##";
    out = head + where + end;
}

// Private methods
void frame_interpreter_temperature_probe::
set_status(device_status_temperature_probe *ds, int t)
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
	ds->write_val((int)device_status_temperature_probe::TEMPERATURE_INDEX, 
		      curr_temp);
	do_event = 1;
    }
 end:
    if(do_event)
	emit(frame_event(ds));
}

void frame_interpreter_temperature_probe::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    openwebnet msg_open;
    qDebug("frame_interpreter_temperature::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds = dsi->current();
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
}



// Sound device frame interpreter

// Public methods
frame_interpreter_sound_device::
frame_interpreter_sound_device(QString w, bool p, int g) :
    frame_interpreter(QString("16"), w, p, g)
{
}

void frame_interpreter_sound_device::
get_init_message(device_status *s, QString& out)
{
    QString head = "*#16*";
    QString end = "##";
    QString what ;
    switch(s->get_type()) {
    case device_status::AMPLIFIER:
	what = QString("*1");
	break;
    default:
	qDebug("init message required for unknown device status");
	out = "";
	return;
    }
    out = head + where + what + end;
}

// Private methods
void frame_interpreter_sound_device::
set_status(device_status_amplifier *ds, int l)
{
    qDebug("frame_interpreter_sound_device::set_status"
	   "(device_status_amplifier, %d)", l);
    stat_var curr_lev(stat_var::AUDIO_LEVEL);
    bool do_event = false;
    // Read current temperature
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
	ds->write_val((int)device_status_amplifier::AUDIO_LEVEL_INDEX, 
		      curr_lev);
	do_event = 1;
    }
 end:
    if(do_event)
	emit(frame_event(ds));
}

void frame_interpreter_sound_device::handle_frame(openwebnet m, 
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
	    set_status(ds, lev);
	    break;
	default:
	    qDebug("Unknown sound device meas frame, ignoring");
	    break;
	}
    } else
	qDebug("Unknown sound device frame, ignoring");
}

void frame_interpreter_sound_device::
handle_frame_handler(char *frame, QPtrList<device_status> *sl)
{
    openwebnet msg_open;
    qDebug("frame_interpreter_sound_device::handle_frame_handler");
    qDebug("#### frame is %s ####", frame);
    msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
    if(!is_frame_ours(msg_open))
	// Discard frame if not ours
	return;
    QPtrListIterator<device_status> *dsi = 
	new QPtrListIterator<device_status>(*sl);
    dsi->toFirst();
    device_status *ds;
    while( ( ds = dsi->current() ) != 0) {
	device_status::type type = ds->get_type();
	switch (type) {
	case device_status::AMPLIFIER:
	    handle_frame(msg_open, (device_status_amplifier *)ds);
	    break;
	default:
	    // Do nothing
	    break;
	}
	++(*dsi);
    }    
}
