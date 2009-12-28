#include "entryphone_device.h"
#include "generic_functions.h"

#include <openmsg.h>
#include <QDebug>

// this value must be sent before the address of the unit that terminates the call
// otherwise CALL_END frame is not valid.
static const char *END_ALL_CALLS = "4";

enum
{
	ANSWER = 2,
	AUTOSWITCHING = 4,
	CALLER_ADDRESS = 9,
	READY = 37,
};

EntryphoneDevice::EntryphoneDevice(const QString &where) :
	device(QString("8"), where)
{
	// invalid values
	kind = mmtype = -1;
	is_calling = false;
}

void EntryphoneDevice::answerCall() const
{
	// TODO: this can be removed once the code is tested
	if (kind == -1 || mmtype == -1)
	{
		qWarning() << "Kind or mmtype are invalid, there's no active call";
		return;
	}
	QString what = QString("%1#%2#%3").arg(ANSWER).arg(kind).arg(mmtype);
	sendCommand(what);
}

void EntryphoneDevice::cameraOn(QString _where)
{
	QString what = QString("%1#%2").arg(AUTOSWITCHING).arg(where);
	sendCommand(what, _where);
}

void EntryphoneDevice::endCall()
{
	QString what = QString("%1#%2#%3").arg(END_OF_CALL).arg(kind).arg(mmtype);
	sendFrame(createMsgOpen(who, what, QString(END_ALL_CALLS) + where));
	resetCallState();
}

void EntryphoneDevice::initVctProcess()
{
	// TODO: enumerate the values for type, which can be: scs only, ip only, both scs and ip
	// TODO: find out values
	int type = 1;
	QString what = QString("%1#%2").arg(READY).arg(type);
	sendCommand(what);
}

void EntryphoneDevice::manageFrame(OpenMsg &msg)
{
	if ((!is_calling) && (QString::fromStdString(msg.whereFull()) != where))
		return;

	int what = msg.what();
	StatusList sl;
	QVariant v;
	bool send_status_update = true;
	switch (what)
	{
	case INCOMING_CALL:
		Q_ASSERT_X(msg.whatSubArgCnt() < 2, "EntryphoneDevice::manageFrame",
			"Incomplete open frame received");
		kind = msg.whatArgN(0);
		mmtype = msg.whatArgN(1);
		// we can safely ignore caller address, we will receive a frame later.
		v.setValue(true);
		is_calling = true;
		break;
	case CALLER_ADDRESS:
		caller_address = QString::fromStdString(msg.whereFull());
		send_status_update = false;
		break;
	case END_OF_CALL:
		resetCallState();
		break;
	}

	if (send_status_update)
	{
		sl[what] = v;
		emit status_changed(sl);
	}
}

void EntryphoneDevice::resetCallState()
{
	is_calling = false;
	caller_address = "";
	kind = -1;
	mmtype = -1;
}
