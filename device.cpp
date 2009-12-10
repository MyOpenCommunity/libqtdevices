#include "device.h"
#include "openclient.h"
#include "frame_interpreter.h"
#include "bttime.h"
#include "generic_functions.h" // createRequestOpen, createMsgOpen

#include <openmsg.h>

#include <QStringList>
#include <QDebug>

// Inizialization of static member
Client *device::client_comandi = 0;
Client *device::client_richieste = 0;


FrameCompressor::FrameCompressor(int timeout, int w)
{
	what = w;
	timer.setInterval(timeout);
	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), SLOT(emitFrame()));
}

bool FrameCompressor::analyzeFrame(const QString &frame_open)
{
	OpenMsg msg(frame_open.toStdString());

	// TODO: accept more than one what
	if (what == -1)
	{
		frame = frame_open;
		timer.start();
		return true;
	}
	else if (what == msg.what())
	{
		frame = frame_open;
		timer.start();
		return true;
	}
	else
		return false;
}

void FrameCompressor::emitFrame()
{
	qDebug() << "FrameCompressor, now emitting frame: " << frame;
	emit compressedFrame(frame);
}


// Device implementation

device::device(QString _who, QString _where, bool p, int g) : interpreter(0)
{
	who = _who;
	where = _where;
	pul = p;
	group = g;
	refcount = 0;
	cmd_compressor = 0;
	req_compressor = 0;
	subscribe_monitor(who.toInt());
}

void device::sendFrame(QString frame) const
{
	Q_ASSERT_X(client_comandi, "device::sendFrame", "Client comandi not set!");
	QByteArray buf = frame.toAscii();
	client_comandi->ApriInviaFrameChiudi(buf.constData());
}

void device::sendInit(QString frame) const
{
	Q_ASSERT_X(client_richieste, "device::sendInit", "Client richieste not set!");
	QByteArray buf = frame.toAscii();
	client_richieste->ApriInviaFrameChiudi(buf.constData());
}

void device::sendCompressedFrame(const QString &frame) const
{
	if (cmd_compressor)
	{
		if (!cmd_compressor->analyzeFrame(frame))
			sendFrame(frame);
	}
	else
		sendFrame(frame);
}

void device::sendCompressedInit(const QString &frame) const
{
	if (req_compressor)
	{
		if (!req_compressor->analyzeFrame(frame))
			sendInit(frame);
	}
	else
		sendInit(frame);
}

void device::sendCommand(QString what) const
{
	sendFrame(createMsgOpen(who, what, where));
}

void device::sendRequest(QString what) const
{
	if (what.isEmpty())
		sendInit(createStatusRequestOpen(who, where));
	else
		sendInit(createRequestOpen(who, what, where));
}

void device::setClients(Client *command, Client *request)
{
	client_comandi = command;
	client_richieste = request;
}

void device::installFrameCompressor(int timeout, int what)
{
	// TODO: add the possibility to install a compressor on a specific what
	// TODO: add an interface to install the compressors independently
	if (!cmd_compressor)
	{
		cmd_compressor = new FrameCompressor(timeout);
		connect(cmd_compressor, SIGNAL(compressedFrame(QString)), SLOT(sendFrame(QString)));
	}
	if (!req_compressor)
	{
		req_compressor = new FrameCompressor(timeout);
		connect(req_compressor, SIGNAL(compressedFrame(QString)), SLOT(sendInit(QString)));
	}
}

void device::init(bool force)
{
	QList<device_status*> dsl;
	for (int i = 0; i < stat.size(); ++i)
	{
		device_status *ds = stat.at(i);

		QStringList msgl;
		msgl.clear();
		if (force)
		{
			Q_ASSERT_X(interpreter, "device::init", "interpreter not set!");
			interpreter->get_init_messages(ds, msgl);
			for (QStringList::Iterator it = msgl.begin(); it != msgl.end(); ++it)
			{
				if (*it != "")
					sendFrame(*it);
			}
		}
		else
		{
			if (ds->initialized())
			{
				emit initialized(ds);
				dsl.append(ds);
			}
			else if (ds->init_requested())
				qDebug("device status init already requested");
			else
			{
				Q_ASSERT_X(interpreter, "device::init", "interpreter not set!");
				interpreter->get_init_messages(ds, msgl);
				for (QStringList::Iterator it = msgl.begin();it != msgl.end(); ++it)
				{
					if (*it != "")
						sendFrame(*it);
				}
				ds->mark_init_requested();
			}
		}
	}
	if (!dsl.isEmpty())
		emit status_changed(dsl);
}

void device::init_requested_handler(QString msg)
{
	qDebug("device::init_requested_handler()");
	if (msg != "")
		sendFrame(msg);
}

void device::set_where(QString w)
{
	qDebug() << "device::set_where(" << w << ")";
	where = w;
	if (interpreter)
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

	for (int i = 0; i < stat.size(); ++i)
	{
		device_status *ds = stat.at(i);
		if (ds->get_type() == _ds->get_type())
		{
			qDebug("Status already there, skip");
			return;
		}
	}
	stat.append(_ds);
}

QString device::get_key()
{
	return who + "*" + where;
}

device::~device()
{
	while (!stat.isEmpty())
		delete stat.takeFirst();
}

// TODO: this method is just for compatibility, remove it when all the device
// will support the new "manageFrame interface".
void device::manageFrame(OpenMsg &msg)
{
	frame_rx_handler(msg.frame_open);
}

void device::frame_rx_handler(char *s)
{
	qDebug("device::frame_rx_handler");
	emit handle_frame(s, stat);
}

void device::get()
{
	++refcount;
}

int device::put()
{
	--refcount;
	return refcount;
}

void device::setup_frame_interpreter(frame_interpreter* i)
{
	interpreter = i;

	connect(this, SIGNAL(handle_frame(char *, QList<device_status*>)),
			i, SLOT(handle_frame_handler(char *, QList<device_status*>)));

	connect(i, SIGNAL(frame_event(QList<device_status*>)),
			SIGNAL(status_changed(QList<device_status*>)));

	connect(i, SIGNAL(init_requested(QString)), SLOT(init_requested_handler(QString)));
}


// MCI implementation
mci_device::mci_device(QString w, bool p, int g) : device(QString("18"), w, p, g)
{
	stat.append(new device_status_mci());
	setup_frame_interpreter(new frame_interpreter_mci(w, p, g));
}

// Light implementation
light::light(QString w, bool p, int g) : device(QString("1"), w, p, g) 
{
	stat.append(new device_status_light());
	setup_frame_interpreter(new frame_interpreter_lights(w, p, g));
}

// Dimmer implementation
dimm::dimm(QString w, bool p, int g) : device(QString("1"), w, p, g)
{
	stat.append(new device_status_dimmer());
	setup_frame_interpreter(new frame_interpreter_dimmer(w, p, g));
}

// Dimmer100 implementation
dimm100::dimm100(QString w, bool p, int g) : device(QString("1"), w, p, g)
{
	stat.append(new device_status_dimmer100());
	setup_frame_interpreter(new frame_interpreter_lights(w, p, g));
}

// Autom implementation
autom::autom(QString w, bool p, int g) : device(QString("2"), w, p, g)
{
	stat.append(new device_status_autom());
	setup_frame_interpreter(new frame_interpreter_autom(w, p, g));
}

// Sound device implementation
sound_device::sound_device(QString w, bool p, int g) : device(QString("16"), w, p, g)
{
	stat.append(new device_status_amplifier());
	setup_frame_interpreter(new frame_interpreter_sound_device(w, p, g));
}

// Radio device implementation
radio_device::radio_device(QString w, bool p, int g) : device(QString("16"), w, p, g)
{
	stat.append(new device_status_radio());
	setup_frame_interpreter(new frame_interpreter_radio_device(w, p, g));
}

// Sound matrix device implementation
sound_matr::sound_matr(QString w, bool p, int g) : device(QString("16"), QString("1000"), p, g)
{
	stat.append(new device_status_sound_matr());
	setup_frame_interpreter(new frame_interpreter_sound_matr_device(QString("1000"), p, g));
}

// Doorphone device implementation
doorphone_device::doorphone_device(QString w, bool p, int g) : device(QString("6"), w, p, g)
{
	qDebug("doorphone_device::doorphone_device()");
	stat.append(new device_status_doorphone());
	setup_frame_interpreter(new frame_interpreter_doorphone_device(w, p, g));
}

// Imp.anti device
impanti_device::impanti_device(QString w, bool p, int g) : device(QString("5"), w, p, g)
{
	qDebug("impanti_device::impanti_device()");
	stat.append(new device_status_impanti());
	setup_frame_interpreter(new frame_interpreter_impanti_device(w, p, g));
}

// Zon.anti device
zonanti_device::zonanti_device(QString w, bool p, int g) : device(QString("5"), w, p, g)
{
	qDebug("zonanti_device::impanti_device()");
	stat.append(new device_status_zonanti());
	setup_frame_interpreter(new frame_interpreter_zonanti_device(w, p, g));
}

// Controlled temperature probe implementation
temperature_probe_controlled::temperature_probe_controlled(QString w, thermo_type_t type, bool _fancoil,
		const char *ind_centrale, const char *indirizzo, bool p, int g) :
	device(QString("4"), w, p, g)
{
	qDebug("temperature_probe_controlled::temperature_probe_controlled(), type=%d, fancoil=%s", type, _fancoil ? "true" : "false");
	stat.append(new device_status_temperature_probe_extra(type));
	stat.append(new device_status_temperature_probe());
	fancoil = _fancoil;
	simple_where = indirizzo;
	if (fancoil)
		stat.append(new device_status_fancoil());

	setup_frame_interpreter(new frame_interpreter_temperature_probe_controlled(w, type, ind_centrale, indirizzo, p, g));
}

void temperature_probe_controlled::setManual(unsigned setpoint)
{
	const unsigned dimension = 14;
	const unsigned mode = 3; // generic mode

	QString what;
	what.sprintf("#%d*%04u*%d", dimension, setpoint, mode);

	QString sharp_where = QString("#") + where;

	QString msg = QString("*#") + who + "*" + sharp_where + "*" + what + "##";
	sendCompressedFrame(msg);
}

void temperature_probe_controlled::setAutomatic()
{
	unsigned mode = 311; // generic mode
	QString sharp_where = QString("#") + where;

	QString msg = QString("*") + who + "*" + QString::number(mode) + QString("*") + sharp_where + QString("##");
	sendFrame(msg);
}

void temperature_probe_controlled::setFancoilSpeed(int speed)
{
	const unsigned dimension = 11;
	if (fancoil)
	{
		QString msg = QString("*#") + who + "*" + simple_where + "*#" + QString::number(dimension) + "*" +
			QString::number(speed) + "##";
		sendFrame(msg);
	}
}

void temperature_probe_controlled::requestFancoilStatus()
{
	const unsigned dimension = 11;
	if (fancoil)
	{
		QString msg = QString("*#") + who + "*" + simple_where + "*" + QString::number(dimension) + "##";
		sendFrame(msg);
	}
}

// Not controlled temperature probe implementation
temperature_probe_notcontrolled::temperature_probe_notcontrolled(QString w, bool external, bool p, int g) :
	device(QString("4"), w, p, g)
{
	stat.append(new device_status_temperature_probe());
	setup_frame_interpreter(new frame_interpreter_temperature_probe(w, external, p, g));
}

// modscen device
modscen_device::modscen_device(QString w, bool p, int g) : device(QString("0"), w, p, g)
{
	qDebug("modscen_device::modscen_device()");
	stat.append(new device_status_modscen());
	setup_frame_interpreter(new frame_interpreter_modscen_device(w, p, g));
}


aux_device::aux_device(QString w, bool p, int g) : device(QString("9"), w, p, g)
{
	// We set an initial value out of admitted range to force the emission of
	// status_changed signal.
	status = stat_var(stat_var::NONE, -1, 0, 1, 1);
}

void aux_device::init(bool force)
{
	OpenMsg msg = OpenMsg::createReadDim(who.toStdString(), where.toStdString());
	qDebug("aux_device::init message: %s", msg.frame_open);
	sendInit(msg.frame_open);
}

void aux_device::manageFrame(OpenMsg &msg)
{
	if (where.toInt() != msg.where())
		return;

	qDebug("aux_device::manageFrame -> frame read:%s", msg.frame_open);

	if (msg.IsNormalFrame())
	{
		int cosa = atoi(msg.Extract_cosa());
		if (status.get_val() != cosa)
		{
			status.set_val(cosa);
			emit status_changed(status);
		}
	}
}
