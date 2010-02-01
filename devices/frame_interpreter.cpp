#include "frame_interpreter.h"
#include "hardware_functions.h" // rearmWDT
#include "device.h"

#include <openmsg.h>

#include <QStringList>
#include <QDebug>

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
	int i = indexOf('#', 1);
	l = i>=0 ? at(i + 1).digitValue() : 3;
	qDebug("*** level is %d", l);
}

void openwebnet_where::interf(int& i)
{
	//  Look for # from position 1
	int j = indexOf('#', 1);
	if (j < 0)
	{
		i = -1;
		return;
	}
	int k = indexOf('#', j+1);
	i = k>=0 ? right(length() - k - 1).toInt() : -1;
	qDebug("*** interface is %d", i);
}

bool openwebnet_where::pp(int& addr)
{
	int trash;
	if (gen(trash, trash) || amb(trash, trash, trash) || gro(trash))
		return false;
	// Check this
	addr = toULong();
	return true;
}

bool openwebnet_where::gen(int& l, int& i)
{
	l = -1;
	if (!length())
		return false;
	if ((at(0) == '0' && length() == 1) || (at(0) == '0' && length() >= 3 && length() <= 6))
	{
		lev(l);
		interf(i);
		return true;
	}
	return false;
}

bool openwebnet_where::amb(int& a, int& l, int& i)
{
	qDebug() << "openwebnet_where:amb. Where is " << *this;
	if (*this == "00")
	{
		// Diagnostic messages
		a = 0; 
		lev(l);
		interf(i);
		qDebug("amb00 !! (%d, %d)", l, i);
		return true;
	}

	if (at(0) >= '1' && at(0) <= '9' && length() == 1)
	{
		a = QChar(at(0)).toAscii() - '0';
		l = 3;
		i = -1;
		qDebug("amb ! (%d, %d, %d)", a, l, i);
		return true;
	}

	if (at(0) >= '1' && at(0) <= '9' && at(1) == '#')
	{
		a = QChar(at(0)).toAscii() - '0';
		lev(l);
		interf(i);
		qDebug("amb !! (%d, %d, %d)", a, l, i);
		return true;
	}

	return false;
}

bool openwebnet_where::gro(int& g)
{
	if (at(0) == '#' && at(1) >= '1' && at(1) <= '9')
	{
		g = QChar(at(1)).toAscii() - 0x30;
		return true;
	}
	return false;
}

bool openwebnet_where::extended()
{
	return indexOf('#', 1) >= 0;
}

// Openwebnet extended class implementation
// Public methods

openwebnet_ext::openwebnet_ext() : openwebnet()
{
}

bool openwebnet_ext::is_target(frame_interpreter *fi, QString who, bool& request_status)
{
	request_status = false;
	if (fi->get_who() != who) return false;
	if (fi->get_pul()) return false;
	int l = 0, i = 0;
	if (gen(l, i))
	{
		qDebug("gen!!");
		if ((!extended() && l == 3) || (extended() && l == 3 && l == fi->get_lev()) ||
				((l == 4) && fi->get_lev() == 4 && i == fi->get_interface()))
		{
			request_status = true;
			return true;
		}
		else
			return false;
	}
	int addr;
	if (pp(addr))
	{
		qDebug("pp (%d)", addr);
		return fi->get_where() == get_where();
	}
	int a;
	if (amb(a, l, i))
	{
		qDebug("amb(%d)", a);
		if (((l == 3 && l == fi->get_lev() && a == fi->get_amb()) ||
					(l == 4 && a == fi->get_amb() && i == fi->get_interface())))
		{
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
	if (fi->get_who() != who) return false;
	if (fi->get_pul()) return false;
	int l, i;
	if (gen(l, i))
	{
		if ((l == 3 && l == fi->get_lev()) || ((l == 4) && fi->get_lev() == 4 &&
					fi->get_interface() == i))
			request_status = true;
		return true;
	}
	int addr;
	if (pp(addr))
		return fi->get_where() == wh;
	int a;
	if (amb(a, l, i))
	{
		if ((l == 3 && l == fi->get_lev() && a == fi->get_amb()) ||
				(l == 4 && a == fi->get_amb() && i == fi->get_interface()))
		{
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

QString openwebnet_ext::get_where()
{
	return estesa ? QString(Extract_dove()) + QString("#") + 
		QString(Extract_livello()) + QString("#") + 
		QString(Extract_interfaccia()) : QString(Extract_dove());
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

bool openwebnet_ext::extended()
{
	openwebnet_where w(get_where());
	return w.extended();
}

// Generic frame interpreter

frame_interpreter::frame_interpreter(QString _who, QString _where, bool p, int g)
{
	who = _who;
	where = _where;
	pul = p;
	group = g;
	connect(&deferred_timer, SIGNAL(timeout()), this, SLOT(deferred_request_init()));
	qDebug("frame_interpreter::frame_interpreter()");
	qDebug() << "who = " << who << ", where = " << where;
}

QString frame_interpreter::get_who()
{
	return who;
}

QString frame_interpreter::get_where()
{
	return where;
}

bool frame_interpreter::get_pul()
{
	return pul;
}

int frame_interpreter::get_amb()
{
	qDebug("frame_interpreter::get_amb");
	int a, l, i, trash;
	openwebnet_where w(where);
	if (where == "00")
	{
		w.amb(a, l, i);
		return a;
	}
	if (w.gro(trash))
		return -1;
	if (w.gen(l, i))
		return -1;
	return where.at(0).digitValue();
}

int frame_interpreter::get_lev()
{
	int l;
	openwebnet_where w(where);
	w.lev(l);
	return l;
}

int frame_interpreter::get_interface()
{
	int i;
	openwebnet_where w(where);
	w.interf(i);
	return i;
}

bool frame_interpreter::belongs_to_group(int g)
{
	// Group is not supported at the moment
	if (group < 0)
		return false;
	return (group & (1 << g));
}

bool frame_interpreter::is_frame_ours(openwebnet_ext m, bool& request_status)
{
	qDebug("frame_interpreter::is_frame_ours");
	qDebug() << "who = " << who << ", where = " << where;
	qDebug() << "msg who = " << m.Extract_chi() << ", msg where = " << m.get_where();
	bool out = m.is_target(this, request_status);
	if (out)
	{
		qDebug("FRAME IS OURS !!");
	}
	return out;
}

void frame_interpreter::request_init(device_status *ds, int delay)
{
	if (delay)
	{
		qDebug("delayed init request (%d) for %p", delay, ds);
		deferred_list_element de;
		de.ds = ds;
		de.expires = (QTime::currentTime()).addMSecs(delay);

		if (!deferred_timer.isActive())
		{
			deferred_list.append(de);
			deferred_timer.setSingleShot(true);
			deferred_timer_expires = de.expires;
			deferred_timer.start(delay);
			return;
		}
		// XXX: qt3 code had this condition separeted from the previous one.
		if (de.expires > deferred_timer_expires)
		{
			deferred_timer.setSingleShot(true);
			deferred_timer_expires = de.expires;
			deferred_timer.start(delay);
		}
		return;
	}

	// No deferred timeout
	QStringList msgl;
	msgl.clear();
	get_init_messages(ds, msgl);
	for (QStringList::Iterator it = msgl.begin(); it != msgl.end(); ++it)
		emit init_requested(*it);
}

void frame_interpreter::deferred_request_init()
{
	qDebug("frame_interpreter::deferred_request_init()");
	QMutableListIterator<deferred_list_element> it(deferred_list);

	int ms = -1;
	if (deferred_list.isEmpty())
		return;
	// Build an invalid time
	QTime next_expires = QTime(25, 61);
	if (!next_expires.isValid()) {
		qDebug("no more deferred status requests");
	}

	while (it.hasNext())
	{
		deferred_list_element de = it.next();
		if (QTime::currentTime().msecsTo(de.expires) <= 0)
		{
			qDebug("requesting status");
			request_init(de.ds, false);
			it.remove();
			break;
		}
		else if (!next_expires.isValid() || de.expires < next_expires)
			next_expires = de.expires;
	}

	if (!next_expires.isValid())
		qDebug("no more deferred status requests");
	else
	{
		qDebug("more deferred status requests");
		ms = QTime::currentTime().msecsTo(next_expires);
	}

	if (ms > 0)
	{
		qDebug("restarting deferred request timer with %d ms delay", ms);
		deferred_timer.start(ms);
	}
	else
		deferred_timer.stop();
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
void frame_interpreter::handle_frame_handler(char *f, QList<device_status*> sl)
{
	// TODO: rendere virtuale pura!!
	qDebug("frame_interpreter::handle_frame_handler");
}

// Set where
void frame_interpreter::set_where(QString s)
{
	qDebug() << "frame_interpreter::set_where(" << s << ")";
	where = s;
}


// Sound device frame interpreter

// Public methods
frame_interpreter_sound_device::frame_interpreter_sound_device(QString w, bool p, int g) :
	frame_interpreter(QString("16"), w, p, g)
{
}

void frame_interpreter_sound_device::get_init_messages(device_status *s, QStringList& out)
{
	QString head = "*#16*";
	QString end = "##";
	QString what;

	out.clear();
	what = "*1";
	out.append(head + where + what + end);
	what = "*5";
	out.append(head + where + what + end);
}

// Private methods
void frame_interpreter_sound_device::set_status(device_status_amplifier *ds, int l, int _on)
{
	qDebug("frame_interpreter_sound_device::set_status"
			"(device_status_amplifier, %d %d)", l, _on);
	stat_var curr_lev(stat_var::AUDIO_LEVEL);
	stat_var curr_status(stat_var::ON_OFF);
	bool do_event = false;
	if (_on != -1)
	{
		int on = _on ? 1 : 0;
		// Read current level and status
		ds->read((int)device_status_amplifier::ON_OFF_INDEX, curr_status);
		if (!curr_status.initialized())
		{
			qDebug("Initializing ampli status");
			curr_status.set_val(on);
			ds->write_val((int)device_status_amplifier::ON_OFF_INDEX, curr_status);
			do_event = true;
		}
		else
		{
			qDebug("Ampli status changed to %d", on);
			curr_status.set_val(on);
			ds->write_val((int)device_status_amplifier::ON_OFF_INDEX, curr_status);
			do_event = true;
		}
	}
	if (l >= 0)
	{
		ds->read((int)device_status_amplifier::AUDIO_LEVEL_INDEX, curr_lev);
		if (!curr_lev.initialized())
		{
			qDebug("Initializing audio level");
			curr_lev.set_val(l);
			ds->write_val((int)device_status_amplifier::AUDIO_LEVEL_INDEX,
					curr_lev);
			do_event = true;
			if (do_event)
					evt_list.append(ds);
			return;
		}
		qDebug("curr audio level is %d\n", curr_lev.get_val());
		qDebug("setting audio level to %d", l);
		curr_lev.set_val(l);
		ds->write_val((int)device_status_amplifier::AUDIO_LEVEL_INDEX, 
				curr_lev);
		do_event = 1;
	}

	if (do_event)
		evt_list.append(ds);
}

void frame_interpreter_sound_device::handle_frame(openwebnet_ext m, device_status_amplifier *ds)
{
	qDebug("frame_interpreter_sound_device::handle_frame");
	int lev;
	if (m.IsMeasureFrame())
	{
		// *#16*where*1*vol##
		int code = atoi(m.Extract_grandezza());
		switch (code)
		{
		case 1:
			lev = atoi(m.Extract_valori(0));
			// Only set level, don't touch status
			set_status(ds, lev, -1);
			break;
		default:
			qDebug("Unknown sound device meas frame, ignoring");
			break;
		}
	}
	else
	{
		int cosa = atoi(m.Extract_cosa());
		switch (cosa)
		{
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

void frame_interpreter_sound_device::handle_frame_handler(char *frame, QList<device_status*> sl)
{
	bool request_status = false;
	openwebnet_ext msg_open;
	rearmWDT();
	qDebug("frame_interpreter_sound_device::handle_frame_handler");
	qDebug("#### frame is %s ####", frame);
	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
	if (!is_frame_ours(msg_open, request_status)) // Discard frame if not ours
		return;

	evt_list.clear();

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (request_status)
		{
			request_init(ds);
			continue;
		}
		device_status::type type = ds->get_type();
		switch (type)
		{
			case device_status::AMPLIFIER:
				handle_frame(msg_open, (device_status_amplifier *)ds);
				break;
			default:
				// Do nothing
				break;
		}
	}

	if (!evt_list.isEmpty())
		emit frame_event(evt_list);
}

// Sound matrix frame interpreter
frame_interpreter_sound_matr_device::frame_interpreter_sound_matr_device(QString w, bool p, int g) :
	frame_interpreter(QString("16"), QString("1000"), p, g)
{
}

void frame_interpreter_sound_matr_device::get_init_message(device_status *ds, QString& out)
{
	out = QString("*#16*1000*11##");
}

void frame_interpreter_sound_matr_device::handle_frame_handler(char *frame, QList<device_status*> sl)
{
	bool request_status = false;
	openwebnet_ext msg_open;
	rearmWDT();
	qDebug("frame_interpreter_sound_matr_device::handle_frame_handler");
	qDebug("#### frame is %s ####", frame);
	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
	if ((atoi(msg_open.Extract_chi()) != 16) && (atoi(msg_open.Extract_chi()) != 22))
		return;

	evt_list.clear();

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (request_status)
		{
			request_init(ds);
			continue;
		}
		device_status::type type = ds->get_type();
		switch (type)
		{
		case device_status::SOUNDMATR:
			handle_frame(msg_open, (device_status_sound_matr *)ds);
			break;
		default:
			// Do nothing
			break;
		}
	}

	if (!evt_list.isEmpty())
		emit frame_event(evt_list);
}

void frame_interpreter_sound_matr_device::handle_frame(openwebnet_ext m, device_status_sound_matr *ds)
{
	int act;
	bool do_event = false;
	qDebug("frame_interpreter_sound_matr_device::handle_frame");
	if (m.IsMeasureFrame())
	{
		if (strcmp(m.Extract_chi(), "16") == 0)
		{
			// *#1*where*1*lev*speed##
			int code = atoi(m.Extract_grandezza());
			qDebug("frame_sound_matr_device::handle_frame, meas frame");
			switch (code)
			{
			case 11:
				for (int i=device_status_sound_matr::AMB1_INDEX;
						i<=device_status_sound_matr::AMB8_INDEX; i++)
				{
					stat_var curr_act(stat_var::ACTIVE_SOURCE);
					ds->read(i, curr_act);
					qDebug("Curr active source for amb %d = %d", i+1, curr_act.get_val());
					act = atoi(m.Extract_valori(i));
					qDebug("New active source = %d", act);
					if (act != curr_act.get_val())
					{
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
	}
	else
	{
		//*22*2#4#1*5#2#3##
		if ((strcmp(m.Extract_chi(), "22") == 0) && (strncmp(m.Extract_cosa(), "2#4#", 4) == 0) &&
			(strcmp(m.Extract_dove(), "5") == 0) && (strcmp(m.Extract_livello(), "2") == 0))
		{
			qDebug("frame_sound_matr_device::handle_frame, normal frame WHO = 22");
			stat_var curr_act(stat_var::ACTIVE_SOURCE);
			char ambiente[3];
			sprintf(ambiente,"%d", atoi(m.Extract_cosa()+4));
			if ((atoi(ambiente) != 0) && (atoi(ambiente) != 15))
			{
				ds->read(atoi(ambiente)-1, curr_act);
				qDebug("Curr active source for amb %s = %d", ambiente, curr_act.get_val());
				act = atoi(m.Extract_interfaccia());
				qDebug("New active source = %d", act);
				curr_act.set_val(act);
				ds->write_val(atoi(ambiente)-1, curr_act);
				do_event = true;
			}
		}
	}
	if (do_event)
		evt_list.append(ds);
}


// Radio device frame interpreter

// Public methods
frame_interpreter_radio_device::frame_interpreter_radio_device(QString w, bool p, int g) :
	frame_interpreter(QString("16"), w, p, g)
{
}

void frame_interpreter_radio_device::get_init_message(device_status *s, QString& out)
{
	QString head = "*#16*";
	QString end = "##";
	// Fixme: e` giusto questo ?
	QString what = "*6";
	out = head + where + what + end;
}

bool frame_interpreter_radio_device::is_frame_ours(openwebnet_ext m, bool& request_status)
{
	qDebug("frame_interpreter_radio_device::is_frame_ours");
	qDebug() << "who = " << who << " , where = " << where;
	qDebug() << "msg who = " << m.Extract_chi() << ", msg where = " << m.get_where();
	if (who != m.Extract_chi())
		return false;
	QString w = m.get_where();
	// Consider only "least significant part" of where, here
	int wh = w.toInt();
	while (wh >= 100)
		wh -= 100;
	while (wh >= 10)
		wh -= 10;
	bool out = m.is_target(this, get_who(), QString::number(wh, 10), 
			request_status);
	if (out)
	{
		qDebug("FRAME IS OURS !!");
	}
	return out;
}

// Private methods
void frame_interpreter_radio_device::handle_frame(openwebnet_ext m, device_status_radio *ds)
{
	qDebug("frame_interpreter_radio_device::handle_frame");
	int freq, staz;
	bool do_event = false;

	// TODO: remove duplicate code from RDS stat var!!
	stat_var curr_freq(stat_var::FREQ);
	stat_var curr_staz(stat_var::STAZ);
	stat_var curr_rds0(stat_var::RDS0);
	stat_var curr_rds1(stat_var::RDS1);
	stat_var curr_rds2(stat_var::RDS2);
	stat_var curr_rds3(stat_var::RDS3);
	stat_var curr_rds4(stat_var::RDS4);
	stat_var curr_rds5(stat_var::RDS5);
	stat_var curr_rds6(stat_var::RDS6);
	stat_var curr_rds7(stat_var::RDS7);

	if (m.IsMeasureFrame())
	{
		int code = atoi(m.Extract_grandezza());
		switch (code)
		{
		case 6:
			qDebug("Frequenza nel messaggio = %s", m.Extract_valori(1));
			freq = strtol(m.Extract_valori(1), NULL, 10);
			ds->read((int)device_status_radio::FREQ_INDEX, curr_freq);
			curr_freq.set_val(freq);
			qDebug("Setting frequency to %d", freq);
			ds->write_val((int)device_status_radio::FREQ_INDEX, curr_freq);
			staz = 0;
			ds->read((int)device_status_radio::STAZ_INDEX, curr_staz);
			curr_staz.set_val(staz);
			qDebug("Setting staz to NULL");
			ds->write_val((int)device_status_radio::STAZ_INDEX, curr_staz);
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
			int rds[8];

			for (int idx=0;idx<8;idx++)
			{
				rds[idx] = strtol(m.Extract_valori(idx), NULL, 10);
				qDebug("RDS: char[%d] = chr(%d)", idx, rds[idx]);
			}

			ds->read((int)device_status_radio::RDS0_INDEX, curr_rds0);
			curr_rds0.set_val(rds[0]);
			ds->write_val((int)device_status_radio::RDS0_INDEX, curr_rds0);

			ds->read((int)device_status_radio::RDS1_INDEX, curr_rds1);
			curr_rds1.set_val(rds[1]);
			ds->write_val((int)device_status_radio::RDS1_INDEX, curr_rds1);

			ds->read((int)device_status_radio::RDS2_INDEX, curr_rds2);
			curr_rds2.set_val(rds[2]);
			ds->write_val((int)device_status_radio::RDS2_INDEX, curr_rds2);

			ds->read((int)device_status_radio::RDS3_INDEX, curr_rds3);
			curr_rds3.set_val(rds[3]);
			ds->write_val((int)device_status_radio::RDS3_INDEX, curr_rds3);

			ds->read((int)device_status_radio::RDS4_INDEX, curr_rds4);
			curr_rds4.set_val(rds[4]);
			ds->write_val((int)device_status_radio::RDS4_INDEX, curr_rds4);

			ds->read((int)device_status_radio::RDS5_INDEX, curr_rds5);
			curr_rds5.set_val(rds[5]);
			ds->write_val((int)device_status_radio::RDS5_INDEX, curr_rds5);

			ds->read((int)device_status_radio::RDS6_INDEX, curr_rds6);
			curr_rds6.set_val(rds[6]);
			ds->write_val((int)device_status_radio::RDS6_INDEX, curr_rds6);

			ds->read((int)device_status_radio::RDS7_INDEX, curr_rds7);
			curr_rds7.set_val(rds[7]);
			ds->write_val((int)device_status_radio::RDS7_INDEX, curr_rds7);

			do_event = true;
			break;
		default:
			qDebug("Unknown radio device meas frame, ignoring");
			break;
		}
	}
	if (do_event)
		evt_list.append(ds);
}

void frame_interpreter_radio_device::handle_frame_handler(char *frame, QList<device_status*> sl)
{
	bool request_status = false;
	openwebnet_ext msg_open;
	rearmWDT();
	qDebug("frame_interpreter_radio_device::handle_frame_handler");
	qDebug("#### frame is %s ####", frame);
	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
	if (!is_frame_ours(msg_open, request_status)) // Discard frame if not ours
		return;

	evt_list.clear();

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (request_status)
		{
			request_init(ds);
			continue;
		}
		device_status::type type = ds->get_type();
		switch (type)
		{
		case device_status::RADIO:
			handle_frame(msg_open, (device_status_radio *)ds);
			break;
		default:
			// Do nothing
			break;
		}
	}

	if (!evt_list.isEmpty())
		emit frame_event(evt_list);
}



// Doorphone device frame interpreter

// Public methods
frame_interpreter_doorphone_device::frame_interpreter_doorphone_device(QString w, bool p, int g) :
	frame_interpreter(QString("6"), w, p, g)
{
	qDebug("frame_interpreter_doorphone_device::"
			"frame_interpreter_doorphone_device()");
}

void frame_interpreter_doorphone_device::get_init_message(device_status *s, QString& out)
{
	out = QString("");
}

bool frame_interpreter_doorphone_device::is_frame_ours(openwebnet_ext m, bool& request_status)
{
	char tmp[20];
	request_status = false;

	qDebug("frame_interpreter_doorphone_device::is_frame_ours");
	qDebug() << "who = " << who << " , where = " << where;
	char *c = m.Extract_chi();
	qDebug() << "msg who = " << c << ", msg where = " << m.Extract_dove();
	QString cosa = m.Extract_cosa();
	qDebug() << "cosa = " << cosa;
	if (!cosa.startsWith("9")) return false;
	// I posti esterni hanno chi = 8
	if (strcmp(c, "8")) return false;
	if (where == "ANY") return true;
	int d;
	QString w = &m.Extract_dove()[1];
	qDebug() << "w = " << w << ", w.toInt =" << w.toInt();
	d = 4000 + w.toInt();
	if (m.estesa)
		sprintf(tmp, "%d#%s", d, m.Extract_livello());
	else 
		sprintf(tmp, "%d", d);

	qDebug() << "compare msg where = " << tmp << " and  where = " << where;
	return !where.compare(tmp);
	// NEVER REACHED
}

// Private methods
void frame_interpreter_doorphone_device::set_status(device_status_doorphone *ds, int s)
{
	qDebug("frame_interpreter_doorphone_device::set_status"
			"(device_status_doorphone, %d)", s);
	// When we get here, we surely got a call. So trigger a frame event
	evt_list.append(ds);
}

void frame_interpreter_doorphone_device::handle_frame(openwebnet_ext m,
	device_status_doorphone *ds)
{
	qDebug("frame_interpreter_doorphone_device::handle_frame");
	// FIXME: REALLY HANDLE FRAME
	evt_list.append(ds);
}

void frame_interpreter_doorphone_device::handle_frame_handler(char *frame, QList<device_status*> sl)
{
	bool request_status = false;
	openwebnet_ext msg_open;
	rearmWDT();
	qDebug("frame_interpreter_doorphone_device::handle_frame_handler");
	qDebug("#### frame is %s ####", frame);
	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
	if (!is_frame_ours(msg_open, request_status))
	{
		// Discard frame if not ours
		qDebug("discarding frame");
		return;
	}

	evt_list.clear();

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (request_status)
		{
			request_init(ds);
			continue;
		}
		device_status::type type = ds->get_type();
		switch (type)
		{
		case device_status::DOORPHONE:
			handle_frame(msg_open, (device_status_doorphone *)ds);
			break;
		default:
			// Do nothing
			break;
		}
	}

	if (!evt_list.isEmpty())
		emit frame_event(evt_list);
}


// Anti-intrusion system device frame interpreter

// Public methods
frame_interpreter_impanti_device::frame_interpreter_impanti_device(QString w, bool p, int g) :
	frame_interpreter(QString("5"), w, p, g)
{
	qDebug("frame_interpreter_impanti_device::frame_interpreter_impanti_device()");
}

void frame_interpreter_impanti_device::get_init_message(device_status *s, QString& out)
{
	out = QString("*#5*0##");
}

bool frame_interpreter_impanti_device::is_frame_ours(openwebnet_ext m, bool& request_status)
{
	// FIXME: IS THIS OK ?
	request_status = false;
	qDebug("frame_interpreter_impanti_device::is_frame_ours");
	qDebug() << "who = " << who << " , where = " << where;
	char *c = m.Extract_chi();
	qDebug() << "msg who = " << c << ", msg where = " << m.Extract_dove();
	int l, i;
	if (QString(c) != who)
		return false;
	return m.gen(l,i) || QString(m.Extract_dove()) == "";
}

// Private methods
void frame_interpreter_impanti_device::handle_frame(openwebnet_ext m, device_status_impanti *ds)
{
	qDebug("frame_interpreter_impanti_device::handle_frame");
	stat_var curr_stat(stat_var::ON_OFF);
	int stat;
	// Read current status of ON OFF variable
	ds->read((int)device_status_impanti::ON_OFF_INDEX, curr_stat);
	qDebug("curr status is %d\n", curr_stat.get_val());
	int cosa = atoi(m.Extract_cosa());
	switch (cosa)
	{
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
	if (stat < 0)
		return;
	if (ds->initialized() && (stat == curr_stat.get_val()))
		return;
	curr_stat.set_val(stat);
	ds->write_val((int)device_status_impanti::ON_OFF_INDEX, curr_stat);
	evt_list.append(ds);
}

void frame_interpreter_impanti_device::handle_frame_handler(char *frame, QList<device_status*> sl)
{
	bool request_status;
	openwebnet_ext msg_open;
	rearmWDT();
	qDebug("frame_interpreter_impanti_device::handle_frame_handler");
	qDebug("#### frame is %s ####", frame);
	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
	if (!is_frame_ours(msg_open, request_status)) // Discard frame if not ours
		return;

	evt_list.clear();

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (request_status)
		{
			request_init(ds);
			continue;
		}
		device_status::type type = ds->get_type();
		switch (type)
		{
		case device_status::IMPANTI:
			handle_frame(msg_open, (device_status_impanti *)ds);
			break;
		default:
			// Do nothing
			break;
		}
	}

	if (!evt_list.isEmpty())
		emit frame_event(evt_list);
}


// Anti-intrusion system device frame interpreter (single zone)

// Public methods
frame_interpreter_zonanti_device::frame_interpreter_zonanti_device(QString w, bool p, int g) :
	frame_interpreter(QString("5"), w, p, g)
{
	qDebug("frame_interpreter_zonanti_device::"
			"frame_interpreter_zonanti_device()");
}

void frame_interpreter_zonanti_device::get_init_message(device_status *s, QString& out)
{
	QString head = "*#5*";
	QString end = "##";
	out = head + where + end;
}

bool frame_interpreter_zonanti_device::is_frame_ours(openwebnet_ext m, bool& request_status)
{
	// FIXME: IS THIS OK ?
	request_status = false;
	qDebug("frame_interpreter_impanti_device::is_frame_ours");
	qDebug() << "who = " << who << " , where = " << where;
	char *c = m.Extract_chi();
	char *d = m.Extract_dove();
	qDebug() << "msg who = " << c << ", msg where = " << d;
	return ((QString(c) == who) && (QString(d) == where));
}

// Private methods
void frame_interpreter_zonanti_device::handle_frame(openwebnet_ext m, device_status_zonanti *ds)
{
	qDebug("frame_interpreter_zonanti_device::handle_frame");
	stat_var curr_stat(stat_var::ON_OFF);
	int stat;
	// Read current status of ON OFF variable
	ds->read((int)device_status_zonanti::ON_OFF_INDEX, curr_stat);
	qDebug("curr status is %d\n", curr_stat.get_val());
	int cosa = atoi(m.Extract_cosa());
	switch (cosa)
	{
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
	if (stat < 0)
		return;
	curr_stat.set_val(stat);
	ds->write_val((int)device_status_zonanti::ON_OFF_INDEX, curr_stat);
	qDebug("status changed, appending evt");
	evt_list.append(ds);
}

void frame_interpreter_zonanti_device::handle_frame_handler(char *frame, QList<device_status*> sl)
{
	bool request_status = false;
	openwebnet_ext msg_open;
	rearmWDT();
	qDebug("frame_interpreter_zonanti_device::handle_frame_handler");
	qDebug("#### frame is %s ####", frame);
	msg_open.CreateMsgOpen(frame,strstr(frame,"##")-frame+2);
	if (!is_frame_ours(msg_open, request_status))
		return;

	evt_list.clear();

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		if (request_status)
		{
			request_init(ds);
			continue;
		}
		device_status::type type = ds->get_type();
		switch (type)
		{
		case device_status::ZONANTI:
			handle_frame(msg_open, (device_status_zonanti *)ds);
			break;
		default:
			// Do nothing
			break;
		}
	}

	if (!evt_list.isEmpty())
		emit frame_event(evt_list);
}


//=================================================================================================================
// MCI frame interpreter

// Public methods
frame_interpreter_mci::frame_interpreter_mci(QString w, bool p, int g) :
	frame_interpreter(QString("18"), w, p, g)
{
}

void frame_interpreter_mci::get_init_messages(device_status *s, QStringList& out)
{
	QString head = "*#18*";
	QString end = "##";

	out.clear();
	switch (s->get_type())
	{
	case device_status::SUPERVISION_MCI:
		out.append(head + where + "*250" + end);
		out.append(head + where + "*212" + end);
		break;
	default:
		qDebug("frame_interpreter_mci: unhandled device status type");
	}
}

void frame_interpreter_mci::handle_frame_handler(char *frame, QList<device_status*> sl)
{
	bool request_status;
	openwebnet_ext msg_open;
	rearmWDT();
	qDebug("frame_interpreter_mci::handle_frame_handler");
	qDebug("#### frame is %s ####", frame);

	msg_open.CreateMsgOpen(frame, strstr(frame, "##")-frame+2);
	if (!is_frame_ours(msg_open, request_status)) // Discard frame if not ours
		return;

	evt_list.clear();

	for (int i = 0; i < sl.size(); ++i)
	{
		device_status *ds = sl.at(i);
		device_status::type type = ds->get_type();
		switch (type)
		{
		case device_status::SUPERVISION_MCI:
			handle_frame(msg_open, (device_status_mci *)ds);
			break;
		default:
			// Do nothing
			break;
		}
	}

	if (!evt_list.isEmpty())
	{
		qDebug("Event list is not empty, invoking emit(frame_event())");
		emit frame_event(evt_list);
	}
	else
		qDebug("**** NO event generated");

	qDebug("frame_interpreter_mci::handle_frame_handler, end");
}

void frame_interpreter_mci::handle_frame(openwebnet_ext m, device_status_mci *ds)
{
	qDebug("frame_interpreter_mci::handle_frame(device_status_mci)");

	if (m.IsNormalFrame())
	{
		qDebug("frame_interpreter_mci::handle_frame - IsNormalFrame");
	}
	else if (m.IsMeasureFrame())
	{
		qDebug("frame_interpreter_mci::handle_frame - IsMeasureFrame");
		int code = atoi(m.Extract_grandezza());
		bool doEvent = false;
		char* s = m.Extract_valori(0);
		int val = 0;
		switch (code)
		{
		case 250:   //MCI Status
			for (int i=0; i<13; i++)
				val = (val<<1)|(s[i]-0x30);
			qDebug("set_status() - MCI Status");
			if (ds->SetMciStatus(val))
				doEvent = true;
			break;
		case 212:   //Autotest freq
			val = atoi(s);
			qDebug("set_status() - Autotest freq");
			if (ds->SetMciAutotestFreq(val))
				doEvent = true;
			break;
		default:
			//do nothing
			break;
		}
		if (doEvent)
			evt_list.append(ds);
	}
	else if (m.IsWriteFrame())
	{
		qDebug("frame_interpreter_mci::handle_frame - IsWriteFrame");
	}
}
