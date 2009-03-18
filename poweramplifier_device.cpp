#include "poweramplifier_device.h"
#include "generic_functions.h" // createRequestOpen

#include <openmsg.h>

#include <QVariant>
#include <QDebug>

#include <assert.h>

enum RequestDimension
{
	REQ_STATUS_ON = 1,
	REQ_STATUS_OFF = 0,
	REQ_VOLUME_UP = 3,
	REQ_VOLUME_DOWN = 4,
	REQ_TREBLE_UP = 40,
	REQ_TREBLE_DOWN = 41,
	REQ_BASS_UP = 36,
	REQ_BASS_DOWN = 37,
	REQ_BALANCE_UP = 42,
	REQ_BALANCE_DOWN = 43,
	REQ_NEXT_PRESET = 55,
	REQ_PREV_PRESET = 56,

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

void PowerAmplifierDevice::sendCommand(QString what) const
{
	sendFrame(createMsgOpen(who, what, where));
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
	sendCommand(QString("%1#4#%2").arg(REQ_STATUS_ON).arg(location));
}

void PowerAmplifierDevice::turnOff() const
{
	sendCommand(QString("%1#4#%2").arg(REQ_STATUS_OFF).arg(location));
}

void PowerAmplifierDevice::volumeUp() const
{
	sendCommand(QString("%1#1").arg(REQ_VOLUME_UP));
}

void PowerAmplifierDevice::volumeDown() const
{
	sendCommand(QString("%1#1").arg(REQ_VOLUME_DOWN));
}

void PowerAmplifierDevice::trebleUp() const
{
	sendCommand(QString("%1#1").arg(REQ_TREBLE_UP));
}

void PowerAmplifierDevice::trebleDown() const
{
	sendCommand(QString("%1#1").arg(REQ_TREBLE_DOWN));
}

void PowerAmplifierDevice::bassUp() const
{
	sendCommand(QString("%1#1").arg(REQ_BASS_UP));
}

void PowerAmplifierDevice::bassDown() const
{
	sendCommand(QString("%1#1").arg(REQ_BASS_DOWN));
}

void PowerAmplifierDevice::balanceUp() const
{
	sendCommand(QString("%1#1").arg(REQ_BALANCE_UP));
}

void PowerAmplifierDevice::balanceDown() const
{
	sendCommand(QString("%1#1").arg(REQ_BALANCE_DOWN));
}

void PowerAmplifierDevice::nextPreset() const
{
	sendCommand(QString::number(REQ_NEXT_PRESET));
}

void PowerAmplifierDevice::prevPreset() const
{
	sendCommand(QString::number(REQ_PREV_PRESET));
}

