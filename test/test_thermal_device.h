#ifndef TEST_THERMAL_DEVICE_H
#define TEST_THERMAL_DEVICE_H

#include "test_device.h"

const QString THERMAL_DEVICE_WHERE = "0313";

class ThermalDevice;
class ThermalDevice4Zones;
class ThermalDevice99Zones;


class TestThermalDevice : public TestDevice
{
Q_OBJECT
private slots:
	void sendSetOff();

protected:
	void setTestDevice(ThermalDevice *d);

private:
	ThermalDevice *dev;
};


class TestThermalDevice4Zones : public TestThermalDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

private:
	ThermalDevice4Zones *dev;
};


class TestThermalDevice99Zones : public TestThermalDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

private:
	ThermalDevice99Zones *dev;
};

#endif // TEST_THERMAL_DEVICE_H
