#include "entryphone_device.h"
#include "generic_functions.h"

#include <openmsg.h>
#include <QDebug>

// this value must be sent before the address of the unit that terminates the call
// otherwise CALL_END frame is not valid.
static const char *END_ALL_CALLS = "4";

enum
{
	CALL = 1,
	ANSWER = 2,
	AUTOSWITCHING = 4,
	CYCLE_EXT_UNIT = 6,
	CALLER_ADDRESS = 9,
	OPEN_LOCK = 19,
	RELEASE_LOCK = 20,
	STAIRCASE_ACTIVATE = 21,
	STAIRCASE_RELEASE = 22,
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

void EntryphoneDevice::internalIntercomCall(QString _where)
{
	kind = 6;
	mmtype = 2;

	QString what = QString("%1#%2#%3#%4").arg(CALL).arg(kind).arg(mmtype).arg(where);
	sendCommand(what, _where);
}

void EntryphoneDevice::externalIntercomCall(QString _where)
{
	kind = 6;
	mmtype = 2;

	QString what = QString("%1#%2#%3#%4").arg(CALL).arg(kind).arg(mmtype).arg(where);
	sendCommand(what, _where);
}

void EntryphoneDevice::cameraOn(QString _where) const
{
	QString what = QString("%1#%2").arg(AUTOSWITCHING).arg(where);
	sendCommand(what, _where);
}

void EntryphoneDevice::stairLightActivate() const
{
	sendCommand(QString::number(STAIRCASE_ACTIVATE));
}

void EntryphoneDevice::stairLightRelease() const
{
	sendCommand(QString::number(STAIRCASE_RELEASE));
}

void EntryphoneDevice::openLock() const
{
	sendCommand(QString::number(OPEN_LOCK), caller_address);
}

void EntryphoneDevice::releaseLock() const
{
	sendCommand(QString::number(RELEASE_LOCK), caller_address);
}

void EntryphoneDevice::cycleExternalUnits() const
{
	sendCommand(QString("%1#%2").arg(CYCLE_EXT_UNIT).arg(where), caller_address);
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
	case CALL:
		Q_ASSERT_X(msg.whatSubArgCnt() < 2, "EntryphoneDevice::manageFrame",
			"Incomplete open frame received");
		kind = msg.whatArgN(0);
		mmtype = msg.whatArgN(1);

		if (kind >= 1 && kind <= 4)
			what = VCT_CALL;
		// TODO: serve differenziare perche' nel caso di kind == 5 (autochiamata)
		// non bisogna far suonare una suoneria.. al contrario della vct call
		// normale.. aggiungere una grandezza che indica il numero della suoneria
		// per le vct!
		else if (kind == 5)
			what = VCT_CALL;
		else
			what = INTERCOM_CALL;

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
