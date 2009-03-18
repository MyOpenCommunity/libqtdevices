#include "poweramplifier_device.h"
#include "generic_functions.h" // createRequestOpen

#include <openmsg.h>

#include <QVariant>
#include <QDebug>

#include <assert.h>

enum RequestDimension
{
	REQ_STATUS = 4,
};


PowerAmplifierDevice::PowerAmplifierDevice(QString address) :
	device(QString("22"), "3#" + address[0] + "#" + address[1])
{
	location = QString(address[0]).toInt();
}

void PowerAmplifierDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who() || msg.whereArgCnt() != 2)
		return;

	QString msg_where = QString("%1#%2#%3").arg(msg.where()).arg(msg.whereArg(0).c_str()).arg(msg.whereArg(1).c_str());

	if (where != msg_where)
		return;

	StatusList status_list;
	QVariant v;

	assert(msg.whatArgCnt() > 0);
	qDebug("PowerAmplifierDevice::frame_rx_handler -> frame read:%s", frame);

	int what = msg.what();

	if (what == DIM_STATUS || what == DIM_VOLUME || what == DIM_LOUD)
	{
		if (what == DIM_STATUS || what == DIM_LOUD)
		{
			bool st = msg.whatArgN(0) == 1;
			v.setValue(st);
		}
		else
			v.setValue(msg.whatArgN(0));
		status_list[what] = v;
		emit status_changed(status_list);
	}
}

void PowerAmplifierDevice::sendRequest(int what) const
{
	sendInit(createRequestOpen(who, QString::number(what), where));
}

void PowerAmplifierDevice::requestStatus() const
{
	sendRequest(DIM_STATUS);
}

void PowerAmplifierDevice::requestVolume() const
{
	sendRequest(DIM_VOLUME);
}

void PowerAmplifierDevice::requestTreble() const
{
	sendRequest(DIM_TREBLE);
}

void PowerAmplifierDevice::requestBass() const
{
	sendRequest(DIM_BASS);
}

void PowerAmplifierDevice::requestBalance() const
{
	sendRequest(DIM_BALANCE);
}

void PowerAmplifierDevice::requestPreset() const
{
	sendRequest(DIM_PRESET);
}

void PowerAmplifierDevice::requestLoud() const
{
	sendRequest(DIM_LOUD);
}

void PowerAmplifierDevice::turnOn() const
{
	sendFrame(createMsgOpen(who, QString("1#%1#%2").arg(REQ_STATUS).arg(location), where));
}

void PowerAmplifierDevice::turnOff() const
{
	sendFrame(createMsgOpen(who, QString("0#%1#%2").arg(REQ_STATUS).arg(location), where));
}

