#ifndef LIGHTINGDEVICE_H
#define LIGHTINGDEVICE_H

#include "device.h"

class LightAddress;

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
		DIM_DEVICE_OFF = 0,
		DIM_DEVICE_ON = 1,
	};

	LightingDevice(QString where, PullMode pull);

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

class LightAddress
{
friend class TestLightingDevice;
public:
	LightAddress() { }
	LightAddress(OpenMsg &msg);
	bool isReceiver(QString where);

private:
	/**
	 * Splits a point to point address into environment and light point components.
	 */
	void splitP2PAddress(const QString &elp, QString &env, QString &lp);

	QString environment;
	QString light_point;
	QString extension;
	bool is_general;
	bool is_environment;
};

#endif // LIGHTINGDEVICE_H
