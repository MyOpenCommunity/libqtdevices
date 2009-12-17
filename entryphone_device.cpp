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
	READY = 37
};

EntryphoneDevice::EntryphoneDevice(const QString &where) :
	device(QString("8"), where)
{
	// invalid values
	kind = mmtype = -1;
	is_calling = false;
	initVctProcess();
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

void EntryphoneDevice::autoSwitching() const
{
}

void EntryphoneDevice::cycleCamera() const
{
}

void EntryphoneDevice::manageFrame(OpenMsg &msg)
{
	if ((!is_calling) && (QString::fromStdString(msg.whereFull()) != where))
		return;

	int what = msg.what();
	StatusList sl;
	QVariant v;
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
	case END_OF_CALL:
		resetCallState();
		break;
	}
	sl[what] = v;
	emit status_changed(sl);
}

void EntryphoneDevice::resetCallState()
{
	is_calling = false;
	caller_address = "";

	// TODO: this must be factored out to a separate method
	StatusList sl;
	sl[END_OF_CALL] = QVariant();
	emit status_changed(sl);
}
