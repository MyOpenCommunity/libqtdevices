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


#ifndef TESTAIRCONDITIONINGDEVICE_H
#define TESTAIRCONDITIONINGDEVICE_H

#include "test_device.h"

class AirConditioningDevice;
class AdvancedAirConditioningDevice;

class TestAirConditioningDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendActivateScenario();
	void sendSendCommand();
	void sendTurnOff();

private:
	void compareCommand(const QString &what);
	AirConditioningDevice *dev;
};

class TestAdvancedAirConditioningDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();

	void receiveSetStatusSetOk();
	void receiveSetStatusSetError();
	void receiveSetStatusNotSet();

	void testStatusToString();
	void testStatusToString2();
	void testStatusToString3();
	void testStatusToString4();

private:
	AdvancedAirConditioningDevice *dev;
};

#endif // TESTAIRCONDITIONINGDEVICE_H
