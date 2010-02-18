#include "deviceold.h"
#include "frame_interpreter.h"

#include <openmsg.h>
#include <stdlib.h> // atoi

#include <QStringList>



DeviceOld::DeviceOld(QString who, QString where) : device(who, where), interpreter(0)
{
}

void DeviceOld::init(bool force)
{
	QList<device_status*> dsl;
	for (int i = 0; i < stat.size(); ++i)
	{
		device_status *ds = stat.at(i);

		QStringList msgl;
		msgl.clear();
		if (force)
		{
			Q_ASSERT_X(interpreter, "DeviceOld::init", "interpreter not set!");
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
				Q_ASSERT_X(interpreter, "DeviceOld::init", "interpreter not set!");
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

void DeviceOld::init_requested_handler(QString msg)
{
	qDebug("DeviceOld::init_requested_handler()");
	if (msg != "")
		sendFrame(msg);
}

DeviceOld::~DeviceOld()
{
	while (!stat.isEmpty())
		delete stat.takeFirst();
}

// TODO: this method is just for compatibility, remove it when all the device
// will support the new "manageFrame interface".
void DeviceOld::manageFrame(OpenMsg &msg)
{
	frame_rx_handler(msg.frame_open);
}

void DeviceOld::frame_rx_handler(char *s)
{
	qDebug("device::frame_rx_handler");
	emit handle_frame(s, stat);
}

void DeviceOld::setup_frame_interpreter(frame_interpreter* i)
{
	interpreter = i;

	connect(this, SIGNAL(handle_frame(char *, QList<device_status*>)),
			i, SLOT(handle_frame_handler(char *, QList<device_status*>)));

	connect(i, SIGNAL(frame_event(QList<device_status*>)),
			SIGNAL(status_changed(QList<device_status*>)));

	connect(i, SIGNAL(init_requested(QString)), SLOT(init_requested_handler(QString)));
}


// MCI implementation
mci_device::mci_device(QString w, bool p, int g) : DeviceOld(QString("18"), w)
{
	stat.append(new device_status_mci());
	setup_frame_interpreter(new frame_interpreter_mci(w, p, g));
}

// Sound device implementation
sound_device::sound_device(QString w, bool p, int g) : DeviceOld(QString("16"), w)
{
	stat.append(new device_status_amplifier());
	setup_frame_interpreter(new frame_interpreter_sound_device(w, p, g));
}

// Radio device implementation
radio_device::radio_device(QString w, bool p, int g) : DeviceOld(QString("16"), w)
{
	stat.append(new device_status_radio());
	setup_frame_interpreter(new frame_interpreter_radio_device(w, p, g));
}

QString radio_device::get_key()
{
	// Radio devices are different, we build the key based only on
	// the least significant digit
	int wh = where.toInt();
	while (wh >= 100)
		wh -= 100;
	while (wh >= 10)
		wh -= 10;

	return QString("%1#%2*%3").arg(openserver_id).arg(who).arg(wh);
}

// Sound matrix device implementation
sound_matr::sound_matr(QString w, bool p, int g) : DeviceOld(QString("16"), QString("1000"))
{
	stat.append(new device_status_sound_matr());
	setup_frame_interpreter(new frame_interpreter_sound_matr_device(QString("1000"), p, g));
}

// Doorphone device implementation
doorphone_device::doorphone_device(QString w, bool p, int g) : DeviceOld(QString("6"), w)
{
	qDebug("doorphone_device::doorphone_device()");
	stat.append(new device_status_doorphone());
	setup_frame_interpreter(new frame_interpreter_doorphone_device(w, p, g));
}

// Imp.anti device
impanti_device::impanti_device(QString w, bool p, int g) : DeviceOld(QString("5"), w)
{
	qDebug("impanti_device::impanti_device()");
	stat.append(new device_status_impanti());
	setup_frame_interpreter(new frame_interpreter_impanti_device(w, p, g));
}

// Zon.anti device
zonanti_device::zonanti_device(QString w, bool p, int g) : DeviceOld(QString("5"), w)
{
	qDebug("zonanti_device::impanti_device()");
	stat.append(new device_status_zonanti());
	setup_frame_interpreter(new frame_interpreter_zonanti_device(w, p, g));
}

aux_device::aux_device(QString w, bool p, int g) : DeviceOld(QString("9"), w)
{
	// We set an initial value out of admitted range to force the emission of
	// status_changed signal.
	status = stat_var(stat_var::NONE, -1, 0, 1, 1);
}

void aux_device::init()
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

