/**
 * \file
 * <!--
 * Copyright 2009 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief This file contain the specific device for the power amplifier.
 *
 * \author Gianni Valdambrini <aleister@develer.com>
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
