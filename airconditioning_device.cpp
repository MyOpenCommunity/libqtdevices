#include "airconditioning_device.h"


AirConditioningDevice::AirConditioningDevice(QString where) : device("0", where)
{
}

void AirConditioningDevice::sendCommand(QString cmd)
{
	sendFrame(QString("*%1*%2*%3##").arg(who).arg(cmd).arg(where));
}

