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


#ifndef TEST_THERMAL_DEVICE_H
#define TEST_THERMAL_DEVICE_H

#include "test_device.h"

const QString THERMAL_DEVICE_4Z_WHERE = "1#2";
const QString THERMAL_DEVICE_99Z_WHERE = "0";

class QDate;
class BtTime;
class ThermalDevice;
class ThermalDevice4Zones;
class ThermalDevice99Zones;


class TestThermalDevice : public TestDevice
{
Q_OBJECT
private slots:
	void sendSetOff();
	void sendSetSummer();
	void sendSetWinter();
	void sendSetProtection();
	void sendSetHolidayDateTime();
	void sendSetWeekendDateTime();
	void sendSetWeekProgram();
	void sendSetManualTemp();

	void receiveSummerProtection();
	void receiveSummerOff();
	void receiveSummerWeekend();
	void receiveSummerHoliday();

	void receiveWinterProtection();
	void receiveWinterOff();
	void receiveWinterWeekend();
	void receiveWinterHoliday();

	void receiveSummerManual();
	void receiveSummerManualTimed();
	void receiveSummerProgram();
	void receiveSummerScenario();

	void receiveWinterManual();
	void receiveWinterManualTimed();
	void receiveWinterProgram();
	void receiveWinterScenario();

protected:
	void setTestDevice(ThermalDevice *d);
	void checkStatusSeason(int what, int status, int season);
	void checkStatusSeasonTemperature(int what, int status,
					  int season, int temperature);

private:
	QString holidayDateFrame(const QDate &date);
	QString holidayTimeFrame(const BtTime &date);

private:
	ThermalDevice *dev;
};


class TestThermalDevice4Zones : public TestThermalDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendSetManualTempTimed();

private:
	ThermalDevice4Zones *dev;
};


class TestThermalDevice99Zones : public TestThermalDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendSetScenario();

private:
	ThermalDevice99Zones *dev;
};

#endif // TEST_THERMAL_DEVICE_H
