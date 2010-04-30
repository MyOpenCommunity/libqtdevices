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
class VirtualSourceDevice;
class VirtualAmplifierDevice;


class TestSourceDevice : public TestDevice
{
Q_OBJECT
public:
	TestSourceDevice();

protected:
	QString source_id;
	void initSource(SourceDevice *dev = 0);

private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendNextTrack();
	void sendPrevTrack();
	void sendTurnOn();
	void sendRequestTrack();
	void sendRequestActiveAreas();

	void receiveStatus();
	void receiveTrack();

	void testActiveAreas();

private:
	SourceDevice *dev;
};


class TestRadioSourceDevice : public TestSourceDevice
{
Q_OBJECT
protected:
	void initRadioSource(RadioSourceDevice *dev = 0);

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
};


class TestVirtualSourceDevice : public TestSourceDevice
{
Q_OBJECT
protected:
	void initVirtualSource(VirtualSourceDevice *dev = 0);

private slots:
	void initTestCase();
	void cleanupTestCase();

	void receiveNextTrack();
	void receivePrevTrack();
	void receiveSourceOn();
	void receiveSourceOff();

private:
	VirtualSourceDevice *dev;
};


class TestAmplifierDevice : public TestDevice
{
Q_OBJECT
public:
	TestAmplifierDevice();

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


class TestVirtualAmplifierDevice : public TestAmplifierDevice
{
Q_OBJECT
public:
	TestVirtualAmplifierDevice();

protected:
	void initVirtualAmplifier(VirtualAmplifierDevice *dev = 0);

private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendUpdateVolume();

private:
	VirtualAmplifierDevice *dev;
	QString where;
};


class TestPowerAmplifierDevice : public TestAmplifierDevice
{
Q_OBJECT
public:
	TestPowerAmplifierDevice();

protected:
	void initPowerAmplifier(PowerAmplifierDevice *dev = 0);

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

private:
	PowerAmplifierDevice *dev;
	QString where;
};



#endif // TEST_MEDIA_DEVICE_H
