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

	LightingDevice(QString where, PullMode pull);

	void turnOn();
	void turnOn(int speed);
	void turnOff();
	void turnOff(int speed);
	// TODO: complete with "temporizzazione" frames

	void requestStatus();
	// TODO: complete with "temporizzazione variabile" request

	// TODO: complete with manageFrame()
private:
	PullMode mode;
};

#endif // LIGHTINGDEVICE_H
