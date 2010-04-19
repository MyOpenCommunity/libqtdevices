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
class AutomaticUpdates;


class TestEnergyDevice : public TestDevice
{
Q_OBJECT
private slots:
	void init();
	void cleanup();

	void sendRequestCumulativeDayGraph8Bit();
	void sendRequestCumulativeMonthGraph8Bit();
	void sendRequestCumulativeYearGraph();
	void sendRequestCumulativeMonth();
	void sendRequestCumulativeMonth2();
	void sendRequestCurrent();
	void sendRequestCurrent2();
	void sendRequestCurrent3();
	void sendRequestCurrent4();
	void sendRequestDailyAverageGraph16Bit();
	void sendRequestCumulativeDayGraph16Bit();
	void sendRequestCumulativeMonthGraph32Bit();

	void receiveCumulativeDay();
	void receiveCumulativeDay2();
	void receiveCumulativeDay16Bit();
	void receiveCurrent();
	void receiveCumulativeMonth();
	void receiveCumulativeYear();
	void receiveDailyAverageGraph();
	void receiveDailyAverageGraph2();
	void receiveDailyAverageGraph16Bit();
	void receiveDayGraph();
	void receiveDayGraph2();
	void receiveDayGraph3();
	void receiveDayGraph16Bit();
	void receiveCumulativeMonthGraph();
	void receiveCumulativeMonthGraph2();
	void receiveCumulativeMonthGraph3();
	void receiveCumulativeMonthGraph32Bit();
	void receiveCumulativeMonthGraph32Bit2();
	void receiveCumulativeMonthGraphPrevYear32Bit();
	void receiveCumulativeYearGraph();
	void receiveMonthlyAverage();
	void receiveMonthlyAverage16Bit();
	void receiveCumulativeDayRequest();
	void receiveCumulativeMonthRequest();

	void testGetDateFromFrame();
	void testConsecutiveGraphFrames();

	// logic for automatic switch from 8 bit to 16/32 bit frames
	void receiveInvalidFrameRequestDailyAverageGraph16Bit();
	void receiveInvalidFrameRequestCumulativeDayGraph16Bit();
	void receiveInvalidFrameRequestCumulativeMonthGraph32Bit();

	// automatic updates
	void sendUpdateStart();
	void sendUpdateStop();

	void receiveUpdateInterval();
	void receiveUpdateStop();

	void testUpdateStartPolling();
	void testUpdateStartAutomatic();

	void testUpdateStop();

private:
	AutomaticUpdates *upd;
	EnergyDevice *dev;
	QString where;
};


#endif // TEST_ENERGY_DEVICE_H
