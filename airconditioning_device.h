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


class AdvancedAirConditioningDevice : public device
{
Q_OBJECT
public:
	AdvancedAirConditioningDevice(QString where);


	enum Mode
	{
		MODE_OFF = 0,
		MODE_WINTER = 1,
		MODE_SUMMER = 2,
		MODE_FAN = 3,
		MODE_DEHUM = 4,
		MODE_AUTO = 5
	};

	enum Velocity
	{
		VEL_AUTO = 0,
		VEL_MIN = 1,
		VEL_MED = 2,
		VEL_MAX = 3,
		VEL_SILENT = 4
	};

	enum Swing
	{
		SWING_OFF = 0,
		SWING_ON = 1
	};


	struct AirConditionerStatus
	{
		AirConditionerStatus(Mode m, int t, Velocity v, Swing s) : mode(m), temp(t), vel(v), swing(s) { }
		Mode mode;
		int temp;
		Velocity vel;
		Swing swing;
	};

	void requestStatus() const;

	void setStatus(Mode mode, int temp, Velocity vel, Swing swing);
	void setStatus(AirConditionerStatus st);

public slots:
	//! receive a frame
	void frame_rx_handler(char *frame);
};

#endif // AIRCONDITIONING_DEVICE_H
