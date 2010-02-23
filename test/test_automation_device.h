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


#ifndef TEST_AUTOMATION_DEVICE_H
#define TEST_AUTOMATION_DEVICE_H

#include "test_checkaddress.h"

class AutomationDevice;
class PPTStatDevice;
class QString;

class TestAutomationDevice : public TestCheckAddress
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendGoUp();
	void sendGoDown();
	void sendStop();

	// TODO: test PULL modes. Use a base class to share code with lighting tests?
	void receiveUp();
	void receiveDown();
	void receiveStop();

private:
	AutomationDevice *dev;
};


class TestPPTStatDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();
	void receiveStatus();

private:
	PPTStatDevice *dev;
	QString where;
};


#endif // TEST_AUTOMATION_DEVICE_H
