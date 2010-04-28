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
	void testSimpleLight();
	void testSimpleLight2();
	void testDimmer();
	void testDimmer2();
	void testDimmer3();
	void testDimmer100();
	void testDimmer100_2();
	void testDimmer100_3();
	void testVariableTiming();
	void testVariableTiming2();
	void testVariableTiming3();
	void testVariableTiming4();

private:
	bool parseFrames(const char *frame, PullStateManager *psm, bool is_environment);
};

#endif //TEST_PULL_MANAGER_H
