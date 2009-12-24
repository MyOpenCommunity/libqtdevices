#include "test_air_conditioning_device.h"

#include <airconditioning_device.h>

void TestAirConditioningDevice::initTestCase()
{
	dev = new AirConditioningDevice("11");
}

void TestAirConditioningDevice::cleanupTestCase()
{
	delete dev;
}
