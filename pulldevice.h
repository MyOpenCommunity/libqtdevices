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


#ifndef PULLDEVICE_H
#define PULLDEVICE_H

#include "device.h"

class QString;
class OpenMsg;


enum AddressType
{
	NOT_MINE = 0,
	ENVIRONMENT,
	GLOBAL,
	P2P,
};

enum PullMode
{
	PULL,
	NOT_PULL,
	PULL_UNKNOWN,
};

/**
 * Check if the address found in a frame is valid for the device.
 *
 * Frame address must be environment or general. Addresses must be complete, ie
 * must have a+pf and extension part (if any).
 * \return true if msg_where includes the device, false otherwise.
 */
AddressType checkAddressIsForMe(const QString &msg_where, const QString &dev_where);


class PullStateManager
{
friend class TestLightingDevice;
friend class TestPullManager;
public:
	PullStateManager(PullMode m);
	/**
	 * Logic for the state manager.
	 * Return true if a point-to-point status frame is needed to choose device's mode, false
	 * if no request frame must be sent.
	 */
	bool moreFrameNeeded(OpenMsg &msg, bool is_environment);
	PullMode getPullMode();
	void setStatusRequested(bool status);

private:
	int status;
	bool status_requested;
	PullMode mode;
};


/*
 * Class to encapsulate PULL mode discovering behaviour.
 *
 * Derived classes must reimplement the two pure virtual functions
 * parseFrame(): function called for every received frame. Derived classes must parse the frame and put the
 *    results into the StatusList.
 * requestPullStatus(): send a status request to the device. This can be reimplemented depending on device
 *    necessities.
 * Derived classes MUST NOT reimplement manageFrame(), as they will override PULL mode discovery logic
 */
class PullDevice : public device
{
friend class TestLightingDevice;
Q_OBJECT
public:
	// TODO: hack to avoid too many changes in device.h, REMOVE when new device parsing is ok.
	virtual void frame_rx_handler(char *frame);
	virtual void manageFrame(OpenMsg &msg);

protected:
	PullDevice(QString who, QString where, PullMode m, int pull_delay);
	// parse the frame and put the results into the provided StatusList
	virtual void parseFrame(OpenMsg &msg, StatusList *sl) = 0;
	// different devices may need different status requests (eg. Dimmer100)
	virtual void requestPullStatus() = 0;

private slots:
	void delayedStatusRequest();

private:
	PullStateManager state;
	QTimer delayed_request;
};
#endif // PULLDEVICE_H
