#ifndef LIGHTINGDEVICE_H
#define LIGHTINGDEVICE_H

#include "device.h"

class LightingDevice : public device
{
friend class TestLightingDevice;
Q_OBJECT
public:
	enum PullMode
	{
		PULL,
		NOT_PULL,
		PULL_UNKNOWN,
	};

	enum Type
	{
		DIM_DEVICE_ON = 1,
		DIM_VARIABLE_TIMING = 2,
		DIM_DIMMER_PROBLEM = 19,
		DIM_DIMMER_LEVEL,
	};

	LightingDevice(QString where, PullMode pull = PULL);

	void turnOn();
	void turnOn(int speed);
	void turnOff();
	void turnOff(int speed);
	void fixedTiming(int value);
	// TODO: using a QTime we are limited to 24h, 60min, 60 secs. Is it ok?
	void variableTiming(QTime t);

	void requestStatus();
	void requestVariableTiming();

	virtual void manageFrame(OpenMsg &msg);

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);

private:
	PullMode mode;
};

class Dimmer : public LightingDevice
{
Q_OBJECT
public:
	Dimmer(QString where, PullMode pull = PULL);

	// TODO: is this needed? There's no graphical interface to set the level...
	void setLevel(int level);
	void increaseLevel();
	void decreaseLevel();

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
};

#endif // LIGHTINGDEVICE_H
