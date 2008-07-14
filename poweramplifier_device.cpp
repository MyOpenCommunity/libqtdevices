#include "poweramplifier_device.h"
#include "device_status.h"
#include <openmsg.h>

#define MM_TYPE 4

enum multimedia_what_t
{
	OFF = 0,
	ON_FOLLOW_ME = 34
};

poweramplifier_device::poweramplifier_device(QString w, bool p, int g) :
	device(QString("22"), w, p, g)
{
	qDebug("poweramplifier_device::poweramplifier_device()");
	where = "3#" + where[0] + "#" + where[1];
	status[ON_OFF] = stat_var(stat_var::NONE, 0, 0, 1, 1);
	status[VOLUME] = stat_var(stat_var::NONE, 0, 0, 31, 1);
	status[PRESET] = stat_var(stat_var::NONE, 0, 0, 20, 1);
	status[TREBLE] = stat_var(stat_var::NONE, 0, -10, 10, 1);
	status[BASS] = stat_var(stat_var::NONE, 0, -10, 10, 1);
	status[BALANCE] = stat_var(stat_var::NONE, 0, -10, 10, 1);
	status[LOUD] = stat_var(stat_var::NONE, 0, 0, 1, 1);
}

void poweramplifier_device::init(bool force)
{
	OpenMsg msg = OpenMsg::createReadDim(who.ascii(), where.ascii());
	qDebug("poweramplifier_device::init message: %s", msg.frame_open);
	sendInit(msg.frame_open);
}

void poweramplifier_device::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who != msg.who() || where != msg.where())
		return;

	qDebug("poweramplifier_device::frame_rx_handler");
	qDebug("frame read:%s", frame);

	QMap<status_key_t, stat_var> st;

	// FIX: Add code here!

	if (st.size())
		emit status_changed(st);
}

void poweramplifier_device::turn_on()
{
	QString what;
	what.sprintf("%d#%d#%s", ON_FOLLOW_ME, MM_TYPE, QString(where[3]).ascii());
	OpenMsg msg = OpenMsg::createCmd(who.ascii(), where.ascii(), what.ascii());
	sendFrame(msg.frame_open);
}

void poweramplifier_device::turn_off()
{
	QString what;
	what.sprintf("%d#%d#%s", OFF, MM_TYPE, QString(where[3]).ascii());
	OpenMsg msg = OpenMsg::createCmd(who.ascii(), where.ascii(), what.ascii());
	sendFrame(msg.frame_open);
}
