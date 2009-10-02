#include "lighting_device.h"
#include "openmsg.h"
#include "generic_functions.h"

#include <QDebug>

enum
{
	LIGHT_ON = 1,
	LIGHT_OFF = 0,
	FIXED_TIMING_MIN = 11,
	FIXED_TIMING_MAX = 18,
	DIMMER10_LEVEL_MIN = 2,
	DIMMER10_LEVEL_MAX = 10,
	DIMMER_INC = 30,
	DIMMER_DEC = 31,
};

enum
{
	DIM_DEVICE_OFF = 0,
};

LightingDevice::LightingDevice(QString where, PullMode pull) :
	device(QString("1"), where)
{
	mode = pull;
}

void LightingDevice::turnOn()
{
	sendCommand(QString::number(LIGHT_ON));
}

void LightingDevice::turnOn(int speed)
{
	sendCommand(QString("%1#%2").arg(LIGHT_ON).arg(speed));
}

void LightingDevice::turnOff()
{
	sendCommand(QString::number(LIGHT_OFF));
}

void LightingDevice::turnOff(int speed)
{
	sendCommand(QString("%1#%2").arg(LIGHT_OFF).arg(speed));
}

void LightingDevice::fixedTiming(int value)
{
	if (value >= FIXED_TIMING_MIN && value <= FIXED_TIMING_MAX)
		sendCommand(QString::number(value));
}

void LightingDevice::variableTiming(QTime t)
{
	sendFrame(createWriteRequestOpen(who, QString("%1*%2*%3*%4").arg(DIM_VARIABLE_TIMING)
		.arg(t.hour()).arg(t.minute()).arg(t.second()), where));
}

void LightingDevice::requestStatus()
{
	sendRequest(QString());
}

void LightingDevice::requestVariableTiming()
{
	sendRequest(QString::number(DIM_VARIABLE_TIMING));
}

void LightingDevice::manageFrame(OpenMsg &msg)
{
	// true if the frame is general or environment (not group).
	bool is_multi_receiver_frame = false;

	bool is_our = (QString::fromStdString(msg.whereFull()) == where);
	if (!is_our && mode != PULL)
	{
		// here we check if address is general or environment
		is_our = checkAddressIsForMe(msg.Extract_dove(), where);
		is_multi_receiver_frame = is_our;
	}
	if (!is_our)
		return;

	StatusList sl;
	parseFrame(msg, &sl);

	// when mode is unknown and the frame is for multiple receivers (ie it's a general or
	// environment frame), we must send a status request to the device before sending
	// a status_changed()
	if (mode == PULL_UNKNOWN && is_multi_receiver_frame)
		// TODO: optimize this scenario
		requestStatus();
	else
		emit status_changed(sl);
}

void LightingDevice::parseFrame(OpenMsg &msg, StatusList *sl)
{
	QVariant v;
	int what = msg.what();
	int status_index = -1;

	switch (what)
	{
	case DIM_DEVICE_ON:
	case DIM_DEVICE_OFF:
		v.setValue(what == DIM_DEVICE_ON ? true : false);
		status_index = DIM_DEVICE_ON;
		break;
	// TODO: add "Temporizzazioni variabili" status change
	default:
		qDebug() << "LightingDevice::manageFrame(): Unknown what";
	}

	if (status_index > 0)
		(*sl)[status_index] = v;
	else
		(*sl)[what] = v;
}


Dimmer::Dimmer(QString where, PullMode pull) :
	LightingDevice(where, pull)
{
}

void Dimmer::setLevel(int level)
{
	// TODO: is this method needed?
}

void Dimmer::increaseLevel()
{
	sendCommand(QString::number(DIMMER_INC));
}

void Dimmer::decreaseLevel()
{
	sendCommand(QString::number(DIMMER_DEC));
}

void Dimmer::parseFrame(OpenMsg &msg, StatusList *sl)
{
	LightingDevice::parseFrame(msg, sl);

	QVariant v;
	int what = msg.what();
	int status_index = -1;

	if (what >= DIMMER10_LEVEL_MIN && what <= DIMMER10_LEVEL_MAX)
	{
		v.setValue(what);
		status_index = DIM_DIMMER_LEVEL;
	}
	else if (what == DIM_DIMMER_PROBLEM)
		v.setValue(true);

	if (status_index > 0)
		(*sl)[status_index] = v;
	else
		(*sl)[what] = v;
}
