#include "lighting_device.h"

enum
{
	LIGHT_ON = 1,
	LIGHT_OFF = 0,
};

LightingDevice::LightingDevice(QString where, PullMode pull) :
	device(QString("1"), where)
{
	mode = pull;
}

void LightingDevice::turnOn()
{
	sendCommand(QString::number(LIGHT_ON));
}

void LightingDevice::turnOn(int speed)
{
	sendCommand(QString::number(LIGHT_ON) + "#" + QString::number(speed));
}

void LightingDevice::turnOff()
{
	sendCommand(QString::number(LIGHT_OFF));
}

void LightingDevice::turnOff(int speed)
{
	sendCommand(QString::number(LIGHT_OFF) + "#" + QString::number(speed));
}

void LightingDevice::requestStatus()
{
	sendRequest(QString());
}
