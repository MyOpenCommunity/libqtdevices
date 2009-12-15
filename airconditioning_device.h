#ifndef AIRCONDITIONING_DEVICE_H
#define AIRCONDITIONING_DEVICE_H

#include "device.h"


class AirConditioningDevice : public device
{
Q_OBJECT
public:
	AirConditioningDevice(QString where);
	void sendCommand(QString cmd) const;

	void setOffCommand(QString off_cmd);

public slots:
	void sendOff() const;

private:
	QString off;
};

#endif // AIRCONDITIONING_DEVICE_H
