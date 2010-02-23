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


#ifndef TEST_POWERAMPLIFIER_DEVICE_H
#define TEST_POWERAMPLIFIER_DEVICE_H

#include "test_device.h"

#include <QString>

class PowerAmplifierDevice;
class OpenServerMock;


class TestPowerAmplifierDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();
	void sendTurnOn();
	void sendTurnOff();
	void sendVolumeUp();
	void sendVolumeDown();
	void sendTrebleUp();
	void sendTrebleDown();
	void sendBassUp();
	void sendBassDown();
	void sendBalanceUp();
	void sendBalanceDown();
	void sendNextPreset();
	void sendPrevPreset();
	void sendLoudOn();
	void sendLoudOff();

	void receiveStatus();
	void receiveVolume();
	void receiveLoud();
	void receivePreset();
	void receiveTreble();
	void receiveBass();
	void receiveBalance();
	void receiveStatusRequest();

private:
	PowerAmplifierDevice *dev;
	QString where;
};

#endif // TEST_POWERAMPLIFIER_DEVICE_H
