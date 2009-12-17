#include "airconditioning_device.h"
#include "generic_functions.h" // createWriteRequestOpen

#include <openmsg.h>


#define ADVANCED_SPLIT_DIM 22


AirConditioningDevice::AirConditioningDevice(QString where) : device("0", where)
{
}

void AirConditioningDevice::sendCommand(QString cmd) const
{
	sendFrame(QString("*%1*%2*%3##").arg(who).arg(cmd).arg(where));
}

void AirConditioningDevice::sendOff() const
{
	Q_ASSERT_X(!off.isNull(), "AirConditioningDevice::sendOff", "Off command not set!");
	sendCommand(off);
}

void AirConditioningDevice::setOffCommand(QString off_cmd)
{
	off = off_cmd;
}



AdvancedAirConditioningDevice::AdvancedAirConditioningDevice(QString where) : device("4", where)
{
}

void AdvancedAirConditioningDevice::requestStatus() const
{
	sendRequest(QString::number(ADVANCED_SPLIT_DIM));
}

void AdvancedAirConditioningDevice::setStatus(Mode mode, int temp, Velocity vel, Swing swing)
{
	QString what;

	switch (mode)
	{
	case MODE_OFF:
		what = QString("%1*%2***").arg(ADVANCED_SPLIT_DIM).arg(mode);
		break;

	case MODE_FAN:
	case MODE_DEHUM:
		what = QString("%1*%2**%3*%4").arg(ADVANCED_SPLIT_DIM).arg(mode).arg(vel).arg(swing);
		break;

	default:
		what = QString("%1*%2*%3*%4*%5").arg(ADVANCED_SPLIT_DIM).arg(mode).arg(temp).arg(vel).arg(swing);
		break;
	}

	sendFrame(createWriteRequestOpen(who, what, where));
}

void AdvancedAirConditioningDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who() || msg.where() != where.toInt())
		return;
}
