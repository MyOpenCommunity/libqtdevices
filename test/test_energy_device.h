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

	void sendRequestCumulativeDayGraph();
	void sendRequestCumulativeMonthGraph();
	void sendRequestCumulativeMonth();
	void sendRequestCumulativeMonth2();

	void receiveCumulativeDay();
	void receiveCurrent();
	void receiveCumulativeMonth();
	void receiveCumulativeYear();
	void receiveDailyAverageGraph();
	void receiveDailyAverageGraph2();
	void receiveDailyAverageGraph3();
	void receiveDayGraph();
	void receiveDayGraph2();
	void receiveDayGraph3();
	void receiveCumulativeMonthGraph();
	void receiveCumulativeMonthGraph2();

	void testGetDateFromFrame();
	void testConsecutiveGraphFrames();

private:
	EnergyDevice *dev;
	QString where;
};


#endif // TEST_ENERGY_DEVICE_H
