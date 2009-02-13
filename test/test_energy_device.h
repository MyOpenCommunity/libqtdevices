#ifndef TEST_ENERGY_DEVICE_H
#define TEST_ENERGY_DEVICE_H

#include "test_device.h"

#include <QString>

class EnergyDevice;
class OpenServerMock;


class TestEnergyDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void requestCumulativeMonth();
	void requestCumulativeMonth2();
	void readCumulativeDay();
	void readCurrent();
	void readCumulativeMonth();
	void readCumulativeYear();
	void readDailyAverageGraph();
	void readDailyAverageGraph2();
	void readDailyAverageGraph3();
	void readDayGraph();
	void readDayGraph2();
	void readDayGraph3();
	void requestCumulativeDayGraph();
	void requestCumulativeMonthGraph();
	void readCumulativeMonthGraph();
	void readCumulativeMonthGraph2();

private:
	EnergyDevice *dev;
	QString where;
};


#endif // TEST_ENERGY_DEVICE_H
