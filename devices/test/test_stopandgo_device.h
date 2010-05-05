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
#ifndef TEST_STOPANDGO_DEVICE_H
#define TEST_STOPANDGO_DEVICE_H

#include "test_device.h"

class StopAndGoDevice;
class StopAndGoPlusDevice;
class StopAndGoBTestDevice;


class TestStopAndGoDevice : public TestDevice
{
Q_OBJECT
private slots:
	void init();
	void cleanup();

	void sendAutoResetActivation();
	void sendAutoResetDisactivation();
	void requestICMState();

	void receiveICMState();

protected:
	StopAndGoDevice *dev;
};

class TestStopAndGoPlusDevice : public TestStopAndGoDevice
{
Q_OBJECT
private slots:
	void init();
	void cleanup();

	void sendClose();
	void sendOpen();
	void sendTrackingSystemActivation();
	void sendTrackingSystemDisactivation();

private:
	StopAndGoPlusDevice *plus;
};

class TestStopAndGoBTestDevice : public TestStopAndGoDevice
{
Q_OBJECT
private slots:
	void init();
	void cleanup();

	void sendDiffSelftestActivation();
	void sendDiffSelftestDisactivation();
	void sendSelftestFreq(int days);
	void requestSelftestFreq();

	void receiveSelftestFreq();

private:
	StopAndGoBTestDevice *btest;
};

#endif // TEST_STOPANDGO_DEVICE_H
