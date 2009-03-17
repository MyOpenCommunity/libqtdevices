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

class PowerAmplifierDevice : public device
{
Q_OBJECT
public:
	PowerAmplifierDevice(QString address);

	// The request methods, used to request an information
	void requestStatus() const;

public slots:
	//! receive a frame
	void frame_rx_handler(char *frame);

private:
	void sendRequest(int what) const;
};

#endif
