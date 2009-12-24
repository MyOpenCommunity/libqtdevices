#include <QtTest/QtTest>

#include "test_landevice.h"
#include "test_energy_device.h"
#include "test_poweramplifier_device.h"
#include "test_automation_device.h"
#include "test_lighting_device.h"
#include "test_automation_device.h"
#include "test_checkaddress.h"
#include "test_scenario_device.h"
#include "test_pull_manager.h"
#include "test_probe_device.h"
#include "test_air_conditioning_device.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QList<TestDevice *> test_list;

	TestEnergyDevice test_energy_device;
	test_list << &test_energy_device;

	TestPowerAmplifierDevice test_poweramplifier_device;
	test_list << &test_poweramplifier_device;

	TestPPTStatDevice test_pptstat_device;
	test_list << &test_pptstat_device;

	TestCheckAddress test_checkaddress;
	test_list << &test_checkaddress;

	TestPullManager test_pull_manager;
	test_list << &test_pull_manager;

	TestLightingDevice test_lighting_device;
	test_list << &test_lighting_device;

	TestDimmer test_dimmer;
	test_list << &test_dimmer;

	TestDimmer100 test_dimmer100;
	test_list << &test_dimmer100;

	TestAutomationDevice test_automation_device;
	test_list << &test_automation_device;

	TestScenarioDevice test_scenario_device;
	test_list << &test_scenario_device;

	TestNonControlledProbeDevice test_non_controlled_probe_device;
	test_list << &test_non_controlled_probe_device;

	TestExternalProbeDevice test_external_probe_device;
	test_list << &test_external_probe_device;

	TestAirConditioningDevice test_air_conditioning_device;
	test_list << &test_air_conditioning_device;

	TestAdvancedAirConditioningDevice test_advanced_air_conditioning_device;
	test_list << &test_advanced_air_conditioning_device;

	QStringList arglist = app.arguments();
	QString testingClass;
	int custom_param_pos = arglist.indexOf("--test-class");
	if (custom_param_pos != -1 && custom_param_pos < arglist.size() - 1)
	{
		testingClass = arglist.at(custom_param_pos + 1);
		arglist.removeAt(custom_param_pos + 1);
		arglist.removeAt(custom_param_pos);
	}

	foreach (TestDevice *tester, test_list)
		if (testingClass.isEmpty() || tester->metaObject()->className() == testingClass)
		{
			tester->initTestDevice();
			QTest::qExec(tester, arglist);
		}
}

