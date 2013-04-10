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


#ifndef TEST_PULL_MANAGER_H
#define TEST_PULL_MANAGER_H
#include "test_device.h"


class PullStateManager;

class TestPullManager : public TestDevice
{
Q_OBJECT
private slots:
	void testSimpleLight_on();
	void testSimpleLight_off();
	void testSimpleLight_on_100();
	void testSimpleLight_off_100();
	void testSimpleLight_setLevel();
	void testSimpleLight_setLevel_100();
	void testSimpleLight_increaseLevel();
	void testSimpleLight_decreaseLevel();
	void testSimpleLight_increaseLevel_100();
	void testSimpleLight_decreaseLevel_100();
	void testSimpleLight_timing();
	void testSimpleLight_variabletiming();

	void testDimmer_on();
	void testDimmer_off();
	void testDimmer_on_100();
	void testDimmer_off_100();
	void testDimmer_setLevel();
	void testDimmer_setLevel_100();
	void testDimmer_increaseLevel();
	void testDimmer_decreaseLevel();
	void testDimmer_increaseLevel_100();
	void testDimmer_decreaseLevel_100();
	void testDimmer_timing();
	void testDimmer_variabletiming();

	void testDimmer100_on();
	void testDimmer100_off();
	void testDimmer100_on_100();
	void testDimmer100_off_100();
	void testDimmer100_setLevel();
	void testDimmer100_setLevel_100();
	void testDimmer100_increaseLevel();
	void testDimmer100_decreaseLevel();
	void testDimmer100_increaseLevel_100();
	void testDimmer100_decreaseLevel_100();
	void testDimmer100_timing();
	void testDimmer100_variabletiming();

	void testSimpleLight();
	void testSimpleLight2();
	void testDimmer();
	void testDimmer2();
	void testDimmer3();
	void testDimmer_increaseLevel_old();
	void testDimmer_decreaseLevel_old();
	void testDimmer100();
	void testDimmer100_2();
	void testDimmer100_3();
	void testDimmer100_increaseLevel_old();
	void testDimmer100_decreaseLevel_old();
	void testDimmer100_receiveIncreaseLevel10();

	void testVariableTiming();
	void testVariableTiming2();
	void testVariableTiming3();
	void testVariableTiming4();

private:
	bool parseFrames(const char *frame, PullStateManager *psm, bool is_environment);
};

#endif //TEST_PULL_MANAGER_H
