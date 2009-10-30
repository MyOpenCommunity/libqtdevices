#ifndef TEST_THERMAL_DEVICE_H
#define TEST_THERMAL_DEVICE_H

#include "test_device.h"

const QString THERMAL_DEVICE_WHERE = "0313";

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

protected:
	void setTestDevice(ThermalDevice *d);

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
