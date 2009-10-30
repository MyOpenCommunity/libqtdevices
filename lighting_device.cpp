#include "lighting_device.h"
#include "openmsg.h"
#include "generic_functions.h"
#include "bttime.h" // BtTimeSeconds

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
	PullDevice(QString("1"), where, pull)
{
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


void LightingDevice::requestPullStatus()
{
	requestStatus();
}



void LightingDevice::parseFrame(OpenMsg &msg, StatusList *sl)
{
	QVariant v;
	int what = msg.what();
	// set the value only if status_index has been modified, to avoid overwriting
	// previously set values which are not considered in this function
	int status_index = -1;

	switch (what)
	{
	case DIM_DEVICE_ON:
	case DIM_DEVICE_OFF:
		v.setValue(what == DIM_DEVICE_ON);
		status_index = DIM_DEVICE_ON;
		break;
	case DIM_VARIABLE_TIMING:
		if (msg.IsMeasureFrame())
		{
			Q_ASSERT_X(msg.whatArgCnt() == 3, "LightingDevice::parseFrame",
				"Variable timing message has more than 3 what args");
			BtTimeSeconds t(msg.whatArgN(0), msg.whatArgN(1), msg.whatArgN(2));
			v.setValue(t);
			status_index = what;
		}
		break;
	}
	// handle fixed timing status changing. This is really useful for environment frames only, since
	// for point-to-point frames we also get a ON/OFF status update directly from the device.
	// It won't hurt handling twice the state (for point-to-point frames).
	if (what >= FIXED_TIMING_MIN && what <= FIXED_TIMING_MAX)
	{
		v.setValue(true);
		status_index = DIM_DEVICE_ON;
	}

	if (status_index > 0)
		(*sl)[status_index] = v;
}


DimmerDevice::DimmerDevice(QString where, PullMode pull) :
	LightingDevice(where, pull)
{
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

	if (msg.IsNormalFrame())
	{
		QVariant v;
		int what = msg.what();
		int status_index = -1;

		if (what >= DIMMER10_LEVEL_MIN && what <= DIMMER10_LEVEL_MAX)
		{
			v.setValue(getDimmerLevel(what));
			status_index = DIM_DIMMER_LEVEL;
		}
		else if (what == DIM_DIMMER_PROBLEM)
		{
			v.setValue(true);
			status_index = what;
		}

		if (status_index > 0)
			(*sl)[status_index] = v;
	}
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

void Dimmer100Device::requestPullStatus()
{
	requestDimmer100Status();
}

void Dimmer100Device::parseFrame(OpenMsg &msg, StatusList *sl)
{
	DimmerDevice::parseFrame(msg, sl);

	int what = msg.what();

	if (what == DIMMER100_STATUS && msg.IsMeasureFrame())
	{
		QVariant v;

		Q_ASSERT_X(msg.whatArgCnt() == 2, "Dimmer100Device::parseFrame",
			"Dimmer 100 status frame must have 2 what args");
		// convert the value in 0-100 range
		v.setValue(msg.whatArgN(0) - 100);
		(*sl)[DIM_DIMMER100_LEVEL] = v;

		v.setValue(msg.whatArgN(1));
		(*sl)[DIM_DIMMER100_SPEED] = v;
	}
}

int Dimmer100Device::getDimmerLevel(int what)
{
	switch (what)
	{
	case 2:
		return 1;
	case 9:
		return 75;
	case 10:
		return 100;
	default:
		Q_ASSERT_X((what >= 3 && what <= 8), "Dimmer100Device::getDimmerLevel",
			"Dimmer level must be between 2 and 10");
		return (what - 2) * 10;
	}
}
