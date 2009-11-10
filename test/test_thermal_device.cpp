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
	checkStatusSeason(215, ThermalDevice::ST_WEEKEND, ThermalDevice::SE_SUMMER);
}

void TestThermalDevice::receiveSummerHoliday()
{
	checkStatusSeason(23000, ThermalDevice::ST_HOLIDAY, ThermalDevice::SE_SUMMER);
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
	checkStatusSeason(115, ThermalDevice::ST_WEEKEND, ThermalDevice::SE_WINTER);
}

void TestThermalDevice::receiveWinterHoliday()
{
	checkStatusSeason(13000, ThermalDevice::ST_HOLIDAY, ThermalDevice::SE_WINTER);
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
	DeviceTester tst(dev, ThermalDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tse(dev, ThermalDevice::DIM_SEASON, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tpr(dev, ThermalDevice::DIM_PROGRAM, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#%1*%2*%3##").arg(dev->who).arg(dev->where).arg(2100 + 3);

	tst.check<int>(frame, ThermalDevice::ST_PROGRAM);
	tse.check<int>(frame, ThermalDevice::SE_SUMMER);
	tpr.check(frame, 3);
}

void TestThermalDevice::receiveSummerScenario()
{
	DeviceTester tst(dev, ThermalDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tse(dev, ThermalDevice::DIM_SEASON, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tpr(dev, ThermalDevice::DIM_SCENARIO, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#%1*%2*%3##").arg(dev->who).arg(dev->where).arg(2200 + 9);

	tst.check<int>(frame, ThermalDevice::ST_SCENARIO);
	tse.check<int>(frame, ThermalDevice::SE_SUMMER);
	tpr.check(frame, 9);
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
	DeviceTester tst(dev, ThermalDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tse(dev, ThermalDevice::DIM_SEASON, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tpr(dev, ThermalDevice::DIM_PROGRAM, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#%1*%2*%3##").arg(dev->who).arg(dev->where).arg(1100 + 3);

	tst.check<int>(frame, ThermalDevice::ST_PROGRAM);
	tse.check<int>(frame, ThermalDevice::SE_WINTER);
	tpr.check(frame, 3);
}

void TestThermalDevice::receiveWinterScenario()
{
	DeviceTester tst(dev, ThermalDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tse(dev, ThermalDevice::DIM_SEASON, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tpr(dev, ThermalDevice::DIM_SCENARIO, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#%1*%2*%3##").arg(dev->who).arg(dev->where).arg(1200 + 9);

	tst.check<int>(frame, ThermalDevice::ST_SCENARIO);
	tse.check<int>(frame, ThermalDevice::SE_WINTER);
	tpr.check(frame, 9);
}

void TestThermalDevice::checkStatusSeason(int what, int status, int season)
{
	DeviceTester tst(dev, ThermalDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tse(dev, ThermalDevice::DIM_SEASON, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#%1*%2*%3##").arg(dev->who).arg(dev->where).arg(what);

	tst.check(frame, status);
	tse.check(frame, season);
}

void TestThermalDevice::checkStatusSeasonTemperature(int what, int status, int season, int temperature)
{
	DeviceTester tst(dev, ThermalDevice::DIM_STATUS, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tse(dev, ThermalDevice::DIM_SEASON, DeviceTester::MULTIPLE_VALUES);
	DeviceTester tte(dev, ThermalDevice::DIM_TEMPERATURE, DeviceTester::MULTIPLE_VALUES);
	QString frame = QString("*#%1*%2*%3*%4##").arg(dev->who).arg(dev->where).arg(what).arg(temperature);

	tst.check(frame, status);
	tse.check(frame, season);
	tte.check(frame, temperature);
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
	dev->setManualTempTimed(20, BtTime(13, 5, 0));
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
