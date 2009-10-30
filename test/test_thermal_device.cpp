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
	QString cmd = "*" + ThermalDevice::WHO +
		      "*303" +
		      "*#" + THERMAL_DEVICE_WHERE +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetSummer()
{
	dev->setSummer();
	client_command->flush();
	QString cmd = "*" + ThermalDevice::WHO +
		      "*0" +
		      "*#" + THERMAL_DEVICE_WHERE +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetWinter()
{
	dev->setWinter();
	client_command->flush();
	QString cmd = "*" + ThermalDevice::WHO +
		      "*1" +
		      "*#" + THERMAL_DEVICE_WHERE +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetProtection()
{
	dev->setProtection();
	client_command->flush();
	QString cmd = "*" + ThermalDevice::WHO +
		      "*302" +
		      "*#" + THERMAL_DEVICE_WHERE +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetHolidayDateTime()
{
	dev->setHolidayDateTime(QDate(2009, 2, 13), BtTime(23, 31), 15);
	client_command->flush();
	QString cmd = "*" + ThermalDevice::WHO +
		      "*33002#3115" +
		      "*#" + THERMAL_DEVICE_WHERE +
		      "##";
	QCOMPARE(server->frameCommand(), cmd + holidayDateFrame(QDate(2009, 2, 13)) + holidayTimeFrame(BtTime(23, 31)));
}

void TestThermalDevice::sendSetWeekendDateTime()
{
	dev->setWeekendDateTime(QDate(2010, 1, 1), BtTime(4, 4), 12);
	client_command->flush();
	QString cmd = "*" + ThermalDevice::WHO +
		      "*315#3112" +
		      "*#" + THERMAL_DEVICE_WHERE +
		      "##";
	QCOMPARE(server->frameCommand(), cmd + holidayDateFrame(QDate(2010, 1, 1)) + holidayTimeFrame(BtTime(4, 4)));
}

void TestThermalDevice::sendSetWeekProgram()
{
	dev->setWeekProgram(13);
	client_command->flush();
	QString cmd = "*" + ThermalDevice::WHO +
		      "*3113" +
		      "*#" + THERMAL_DEVICE_WHERE +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

void TestThermalDevice::sendSetManualTemp()
{
	dev->setManualTemp(250);
	client_command->flush();
	QString cmd = "*#" + ThermalDevice::WHO +
		      "*#" + THERMAL_DEVICE_WHERE +
		      QString("*#14*%1*3").arg(250, 4, 10, ZERO_FILL) +
		      "##";
	QCOMPARE(server->frameCommand(), cmd);
}

QString TestThermalDevice::holidayDateFrame(const QDate &date)
{
	QString cmd = "*#" + ThermalDevice::WHO +
		      "*#" + THERMAL_DEVICE_WHERE +
		      QString("*#30*%1*%2*%3").arg(date.day(), 2, 10, ZERO_FILL)
					      .arg(date.month(), 2, 10, ZERO_FILL)
					      .arg(date.year(), 4, 10, ZERO_FILL) +
		      "##";
	return cmd;
}

QString TestThermalDevice::holidayTimeFrame(const BtTime &time)
{
	QString cmd = "*#" + ThermalDevice::WHO +
		      "*#" + THERMAL_DEVICE_WHERE +
		      QString("*#31*%1*%2").arg(time.hour(), 2, 10, ZERO_FILL)
					   .arg(time.minute(), 2, 10, ZERO_FILL) +
		      "##";
	return cmd;
}


// TestThermalDevice4Zones implementation]

void TestThermalDevice4Zones::initTestCase()
{
	dev = new ThermalDevice4Zones(THERMAL_DEVICE_WHERE);
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
	dev->setManualTempTimed(20, BtTime(13, 5));
	client_command->flush();
	QString cmd1 = "*" + ThermalDevice::WHO +
		       QString("*312#%1#2").arg(20, 4, 10, ZERO_FILL) +
		       "*#" + THERMAL_DEVICE_WHERE +
		       "##";
	QString cmd2 = "*#" + ThermalDevice::WHO +
		       "*#" + THERMAL_DEVICE_WHERE +
		       QString("*#32*%1*%2").arg(13, 2, 10, ZERO_FILL).arg(5, 2, 10, ZERO_FILL) +
		       "##";
	QCOMPARE(server->frameCommand(), cmd1 + cmd2);
}


// TestThermalDevice99Zones implementation]

void TestThermalDevice99Zones::initTestCase()
{
	dev = new ThermalDevice99Zones(THERMAL_DEVICE_WHERE);
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
	QString cmd = "*" + ThermalDevice::WHO +
		      "*" + QString::number(3200 + 12) +
		      "*#" + THERMAL_DEVICE_WHERE + "##";
	QCOMPARE(server->frameCommand(), cmd);
}
