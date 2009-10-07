#ifndef LIGHTINGDEVICE_H
#define LIGHTINGDEVICE_H

#include "device.h"
#include "generic_functions.h"

class LightingDevice : public device
{
friend class TestLightingDevice;
Q_OBJECT
public:
	enum Type
	{
		DIM_DEVICE_ON = 1,
		DIM_VARIABLE_TIMING = 2,
		DIM_DIMMER_PROBLEM = 19,
		DIM_DIMMER_LEVEL,
		DIM_DIMMER100_STATUS,
	};

	LightingDevice(QString where, PullMode pull = PULL);

	void turnOn();
	void turnOn(int speed);
	void turnOff();
	void turnOff(int speed);
	void fixedTiming(int value);
	void variableTiming(int h, int m, int s);

	void requestStatus();
	void requestVariableTiming();

	virtual void manageFrame(OpenMsg &msg);

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);

private:
	PullMode mode;
};

// Type returned in StatusList for DIM_VARIABLE_TIMING
Q_DECLARE_METATYPE(QList<int>);

class Dimmer : public LightingDevice
{
friend class TestDimmer;
Q_OBJECT
public:
	Dimmer(QString where, PullMode pull = PULL);

	// TODO: is this needed? There's no graphical interface to set the level...
	void setLevel(int level);
	void increaseLevel();
	void decreaseLevel();

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
	virtual int getDimmerLevel(int what);
};


class Dimmer100 : public Dimmer
{
friend class TestDimmer100;
Q_OBJECT
public:
	Dimmer100(QString where, PullMode pull = PULL);

	void increaseLevel100(int delta, int speed);
	void decreaseLevel100(int delta, int speed);

	void requestDimmer100Status();

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
	virtual int getDimmerLevel(int what);
};
#endif // LIGHTINGDEVICE_H
