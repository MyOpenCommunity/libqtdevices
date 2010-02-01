#include <QtTest/QtTest>
#include <QList>
#include <QRegExp>

#include "test_platform_device.h"
#include "test_energy_device.h"
#include "test_poweramplifier_device.h"
#include "test_lighting_device.h"
#include "test_automation_device.h"
#include "test_checkaddress.h"
#include "test_scenario_device.h"
#include "test_thermal_device.h"
#include "test_pull_manager.h"
#include "test_alarmsounddiff_device.h"
#include "test_entryphone_device.h"
#include "test_probe_device.h"
#include "test_air_conditioning_device.h"
#include "main.h" // bt_global::config

// This empty function is required because frame_interpreter use a rearmWDT
// function, so we have to define it. We don't want to include hardware_functions,
// in order to compile and execute all tests with the standard version of Qt.
void rearmWDT() {}

// move this somewhere else (generic_functions.cpp?): we can't link main.cpp
QHash<GlobalFields, QString> bt_global::config;


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QList<TestDevice *> test_list;

	TestPlatformDevice test_platform_device;
	test_list << &test_platform_device;

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

	TestThermalDevice4Zones test_thermal_regulator_4zones_device;
	test_list << &test_thermal_regulator_4zones_device;

	TestThermalDevice99Zones test_thermal_regulator_99zones_device;
	test_list << &test_thermal_regulator_99zones_device;

	TestAlarmSoundDiffDevice test_alarm_sound_diff_device;
	test_list << &test_alarm_sound_diff_device;

	TestEntryphoneDevice test_entryphone_device;
	test_list << &test_entryphone_device;

	TestNonControlledProbeDevice test_non_controlled_probe_device;
	test_list << &test_non_controlled_probe_device;

	TestExternalProbeDevice test_external_probe_device;
	test_list << &test_external_probe_device;

	TestControlledProbeDevice test_controlled_probe_device;
	test_list << &test_controlled_probe_device;

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

	// use regular expressions to avoid writing the full class name each time
	QRegExp re(testingClass, Qt::CaseInsensitive);
	foreach (TestDevice *tester, test_list)
	{
		QString class_name = tester->metaObject()->className();
		if (testingClass.isEmpty() || class_name.contains(re))
		{
			tester->initTestDevice();
			QTest::qExec(tester, arglist);
		}
	}
}

