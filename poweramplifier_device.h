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


#ifndef POWERAMPLIFIER_DEVICE_H
#define POWERAMPLIFIER_DEVICE_H

#include "device.h"


/**
 * \class PowerAmplifierDevice
 *
 * This class represent a device for managing the power amplifier. It has methods
 * to retrieve information about its settings (the request* methods) and methods to
 * modify a setting. When a request of information is done (or when something change
 * after a command), the response is sent through the signal status_changed.
 */
class PowerAmplifierDevice : public device
{
Q_OBJECT
public:
	PowerAmplifierDevice(QString address);

	// The request methods, used to request an information
	void requestStatus() const;
	void requestVolume() const;
	void requestTreble() const;
	void requestBass() const;
	void requestBalance() const;
	void requestPreset() const;
	void requestLoud() const;

	enum Type
	{
		DIM_STATUS = 12,
		DIM_VOLUME = 1,
		DIM_TREBLE = 2,
		DIM_BASS = 4,
		DIM_BALANCE = 17,
		DIM_PRESET = 19,
		DIM_LOUD = 20
	};

	void turnOn() const;
	void turnOff() const;
	void volumeUp() const;
	void volumeDown() const;
	void trebleUp() const;
	void trebleDown() const;
	void bassUp() const;
	void bassDown() const;
	void balanceUp() const;
	void balanceDown() const;
	void nextPreset() const;
	void prevPreset() const;
	void loudOn() const;
	void loudOff() const;

public slots:
	//! receive a frame
	void frame_rx_handler(char *frame);

private:
	// The location is normally used inside 'where' so we don't need to store it.
	// As exception, the turnOn/Off methods require to specify this information.
	int location;
};

#endif
