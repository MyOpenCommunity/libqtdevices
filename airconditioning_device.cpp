#include "airconditioning_device.h"


AirConditioningDevice::AirConditioningDevice(QString where, QString off_cmd) : device("0", where)
{
	off = off_cmd;
}

void AirConditioningDevice::sendCommand(QString cmd) const
{
	sendFrame(QString("*%1*%2*%3##").arg(who).arg(cmd).arg(where));
}

void AirConditioningDevice::sendOff() const
{
	sendCommand(off);
}

