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

enum AdvancedMode
{
	// PULL_UNKNOWN and NOT_PULL devices with unknown state
	PULL_ADVANCED_UNKNOWN,
	// NOT_PULL advanced devices
	PULL_ADVANCED,
	// NOT_PULL base devices and PULL devices
	PULL_NOT_ADVANCED,
};

enum FrameHandled
{
	FRAME_NOT_HANDLED,
	FRAME_HANDLED,
	FRAME_MAYBE_HANDLED,
};

AddressType checkAddressIsForMe(const QString &msg_where, const QString &dev_where);


class PullStateManager
{
friend class TestLightingDevice;
friend class TestPullManager;
public:
	typedef FrameHandled (*FrameChecker)(OpenMsg &msg);
	typedef QPair<bool, FrameHandled> CheckResult;

	PullStateManager(PullMode m, AdvancedMode adv = PULL_ADVANCED_UNKNOWN, FrameChecker checker = NULL);

	CheckResult moreFrameNeeded(OpenMsg &msg, bool is_environment);
	PullMode getPullMode();
	void setStatusRequested(bool status);
	AdvancedMode getAdvanced() { return advanced; }
	bool isDetectionComplete() { return mode != PULL_UNKNOWN && advanced != PULL_ADVANCED_UNKNOWN; }

private:
	int status;
	bool status_requested;
	PullMode mode;
	AdvancedMode advanced;

	// filters the frames interpreted by this device
	FrameChecker frame_checker;
	// the handled/maybe handled status of the last environment frame processed
	FrameHandled last_handled;
};


class PullDevice : public device
{
friend class TestLightingDevice;
friend class TestDimmerDevice;
Q_OBJECT
public:
	virtual void manageFrame(OpenMsg &msg);

protected:
	PullDevice(QString who, QString where, PullMode m, int openserver_id, int pull_delay, AdvancedMode adv = PULL_ADVANCED_UNKNOWN, PullStateManager::FrameChecker checker = NULL);

	// different devices may need different status requests (eg. Dimmer100)
	virtual void requestPullStatus() = 0;

	bool isAdvanced() { return state.getAdvanced() == PULL_ADVANCED; }

private slots:
	void delayedStatusRequest();

private:
	PullStateManager state;
	QTimer delayed_request;
};
#endif // PULLDEVICE_H

/*! \file */ /* otherwise doxygen does not generate documentation for enums */
