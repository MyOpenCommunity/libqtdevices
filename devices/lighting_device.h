#ifndef LIGHTINGDEVICE_H
#define LIGHTINGDEVICE_H

#include "pulldevice.h"

// Convert a dimmer10 level into range 0-100 with the formula given on the specs
int dimmerLevelTo100(int level);

class LightingDevice : public PullDevice
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
		DIM_DIMMER100_LEVEL,
		DIM_DIMMER100_SPEED,
	};

	LightingDevice(QString where, PullMode pull = PULL_UNKNOWN);

	void turnOn();
	void turnOn(int speed);
	void turnOff();
	void turnOff(int speed);
	void fixedTiming(int value);
	void variableTiming(int h, int m, int s);

	void requestStatus();
	void requestVariableTiming();

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
	virtual void requestPullStatus();
};


class DimmerDevice : public LightingDevice
{
friend class TestDimmer;
Q_OBJECT
public:
	DimmerDevice(QString where, PullMode pull = PULL_UNKNOWN);

	void increaseLevel();
	void decreaseLevel();

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
};


class Dimmer100Device : public DimmerDevice
{
friend class TestDimmer100;
Q_OBJECT
public:
	Dimmer100Device(QString where, PullMode pull = PULL_UNKNOWN);

	void increaseLevel100(int delta, int speed);
	void decreaseLevel100(int delta, int speed);

	void requestDimmer100Status();

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
	virtual void requestPullStatus();
};
#endif // LIGHTINGDEVICE_H
