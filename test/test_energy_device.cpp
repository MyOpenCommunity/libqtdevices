#include "test_energy_device.h"
#include "test_device.h"

#include <energy_device.h>

#include <QVariant>
#include <QMetaType>


void TestEnergyDevice::initTestCase()
{
	// To use StatusList in signal/slots and watch them through QSignalSpy
	qRegisterMetaType<StatusList>("StatusList");

	device::setClients(0, 0, 0);
	where = "20";
	dev = new EnergyDevice(where);
}

void TestEnergyDevice::cleanupTestCase()
{
	delete dev;
}

void TestEnergyDevice::readCumulativeDay()
{
	TestDevice t(dev, 54);
	t.check(QString("*#18*%1*54*150##").arg(where), 150);
}
