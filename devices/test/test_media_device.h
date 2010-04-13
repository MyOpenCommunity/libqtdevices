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

#ifndef TEST_MEDIA_DEVICE_H
#define TEST_MEDIA_DEVICE_H

#include "test_device.h"

#include <QString>


class SourceDevice;
class RadioSourceDevice;
class AmplifierDevice;
class PowerAmplifierDevice;


class TestSourceDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendNextTrack();
	void sendPrevTrack();
	void sendTurnOn();
	void sendRequestTrack();

	void receiveStatus();
	void receiveTrack();

private:
	SourceDevice *dev;
	QString source_id;
};


class TestRadioSourceDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendFrequenceUp();
	void sendFrequenceDown();
	void sendSaveStation();
	void sendRequestFrequency();

	void receiveFrequency();
	void receiveRDS();
	void receiveStopRDS();

private:
	RadioSourceDevice *dev;
	QString source_id;
};


class TestAmplifierDevice : public TestDevice
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

	void receiveStatus();
	void receiveVolume();
	void receiveStatusRequest();

protected:
	void initAmplifier(AmplifierDevice *dev = 0);

private:
	AmplifierDevice *dev;
	QString area;
	QString point;
};


class TestPowerAmplifierDevice : public TestAmplifierDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

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

	void receiveLoud();
	void receivePreset();
	void receiveTreble();
	void receiveBass();
	void receiveBalance();

protected:
	void initPowerAmplifier(PowerAmplifierDevice *dev = 0);

private:
	PowerAmplifierDevice *dev;
	QString where;
};



#endif // TEST_MEDIA_DEVICE_H
