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


#include "test_thermal_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "thermal_device.h"
#include "bttime.h"

#include <openmsg.h>

#include <QtTest/QtTest>

#define ZERO_FILL QLatin1Char('0')

// TestThermalDevice implementation]

void TestThermalDevice::setTestDevice(ThermalDevice *d)
{
	dev = d;
}

void TestThermalDevice::sendSetOff()
{
	dev->setOff();
	client_command->flush();
	QString cmd = QString("*4*303") +
		      "*" + dev->where +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetSummer()
{
	dev->setSummer();
	client_command->flush();
	QString cmd = QString("*4*0") +
		      "*" + dev->where +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetWinter()
{
	dev->setWinter();
	client_command->flush();
	QString cmd = QString("*4*1") +
		      "*" + dev->where +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetProtection()
{
	dev->setProtection();
	client_command->flush();
	QString cmd = QString("*4*302") +
		      "*" + dev->where +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetHolidayDateTime()
{
	dev->setHolidayDateTime(QDate(2009, 2, 13), BtTime(23, 31, 0), 15);
	client_command->flush();
	QString cmd = QString("*4*33002#3115") +
		      "*" + dev->where +
		      "##";
	QCOMPARE(server->frameCommand(), cmd + holidayDateFrame(QDate(2009, 2, 13)) + holidayTimeFrame(BtTime(23, 31, 0)));
}

void TestThermalDevice::sendSetWeekendDateTime()
{
	dev->setWeekendDateTime(QDate(2010, 1, 1), BtTime(4, 4, 0), 12);
	client_command->flush();
	QString cmd = QString("*4*315#3112") +
		      "*" + dev->where +
		      "##";
	QCOMPARE(server->frameCommand(), cmd + holidayDateFrame(QDate(2010, 1, 1)) + holidayTimeFrame(BtTime(4, 4, 0)));
}

void TestThermalDevice::sendSetWeekProgram()
{
	dev->setWeekProgram(13);
	client_command->flush();
	QString cmd = QString("*4*3113") +
		      "*" + dev->where +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetManualTemp()
{
	dev->setManualTemp(250);
	client_command->flush();
	QString cmd = QString("*#4") +
		      "*" + dev->where +
		      QString("*#14*0250*3") +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

QString TestThermalDevice::holidayDateFrame(const QDate &date)
{
	QString cmd = QString("*#4") +
		      "*" + dev->where +
		      QString("*#30*%1*%2*%3").arg(date.day(), 2, 10, ZERO_FILL)
					      .arg(date.month(), 2, 10, ZERO_FILL)
					      .arg(date.year(), 4, 10, ZERO_FILL) +
		      "##";
	return cmd;
}

QString TestThermalDevice::holidayTimeFrame(const BtTime &time)
{
	QString cmd = QString("*#4") +
		      "*" + dev->where +
		      QString("*#31*%1*%2").arg(time.hour(), 2, 10, ZERO_FILL)
					   .arg(time.minute(), 2, 10, ZERO_FILL) +
		      "##";
	return cmd;
}

void TestThermalDevice::receiveSummerProtection()
{
	checkStatusSeason(202, ThermalDevice::ST_PROTECTION, ThermalDevice::SE_SUMMER);
}

void TestThermalDevice::receiveSummerOff()
{
	checkStatusSeason(203, ThermalDevice::ST_OFF, ThermalDevice::SE_SUMMER);
}

void TestThermalDevice::receiveSummerWeekend()
{
	QString frame = QString("*4*215#2102*%1##").arg(dev->where);

	checkStatusSeasonProgram(frame, ThermalDevice::ST_WEEKEND, ThermalDevice::SE_SUMMER, 2);
}

void TestThermalDevice::receiveSummerHoliday()
{
	QString frame = QString("*4*23004*%1##").arg(dev->where);

	checkStatusSeasonProgram(frame, ThermalDevice::ST_HOLIDAY, ThermalDevice::SE_SUMMER, 4);
}

void TestThermalDevice::receiveWinterProtection()
{
	checkStatusSeason(102, ThermalDevice::ST_PROTECTION, ThermalDevice::SE_WINTER);
}

void TestThermalDevice::receiveWinterOff()
{
	checkStatusSeason(103, ThermalDevice::ST_OFF, ThermalDevice::SE_WINTER);
}

void TestThermalDevice::receiveWinterWeekend()
{
	QString frame = QString("*4*115#1102*%1##").arg(dev->where);

	checkStatusSeasonProgram(frame, ThermalDevice::ST_WEEKEND, ThermalDevice::SE_WINTER, 2);
}

void TestThermalDevice::receiveWinterHoliday()
{
	QString frame = QString("*4*13004*%1##").arg(dev->where);

	checkStatusSeasonProgram(frame, ThermalDevice::ST_HOLIDAY, ThermalDevice::SE_WINTER, 4);
}

void TestThermalDevice::receiveSummerManual()
{
	checkStatusSeasonTemperature(210, ThermalDevice::ST_MANUAL, ThermalDevice::SE_SUMMER, 250);
}

void TestThermalDevice::receiveSummerManualTimed()
{
	checkStatusSeasonTemperature(212, ThermalDevice::ST_MANUAL_TIMED, ThermalDevice::SE_SUMMER, 260);
}

void TestThermalDevice::receiveSummerProgram()
{
	QString frame = QString("*#4*%1*%2##").arg(dev->where).arg(2100 + 3);

	checkStatusSeasonProgram(frame, ThermalDevice::ST_PROGRAM, ThermalDevice::SE_SUMMER, 3);
}

void TestThermalDevice::receiveSummerScenario()
{
	MultiDeviceTester t(dev);
	t << makePair(ThermalDevice::DIM_STATUS, (int)ThermalDevice::ST_SCENARIO);
	t << makePair(ThermalDevice::DIM_SEASON, (int)ThermalDevice::SE_SUMMER);
	t << makePair(ThermalDevice::DIM_SCENARIO, 9);

	QString frame = QString("*#4*%1*%2##").arg(dev->where).arg(2200 + 9);
	t.check(frame);
}

void TestThermalDevice::receiveWinterManual()
{
	checkStatusSeasonTemperature(110, ThermalDevice::ST_MANUAL, ThermalDevice::SE_WINTER, 250);
}

void TestThermalDevice::receiveWinterManualTimed()
{
	checkStatusSeasonTemperature(112, ThermalDevice::ST_MANUAL_TIMED, ThermalDevice::SE_WINTER, 260);
}

void TestThermalDevice::receiveWinterProgram()
{
	QString frame = QString("*#4*%1*%2##").arg(dev->where).arg(1100 + 3);

	checkStatusSeasonProgram(frame, ThermalDevice::ST_PROGRAM, ThermalDevice::SE_WINTER, 3);
}

void TestThermalDevice::receiveWinterScenario()
{
	MultiDeviceTester t(dev);
	t << makePair(ThermalDevice::DIM_STATUS, (int)ThermalDevice::ST_SCENARIO);
	t << makePair(ThermalDevice::DIM_SEASON, (int)ThermalDevice::SE_WINTER);
	t << makePair(ThermalDevice::DIM_SCENARIO, 9);

	QString frame = QString("*#4*%1*%2##").arg(dev->where).arg(1200 + 9);
	t.check(frame);
}

void TestThermalDevice::checkStatusSeason(int what, int status, int season)
{
	MultiDeviceTester t(dev);
	t << makePair(ThermalDevice::DIM_STATUS, status);
	t << makePair(ThermalDevice::DIM_SEASON, season);
	QString frame = QString("*#4*%1*%2##").arg(dev->where).arg(what);
	t.check(frame);
}

void TestThermalDevice::checkStatusSeasonTemperature(int what, int status, int season, int temperature)
{
	MultiDeviceTester t(dev);
	t << makePair(ThermalDevice::DIM_STATUS ,status);
	t << makePair(ThermalDevice::DIM_SEASON, season);
	t << makePair(ThermalDevice::DIM_TEMPERATURE, temperature);

	QString frame = QString("*#4*%1*%2*%3##").arg(dev->where).arg(what).arg(temperature);
	t.check(frame);
}

void TestThermalDevice::checkStatusSeasonProgram(const QString &frame, int status, int season, int program)
{
	MultiDeviceTester t(dev);
	t << makePair(ThermalDevice::DIM_STATUS ,status);
	t << makePair(ThermalDevice::DIM_SEASON, season);
	t << makePair(ThermalDevice::DIM_PROGRAM, program);
	t.check(frame);
}



// TestThermalDevice4Zones implementation]

void TestThermalDevice4Zones::initTestCase()
{
	dev = new ThermalDevice4Zones(THERMAL_DEVICE_4Z_WHERE);
	setTestDevice(dev);
}

void TestThermalDevice4Zones::cleanupTestCase()
{
	delete dev;
	dev = NULL;
	setTestDevice(NULL);
}

void TestThermalDevice4Zones::sendSetManualTempTimed()
{
	ThermalDevice4Zones::temp_timed_delay = 0;
	dev->setManualTempTimed(20, BtTime(13, 5, 0));
	QCoreApplication::processEvents(); // to process the QTimer::singleShot
	client_command->flush();
	QString cmd1 = QString("*4*312#0020#2") +
		       "*" + dev->where +
		       "##";
	QString cmd2 = QString("*#4") +
		       "*" + dev->where +
		       "*#32*13*05" +
		       "##";
	QCOMPARE(server->frameCommand(), cmd1 + cmd2);
}


// TestThermalDevice99Zones implementation]

void TestThermalDevice99Zones::initTestCase()
{
	dev = new ThermalDevice99Zones(THERMAL_DEVICE_99Z_WHERE);
	setTestDevice(dev);
}

void TestThermalDevice99Zones::cleanupTestCase()
{
	delete dev;
	dev = NULL;
	setTestDevice(NULL);
}

void TestThermalDevice99Zones::sendSetScenario()
{
	dev->setScenario(12);
	client_command->flush();
	QString cmd = QString("*4*3212") +
		      "*" + dev->where + "##";
	QCOMPARE(server->frameCommand(), cmd);
}
