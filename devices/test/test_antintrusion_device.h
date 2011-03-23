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

#ifndef TEST_ANTINTRUSION_DEVICE_H
#define TEST_ANTINTRUSION_DEVICE_H

#include "test_device.h"

class AntintrusionDevice;


class TestAntintrusionDevice : public TestDevice
{
Q_OBJECT
private slots:
	void init();
	void cleanup();

	void sendToggleActivation1();
	void sendToggleActivation2();
	void sendToggleActivation3();
	void sendRequestStatus();

	void testSetPartialization();

	void receiveSystemInserted();
	void receiveSystemMaintenance();
	void receiveZonePartialized();
	void receiveZoneInserted();
	void receiveIntrusionAlarm();
	void receiveTamperAlarm();
	void receiveAntipanicAlarm();
	void receiveTechnicalAlarm();
	void receiveResetTechnicalAlarm();
private:
	AntintrusionDevice *dev;
};

#endif
