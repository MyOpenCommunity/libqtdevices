#include <QtTest/QtTest>

#include "test_landevice.h"
#include "test_energy_device.h"


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	TestLanDevice test_lan_device;
	QTest::qExec(&test_lan_device, argc, argv);

	TestEnergyDevice test_energy_device;
	QTest::qExec(&test_energy_device, argc, argv);

}

