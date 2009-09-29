#include <QtTest/QtTest>

#include "test_landevice.h"
#include "test_energy_device.h"
#include "test_poweramplifier_device.h"
#include "test_dev_automation.h"
#include "test_lighting_device.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	TestLanDevice test_lan_device;
	QTest::qExec(&test_lan_device, argc, argv);

	TestEnergyDevice test_energy_device;
	QTest::qExec(&test_energy_device, argc, argv);

	TestPowerAmplifierDevice test_poweramplifier_device;
	QTest::qExec(&test_poweramplifier_device, argc, argv);

	TestPPTStatDevice test_pptstat_device;
	QTest::qExec(&test_pptstat_device, argc, argv);

	TestLightingDevice test_lighting_no_interface("0313");
	QTest::qExec(&test_lighting_no_interface, argc, argv);

	TestLightingDevice test_lighting_interface("0313#4#12");
	QTest::qExec(&test_lighting_interface, argc, argv);
}

