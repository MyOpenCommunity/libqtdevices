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


#ifndef TEST_SCENEVODEVICESCOND_H
#define TEST_SCENEVODEVICESCOND_H

#include <QObject>
#include <QString>

class DeviceConditionDisplayMock;
class OpenServerMock;
class ClientWriter;
class ClientReader;
class QSignalSpy;


/**
 * The test class used to check the device conditions of the evolved scenarios.
 */
class TestScenEvoDevicesCond : public QObject
{
Q_OBJECT
public:
	TestScenEvoDevicesCond();
	virtual ~TestScenEvoDevicesCond();

private:
	OpenServerMock *server;
	ClientReader *client_monitor;
	ClientWriter *client_command;
	ClientWriter *client_request;
	QString dev_where;

	DeviceConditionDisplayMock *mock_display;
	void checkCondition(QSignalSpy &spy, QString frame, bool satisfied);

private slots:
	void init();

	// tests below
	void testLightOn();
	void testLightOff();
	void testLightConditionChange1();
	void testLightConditionChange2();
	void testLightConditionChange3();
	void testDimmingOff();
	void testDimmingRange1();
	void testDimmingRange2();
	void testDimmingRange3();
	void testDimmingConditionChange1();
	void testDimmingConditionChange2();
	void testDimmingConditionChange3();
	void testDimming100Off();
	void testDimming100Range1();
	void testDimming100Range2();
	void testDimming100Range3();
	void testDimming100Range4();
	void testDimming100ConditionChange1();
	void testDimming100ConditionChange2();
	void testDimming100ConditionChange3();
	void testInternalTemperature1();
	void testInternalTemperature2();
	void testInternalTemperature3();
	void testInternalTemperatureConditionChange1();
	void testInternalTemperatureConditionChange2();
	void testInternalTemperatureConditionChange3();
	void testInternalTemperatureMinMax();
	void testExternalTemperature1();
	void testExternalTemperature2();
	void testExternalTemperature3();
	void testExternalTemperatureMinMax();
	void testExternalTemperatureConditionChange1();
	void testExternalTemperatureConditionChange2();
	void testExternalTemperatureConditionChange3();
	void testAux();
	void testAuxAtStart();
	void testAuxConditionChange1();
	void testAuxConditionChange2();
	void testAuxConditionChange3();
	void testVolumeOn();
	void testVolumeOff();
	void testVolumeRange1();
	void testVolumeRange2();
	void testVolumeRange3();
	void testVolumeRange4();
	void testVolumeConditionChange1();
	void testVolumeConditionChange2();
	void testVolumeConditionChange3();
};

#endif // TEST_SCENEVODEVICESCOND_H
