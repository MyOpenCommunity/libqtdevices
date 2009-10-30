#include "test_thermal_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "thermal_device.h"

#include <openmsg.h>

#include <QtTest/QtTest>


// TestThermalDevice implementation]

void TestThermalDevice::setTestDevice(ThermalDevice *d)
{
	dev = d;
}

void TestThermalDevice::sendSetOff()
{
	dev->setOff();
	client_command->flush();
	QString cmd = "*" + ThermalDevice::WHO + "*303*#" + THERMAL_DEVICE_WHERE + "##";
	QCOMPARE(server->frameCommand(), cmd);
}


// TestThermalDevice4Zones implementation]

void TestThermalDevice4Zones::initTestCase()
{
	dev = new ThermalDevice4Zones(THERMAL_DEVICE_WHERE);
	setTestDevice(dev);
}

void TestThermalDevice4Zones::cleanupTestCase()
{
	delete dev;
	dev = NULL;
	setTestDevice(NULL);
}


// TestThermalDevice99Zones implementation]

void TestThermalDevice99Zones::initTestCase()
{
	dev = new ThermalDevice99Zones(THERMAL_DEVICE_WHERE);
	setTestDevice(dev);
}

void TestThermalDevice99Zones::cleanupTestCase()
{
	delete dev;
	dev = NULL;
	setTestDevice(NULL);
}
