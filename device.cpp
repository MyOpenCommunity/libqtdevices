// Device implementation
#include <qstring.h>
#include <qptrlist.h>
#include <qobject.h>
#include <qstringlist.h>

#include "openclient.h"
#include "device.h"
#include "frame_interpreter.h"
#include "device_cache.h"

// Device implementation

device::device(QString _who, QString _where, bool p, int g)
{
	who = _who;
	where = _where;
	pul = p;
	group = g;
	refcount = 0;
	stat = new QPtrList<device_status>;
}

void device::setClients(Client *comandi, Client *monitor, Client *richieste)
{
	client_comandi = comandi;
	client_monitor = monitor;
	client_richieste = richieste;
}

void device::sendFrame(const char *frame)
{
	client_comandi->ApriInviaFrameChiudi(frame);
}

void device::sendInit(const char *frame)
{
	client_richieste->ApriInviaFrameChiudi(frame);
}

void device::init(bool force )
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
		if(force){
			qDebug("device status force initialize");
			interpreter->get_init_messages(ds, msgl);
			for ( QStringList::Iterator it = msgl.begin(); it != msgl.end(); ++it ) {
				qDebug("init message is %s", (*it).ascii());
				if((*it) != "")
					emit(send_frame((char *)((*it).ascii())));
			}
		}
		else
			if(ds->initialized()) {
				qDebug("device status has already been initialized");
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

// MCI implementation
mci_device::mci_device(QString w, bool p, int g) : device(QString("18"), w, p, g)
{
	interpreter = new frame_interpreter_mci(w, p, g);
	set_frame_interpreter(interpreter);
	stat->append(new device_status_mci());
	connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)),
			    interpreter, SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
	connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), 
					this, SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Light implementation
light::light(QString w, bool p, int g) : device(QString("1"), w, p, g) 
{
	interpreter = new frame_interpreter_lights(w, p, g);
	set_frame_interpreter(interpreter);
	stat->append(new device_status_light());
	connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)),
			interpreter, 
			SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
	connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this, 
			SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Dimmer implementation
dimm::dimm(QString w, bool p, int g) : device(QString("1"), w, p, g)
{
	interpreter = new frame_interpreter_dimmer(w, p, g);
	set_frame_interpreter(interpreter);
	stat->append(new device_status_dimmer());
	connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)),
			interpreter,
			SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
	connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)), this,
			SLOT(frame_event_handler(QPtrList<device_status>)));
}

// Dimmer100 implementation
dimm100::dimm100(QString w, bool p, int g) : device(QString("1"), w, p, g)
{
	interpreter = new frame_interpreter_lights(w, p, g);
	set_frame_interpreter(interpreter);
	stat->append(new device_status_dimmer100());
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

thermal_regulator::thermal_regulator(QString where, bool p, int g)
	: device(QString("4"), where, p, g)
{
}

void thermal_regulator::setOff()
{
	openwebnet msg_open;
	QString msg = QString("*%1*%2*%3##").arg(who).arg(QString::number(GENERIC_OFF)).arg(QString("#") + where);
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

void thermal_regulator::setProtection()
{
	openwebnet msg_open;
	QString msg = QString("*%1*%2*%3##").arg(who).arg(QString::number(GENERIC_PROTECTION)).arg(QString("#") + where);
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

void thermal_regulator::setSummer()
{
	openwebnet msg_open;
	QString msg = QString("*%1*%2*%3##").arg(who).arg(QString::number(SUMMER)).arg(QString("#") + where);
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

void thermal_regulator::setWinter()
{
	openwebnet msg_open;
	QString msg = QString("*%1*%2*%3##").arg(who).arg(QString::number(WINTER)).arg(QString("#") + where);
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

void thermal_regulator::setManualTemp(unsigned temperature)
{
	// temperature is 4 digits wide
	unsigned temp_width = 4;
	// prepend some 0 if temperature is positive
	QString padded_temperature = QString::number(temperature).rightJustify(temp_width, '0');

	// sanity check on input
	if (temperature > 1000)
	{
		unsigned tmp = temperature - 1000;
		if (tmp < 50 || tmp > 400)
			return;
	}

	const QString sharp_where = QString("#") + where;
	QString what = QString::number(TEMPERATURE_SET) + "*";
	what += padded_temperature + "*" + QString::number(GENERIC_MODE);

	QString msg = QString("*#") + who + "*" + sharp_where + "*#" + what + "##";
	openwebnet msg_open;
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

void thermal_regulator::setWeekProgram(int program)
{
	const QString what = QString::number(WEEK_PROGRAM + program);
	const QString sharp_where = QString("#") + where;
	QString msg = QString("*") + who + "*" + what + "*" + sharp_where + "##";
	openwebnet msg_open;
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

void thermal_regulator::setWeekendDateTime(QDate date, QTime time, int program)
{
	const QString sharp_where = QString("#") + where;

	// we need to send 3 frames
	// - frame at par. 2.3.9, to set what program has to be executed at the end of weekend mode
	// - frame at par. 2.3.16 to set date
	// - frame at par. 2.3.17 to set time
	//
	// First frame: set program
	const int what_program = WEEK_PROGRAM + program;
	QString what = QString::number(GENERIC_WEEKEND) + "#" + QString::number(what_program);

	openwebnet msg_open;
	QString msg = QString("*") + who + "*" + what + "*" + sharp_where + "##";
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);

	// Second frame: set date
	setHolidayEndDate(date);

	// Third frame: set time
	setHolidayEndTime(time);
}

void thermal_regulator::setHolidayDateTime(QDate date, QTime time, int program)
{
	const QString sharp_where = QString("#") + where;

	// we need to send 3 frames, as written in bug #44
	// - frame at par. 2.3.10, with number_of_days = 2 (dummy number, we set end date and time explicitly with
	// next frames), to set what program has to be executed at the end of holiday mode
	// - frame at par. 2.3.16 to set date
	// - frame at par. 2.3.17 to set time
	//
	// First frame: set program
	const int number_of_days = 2;
	const int what_days = HOLIDAY_NUM_DAYS + number_of_days;
	const int what_program = WEEK_PROGRAM + program;

	openwebnet msg_open;
	QString msg = QString("*%1*%2#%3*%4##").arg(who).arg(what_days).arg(what_program).arg(sharp_where);
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);

	// Second frame: set date
	setHolidayEndDate(date);

	// Third frame: set time
	setHolidayEndTime(time);
}

void thermal_regulator::setHolidayEndDate(QDate date)
{
	const QString sharp_where = QString("#") + where;

	const unsigned date_width = 2;
	const QString day = QString::number(date.day()).rightJustify(date_width, '0');
	const QString month = QString::number(date.month()).rightJustify(date_width, '0');
	const QString year = QString::number(date.year());
	const QString date_end = QString::number(HOLIDAY_DATE_END);

	openwebnet msg_open;
	QString msg = QString("*#") + who + "*" + sharp_where + "*#" + date_end + "*" + day + "*" + month + "*" + year + "##";
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

void thermal_regulator::setHolidayEndTime(QTime time)
{
	const QString sharp_where = QString("#") + where;

	// Time numbers are 2 digits wide
	const unsigned time_width = 2;
	const QString hour = QString::number(time.hour()).rightJustify(time_width, '0');
	const QString minute = QString::number(time.minute()).rightJustify(time_width, '0');
	const QString time_end = QString::number(HOLIDAY_TIME_END);

	openwebnet msg_open;
	QString msg = QString("*#%1*%2*#%3*%4*%5##").arg(who).arg(sharp_where).arg(time_end).arg(hour).arg(minute);
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

thermal_regulator_4z::thermal_regulator_4z(QString where, bool p, int g)
	: thermal_regulator(where, p, g)
{
	interpreter = new frame_interpreter_thermal_regulator(who, where, p, g);
	set_frame_interpreter(interpreter);
	stat->append(new device_status_thermal_regulator_4z());

	connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)),
			interpreter, SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
	connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)),
			this, SLOT(frame_event_handler(QPtrList<device_status>)));
}

void thermal_regulator_4z::setManualTempTimed(int temperature, QTime time)
{
	const QString sharp_where = QString("#") + where;

	// we need to send 2 frames
	// - frame at par. 2.3.13, with number_of_hours = 2 (dummy number, we set end time explicitly with
	// next frame), to set the temperature of timed manual operation
	// - frame at par. 2.3.18 to set hours and minutes of mode end. Timed manual operation can't last longer
	// than 24 hours.
	//
	// First frame: set temperature
	const QString number_of_hours = "2";
	const unsigned temp_width = 4;
	const QString padded_temp = QString::number(temperature).rightJustify(temp_width, '0');
	const QString what = QString::number(GENERIC_MANUAL_TIMED) + "#" + padded_temp + "#" + number_of_hours;

	openwebnet msg_open;
	QString msg = QString("*") + who + "*" + what + "*" + sharp_where + "##";
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);

	// Second frame: set end time
	const unsigned time_width = 2;
	const QString hour = QString::number(time.hour()).rightJustify(time_width, '0');
	const QString minute = QString::number(time.minute()).rightJustify(time_width, '0');

	const QString what2 = QString("#") + QString::number(MANUAL_TIMED_END) + "*" + hour + "*" + minute;
	openwebnet msg_open_2;
	msg = QString("*#") + who + "*" + sharp_where + "*" + what2 + "##";
	msg_open_2.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open_2.frame_open);
}

thermal_regulator_99z::thermal_regulator_99z(QString where, bool p, int g)
	: thermal_regulator(where, p, g)
{
	interpreter = new frame_interpreter_thermal_regulator(who, where, p, g);
	set_frame_interpreter(interpreter);
	stat->append(new device_status_thermal_regulator_99z());

	connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)),
			interpreter, SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
	connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)),
			this, SLOT(frame_event_handler(QPtrList<device_status>)));
}

void thermal_regulator_99z::setScenario(int scenario)
{
	const QString what = QString::number(SCENARIO_PROGRAM + scenario);
	const QString sharp_where = QString("#") + where;
	QString msg = QString("*") + who + "*" + what + "*" + sharp_where + "##";
	openwebnet msg_open;
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

// Controlled temperature probe implementation
temperature_probe_controlled::temperature_probe_controlled(QString w, thermo_type_t type, bool _fancoil,
		const char *ind_centrale, const char *indirizzo, bool p, int g) :
	device(QString("4"), w, p, g)
{
	qDebug("temperature_probe_controlled::temperature_probe_controlled(), type=%d, fancoil=%s", type, _fancoil ? "true" : "false");
	interpreter = new frame_interpreter_temperature_probe_controlled(w, type, ind_centrale, indirizzo, p, g);
	set_frame_interpreter(interpreter);
	stat->append(new device_status_temperature_probe_extra(type));
	stat->append(new device_status_temperature_probe());
	fancoil = _fancoil;
	simple_where = indirizzo;
	if (fancoil)
		stat->append(new device_status_fancoil());

	connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)),
			interpreter, SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
	connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)),
			this, SLOT(frame_event_handler(QPtrList<device_status>)));
}

void temperature_probe_controlled::setManual(unsigned setpoint)
{
	const unsigned dimension = 14;
	const unsigned mode = 3; // generic mode
	const unsigned temp_width = 4;
	const QString temp = QString::number(setpoint).rightJustify(temp_width, '0');

	QString msg = QString("*#") + who + QString("*#") + where + QString("*") + QString("#") +
		QString::number(dimension) + QString("*") + temp + QString("*") + QString::number(mode) + QString("##");

	openwebnet msg_open;
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

void temperature_probe_controlled::setAutomatic()
{
	unsigned mode = 311; // generic mode

	QString msg = QString("*") + who + "*" + QString::number(mode) + QString("*#") + where + QString("##");
	openwebnet msg_open;
	msg_open.CreateMsgOpen(const_cast<char *> (msg.ascii()), msg.length());
	sendFrame(msg_open.frame_open);
}

void temperature_probe_controlled::setFancoilSpeed(int speed)
{
	const unsigned dimension = 11;
	if (fancoil)
	{
		QString msg = QString("*#") + who + "*" + simple_where + "*#" + QString::number(dimension) + "*" +
			QString::number(speed) + "##";
		openwebnet msg_open;
		msg_open.CreateMsgOpen(const_cast<char *>(msg.ascii()), msg.length());
		sendFrame(msg_open.frame_open);
	}
}

void temperature_probe_controlled::requestFancoilStatus()
{
	const unsigned dimension = 11;
	if (fancoil)
	{
		QString msg = QString("*#") + who + "*" + simple_where + "*" + QString::number(dimension) + "##";
		openwebnet msg_open;
		msg_open.CreateMsgOpen(const_cast<char *>(msg.ascii()), msg.length());
		sendFrame(msg_open.frame_open);
	}
}

// Not controlled temperature probe implementation
temperature_probe_notcontrolled::temperature_probe_notcontrolled(QString w, bool external, bool p, int g) :
	device(QString("4"), w, p, g)
{
	interpreter = new frame_interpreter_temperature_probe(w, external, p, g);
	set_frame_interpreter(interpreter);
	stat->append(new device_status_temperature_probe());

	connect(this, SIGNAL(handle_frame(char *, QPtrList<device_status> *)),
	        interpreter, SLOT(handle_frame_handler(char *, QPtrList<device_status> *)));
	connect(interpreter, SIGNAL(frame_event(QPtrList<device_status>)),
	        this, SLOT(frame_event_handler(QPtrList<device_status>)));
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
