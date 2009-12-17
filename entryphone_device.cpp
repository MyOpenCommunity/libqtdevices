#include "entryphone_device.h"

#include <openmsg.h>
#include <QDebug>

enum
{
	ANSWER = 2,
	CALL_END = 3,
	READY = 37
};

EntryphoneDevice::EntryphoneDevice(const QString &where) :
	device(QString("8"), where)
{
	// invalid values
	kind = mmtype = -1;
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

void EntryphoneDevice::endCall() const
{
	QString what = QString("%1#%2#%3").arg(CALL_END).arg(kind).arg(mmtype);
	sendCommand(what);
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
	// TODO: this is not true...this check must be done just once, when communication starts
	if (QString::fromStdString(msg.whereFull()) != where)
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
		break;
	}
	sl[what] = v;
	emit status_changed(sl);
}
