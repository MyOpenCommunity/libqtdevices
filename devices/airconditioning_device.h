#ifndef AIRCONDITIONING_DEVICE_H
#define AIRCONDITIONING_DEVICE_H

#include "device.h"

class AirConditioningInterface
{
public:
	virtual void turnOff() const = 0;
	template<class T> QString commandToString(const T &info);
	virtual void activateScenario(const QString &what) const = 0;
};


class AirConditioningDevice : public device, public AirConditioningInterface
{
friend class TestAirConditioningDevice;
Q_OBJECT
public:
	AirConditioningDevice(QString where, int openserver_id = 0);
	void sendCommand(const QString &cmd) const;

	void setOffCommand(QString off_cmd);
	virtual void turnOff() const;
	virtual void activateScenario(const QString &what) const;
	template<class T> QString commandToString(const T &info) { return info; };

private:
	QString off;
};


class AdvancedAirConditioningDevice : public device, public AirConditioningInterface
{
friend class TestAdvancedAirConditioningDevice;
Q_OBJECT
public:
	AdvancedAirConditioningDevice(QString where, int openserver_id = 0);


	enum Mode
	{
		MODE_OFF = 0,
		MODE_WINTER = 1,
		MODE_SUMMER = 2,
		MODE_FAN = 3,
		MODE_DEHUM = 4,
		MODE_AUTO = 5
	};

	// Velocity and swing sometimes may not be always present, we need a value to indicate this
	enum Velocity
	{
		VEL_AUTO = 0,
		VEL_MIN = 1,
		VEL_MED = 2,
		VEL_MAX = 3,
		VEL_SILENT = 4,
		VEL_INVALID,           // none of the above values
	};

	enum Swing
	{
		SWING_OFF = 0,
		SWING_ON = 1,
		SWING_INVALID,        // none of the above values
	};


	struct AirConditionerStatus
	{
		AirConditionerStatus() : mode(MODE_OFF) { }
		AirConditionerStatus(Mode m, int t, Velocity v, Swing s) : mode(m), temp(t), vel(v), swing(s) { }
		Mode mode;
		int temp;
		Velocity vel;
		Swing swing;
	};

	void requestStatus() const;

	void setStatus(Mode mode, int temp, Velocity vel, Swing swing) const;
	void setStatus(AirConditionerStatus st) const;
	virtual void turnOff() const;
	virtual void activateScenario(const QString &what) const;
	template<class T> QString commandToString(const T &info) { return statusToString(info); };

private:
	QString statusToString(const AirConditionerStatus &st) const;
};

#endif // AIRCONDITIONING_DEVICE_H
