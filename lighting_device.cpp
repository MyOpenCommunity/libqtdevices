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
	DIMMER100_STATUS = 1,
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
	int v = FIXED_TIMING_MIN + value;
	if (v >= FIXED_TIMING_MIN && v <= FIXED_TIMING_MAX)
		sendCommand(QString::number(v));
}

// Limitations:
// 0 <= h <= 255
// 0 <= m <= 59
// 0 <= s <= 59
void LightingDevice::variableTiming(int h, int m, int s)
{
	if ((h >= 0 && h <= 255) && (m >= 0 && m <= 59) && (s >= 0 && s <= 59))
		sendFrame(createWriteRequestOpen(who, QString("%1*%2*%3*%4").arg(DIM_VARIABLE_TIMING)
			.arg(h).arg(m).arg(s), where));
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

	switch (checkAddressIsForMe(QString::fromStdString(msg.whereFull()), where, mode))
	{
	case NOT_MINE:
		return;
	case GLOBAL:
	case ENVIRONMENT:
		is_multi_receiver_frame = true;
		break;
	default:
		break;
	}

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
	case DIM_VARIABLE_TIMING:
	{
		QList<int> l;
		for (unsigned i = 0; i < msg.whatArgCnt(); ++i)
			l << msg.whatArgN(i);
		v.setValue(l);
	}
		break;
	default:
		qDebug() << "LightingDevice::manageFrame(): Unknown what";
	}

	if (status_index > 0)
		(*sl)[status_index] = v;
	else
		(*sl)[what] = v;
}


DimmerDevice::DimmerDevice(QString where, PullMode pull) :
	LightingDevice(where, pull)
{
}

void DimmerDevice::setLevel(int level)
{
	// TODO: is this method needed?
}

void DimmerDevice::increaseLevel()
{
	sendCommand(QString::number(DIMMER_INC));
}

void DimmerDevice::decreaseLevel()
{
	sendCommand(QString::number(DIMMER_DEC));
}

void DimmerDevice::parseFrame(OpenMsg &msg, StatusList *sl)
{
	LightingDevice::parseFrame(msg, sl);

	QVariant v;
	int what = msg.what();
	int status_index = -1;

	if (what >= DIMMER10_LEVEL_MIN && what <= DIMMER10_LEVEL_MAX)
	{
		v.setValue(getDimmerLevel(what));
		status_index = DIM_DIMMER_LEVEL;
	}
	else if (what == DIM_DIMMER_PROBLEM)
		v.setValue(true);

	if (status_index > 0)
		(*sl)[status_index] = v;
	else
		(*sl)[what] = v;
}

int DimmerDevice::getDimmerLevel(int what)
{
	Q_ASSERT_X(what >= 2 && what <= 10, "DimmerDevice::getDimmerLevel",
		"DimmerDevice level must be between 2 and 10");
	return what * 10;
}


Dimmer100Device::Dimmer100Device(QString where, PullMode pull) :
	DimmerDevice(where, pull)
{
}

void Dimmer100Device::increaseLevel100(int delta, int speed)
{
	sendCommand(QString("%1#%2#%3").arg(DIMMER_INC).arg(delta).arg(speed));
}

void Dimmer100Device::decreaseLevel100(int delta, int speed)
{
	sendCommand(QString("%1#%2#%3").arg(DIMMER_DEC).arg(delta).arg(speed));
}

void Dimmer100Device::requestDimmer100Status()
{
	sendRequest(QString::number(DIMMER100_STATUS));
}

void Dimmer100Device::parseFrame(OpenMsg &msg, StatusList *sl)
{
	DimmerDevice::parseFrame(msg, sl);

	QVariant v;
	int what = msg.what();

	if (what == DIMMER100_STATUS)
	{
		QList<int> l;
		for (unsigned i = 0; i < msg.whatArgCnt(); ++i)
			l << msg.whatArgN(i);
		v.setValue(l);
	}
	(*sl)[DIM_DIMMER100_STATUS] = v;
}

int Dimmer100Device::getDimmerLevel(int what)
{
	switch (what)
	{
	case 2:
		return 1;
	case 9:
		return 75;
	default:
		Q_ASSERT_X((what >= 3 && what <= 8) || what == 10, "Dimmer100Device::getDimmerLevel",
			"Dimmer level must be between 2 and 10");
		return (what - 2) * 10;
	}
}
