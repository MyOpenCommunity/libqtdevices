#include "test_energy_device.h"
#include "device_tester.h"

#include <energy_device.h>

#include <QVariant>


void TestEnergyDevice::initTestCase()
{
	where = "20";
	dev = new EnergyDevice(where);
}

void TestEnergyDevice::cleanupTestCase()
{
	delete dev;
}

void TestEnergyDevice::readCumulativeDay()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_DAY);
	t.check(QString("*#18*%1*54*150##").arg(where), 150);
}

void TestEnergyDevice::readCurrent()
{
	DeviceTester t(dev, EnergyDevice::DIM_CURRENT);
	t.check(QString("*#18*%1*113*74##").arg(where), 74);
}

void TestEnergyDevice::readCumulativeMonth()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH);
	t.check(QString("*#18*%1*52#2008#02*106##").arg(where), 106);
	t.check(QString("*#18*%1*53*95##").arg(where), 95);
}

void TestEnergyDevice::readCumulativeYear()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR);
	t.check(QString("*#18*%1*51*33##").arg(where), 33);
}

