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


#ifndef TEST_PROBE_DEVICE_H
#define TEST_PROBE_DEVICE_H

#include "test_device.h"

class NonControlledProbeDevice;
class ControlledProbeDevice;

class TestNonControlledProbeDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();

	void receiveTemperature();

private:
	NonControlledProbeDevice *dev;
};

class TestExternalProbeDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();

	void receiveTemperature();

private:
	NonControlledProbeDevice *dev;
};

class TestControlledProbeDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendSetManual();
	void sendSetAutomatic();
	void sendSetFancoilSpeed();
	void sendRequestFancoilStatus();
	void sendRequestStatus();

	void receiveFancoilStatus();
	void receiveTemperature();
	void receiveManual();
	void receiveAuto();
	void receiveAntifreeze();
	void receiveOff();
	void receiveSetPoint();
	void receiveLocalOffset();
	void receiveLocalOff();
	void receiveLocalAntifreeze();
	void receiveSetPointAdjusted();

private:
	ControlledProbeDevice *dev;
};

#endif // TEST_PROBE_DEVICE_H
