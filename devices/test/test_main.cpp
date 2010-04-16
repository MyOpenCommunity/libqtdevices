/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <QtTest/QtTest>
#include <QList>
#include <QRegExp>

#include <iostream>

#include "test_platform_device.h"
#include "test_energy_device.h"
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
#include "test_loads_device.h"
#include "test_media_device.h"
#include "test_message_device.h"
#include "main.h" // bt_global::config

// This empty function is required because frame_interpreter use a rearmWDT
// function, so we have to define it. We don't want to include hardware_functions,
// in order to compile and execute all tests with the standard version of Qt.
void rearmWDT() {}

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

	TestLoadsDevice test_loads_device;
	test_list << &test_loads_device;

	TestSourceDevice test_source_device;
	test_list << &test_source_device;

	TestRadioSourceDevice test_radio_source_device;
	test_list << &test_radio_source_device;

	TestAmplifierDevice test_amplifier_device;
	test_list << &test_amplifier_device;

	TestMessageDevice test_message_device;
	test_list << &test_message_device;

	QStringList arglist = app.arguments();
	if (arglist.contains("--help"))
	{
		std::cout << "Class List:" << std::endl;
		foreach (TestDevice *dev, test_list)
			std::cout << " " << dev->metaObject()->className() << std::endl;
		return 0;
	}

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

