#ifndef AIRCONDITIONING_DEVICE_H
#define AIRCONDITIONING_DEVICE_H

#include "device.h"


class AirConditioningDevice : public device
{
Q_OBJECT
public:
	AirConditioningDevice(QString where, QString off_cmd);
	void sendCommand(QString cmd) const;

public slots:
	void sendOff() const;

private:
	QString off;
};

#endif // AIRCONDITIONING_DEVICE_H
