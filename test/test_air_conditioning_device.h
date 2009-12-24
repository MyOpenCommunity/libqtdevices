#ifndef TESTAIRCONDITIONINGDEVICE_H
#define TESTAIRCONDITIONINGDEVICE_H

#include "test_device.h"

class AirConditioningDevice;

class TestAirConditioningDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

private:
	AirConditioningDevice *dev;
};

#endif // TESTAIRCONDITIONINGDEVICE_H
