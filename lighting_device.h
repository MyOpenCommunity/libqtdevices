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
	};

	LightingDevice(QString where, PullMode pull = PULL);

	void turnOn();
	void turnOn(int speed);
	void turnOff();
	void turnOff(int speed);
	// TODO: complete with "temporizzazione" frames

	void requestStatus();
	// TODO: complete with "temporizzazione variabile" request

	virtual void manageFrame(OpenMsg &msg);

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

	virtual void manageFrame(OpenMsg &msg);
};

#endif // LIGHTINGDEVICE_H
