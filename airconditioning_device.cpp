#include "airconditioning_device.h"


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

