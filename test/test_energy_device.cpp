#include "test_energy_device.h"
#include "test_device.h"
#include "openserver_mock.h"

#include <energy_device.h>

#include <QVariant>
#include <QMetaType>


void TestEnergyDevice::initTestCase()
{
	// To use StatusList in signal/slots and watch them through QSignalSpy
	qRegisterMetaType<StatusList>("StatusList");

	server = new OpenServerMock;
	device::setClients(server->connectCommand(), server->connectMonitor(), server->connectRequest());
	where = "20";
	dev = new EnergyDevice(where);
}

void TestEnergyDevice::cleanupTestCase()
{
	delete dev;
	delete server;
}

void TestEnergyDevice::readCumulativeDay()
{
	TestDevice t(dev, EnergyDevice::DIM_CUMULATIVE_DAY);
	t.check(QString("*#18*%1*54*150##").arg(where), 150);
}

void TestEnergyDevice::readCurrent()
{
	TestDevice t(dev, EnergyDevice::DIM_CURRENT);
	t.check(QString("*#18*%1*113*74##").arg(where), 74);
}

void TestEnergyDevice::readCumulativeMonth()
{
	TestDevice t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH);
	t.check(QString("*#18*%1*52#2008#02*106##").arg(where), 106);
	t.check(QString("*#18*%1*53*95##").arg(where), 95);
}

void TestEnergyDevice::readCumulativeYear()
{
	TestDevice t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR);
	t.check(QString("*#18*%1*51*33##").arg(where), 33);
}

