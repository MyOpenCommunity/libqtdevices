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


#ifndef TEST_ENERGY_DEVICE_H
#define TEST_ENERGY_DEVICE_H

#include "test_device.h"

#include <QString>

class EnergyDevice;


class TestEnergyDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestCumulativeDayGraph();
	void sendRequestCumulativeMonthGraph();
	void sendRequestCumulativeYearGraph();
	void sendRequestCumulativeMonth();
	void sendRequestCumulativeMonth2();
	void sendRequestCurrent();
	void sendRequestCurrent2();
	void sendRequestCurrent3();
	void sendRequestCurrent4();
	void receiveCumulativeDay();
	void receiveCumulativeDay2();
	void receiveCurrent();
	void receiveCumulativeMonth();
	void receiveCumulativeYear();
	void receiveDailyAverageGraph();
	void receiveDailyAverageGraph2();
	void receiveDayGraph();
	void receiveDayGraph2();
	void receiveDayGraph3();
	void receiveCumulativeMonthGraph();
	void receiveCumulativeMonthGraph2();
	void receiveCumulativeMonthGraph3();
	void receiveCumulativeYearGraph();
	void receiveMonthlyAverage();
	void receiveCumulativeDayRequest();
	void receiveCumulativeMonthRequest();

	void testGetDateFromFrame();
	void testConsecutiveGraphFrames();

private:
	EnergyDevice *dev;
	QString where;
};


#endif // TEST_ENERGY_DEVICE_H
