#ifndef AIRCONDITIONING_DEVICE_H
#define AIRCONDITIONING_DEVICE_H

#include "device.h"


class AirConditioningDevice : public device
{
Q_OBJECT
public:
	AirConditioningDevice(QString where);
	void sendCommand(QString cmd);
};

#endif // AIRCONDITIONING_DEVICE_H
