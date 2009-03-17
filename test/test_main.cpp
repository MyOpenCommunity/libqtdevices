#include <QtTest/QtTest>

#include "test_landevice.h"
#include "test_energy_device.h"
#include "test_poweramplifier_device.h"


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	TestLanDevice test_lan_device;
	QTest::qExec(&test_lan_device, argc, argv);

	TestEnergyDevice test_energy_device;
	QTest::qExec(&test_energy_device, argc, argv);

	TestPowerAmplifierDevice test_poweramplifier_device;
	QTest::qExec(&test_poweramplifier_device, argc, argv);
}

