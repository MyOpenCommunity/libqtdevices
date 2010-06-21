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


#ifndef LIGHTINGDEVICE_H
#define LIGHTINGDEVICE_H

#include "pulldevice.h"


#define PULL_DELAY_LIGHT       1500
#define PULL_DELAY_DIMMER10    4000
#define PULL_DELAY_DIMMER100   9000

int dimmerLevelTo100(int level);


class LightingDevice : public PullDevice
{
friend class TestLightingDevice;
Q_OBJECT
public:
	enum Type
	{
		DIM_DEVICE_ON = 1,
		DIM_VARIABLE_TIMING = 2,
		DIM_DIMMER_PROBLEM = 19,
		DIM_DIMMER_LEVEL,
		DIM_DIMMER100_LEVEL,
		DIM_DIMMER100_SPEED,
	};

	LightingDevice(QString where, PullMode pull = PULL_UNKNOWN, int pull_delay = PULL_DELAY_LIGHT,
		       AdvancedMode adv = PULL_ADVANCED_UNKNOWN, PullStateManager::FrameChecker checker = &LightingDevice::isFrameHandled);

	void turnOn();
	void turnOn(int speed);
	void turnOff();
	void turnOff(int speed);
	void fixedTiming(int value);
	void variableTiming(int h, int m, int s);

	void requestStatus();
	void requestVariableTiming();

	static FrameHandled isFrameHandled(OpenMsg &msg);

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
	virtual void requestPullStatus();
};


class DimmerDevice : public LightingDevice
{
friend class TestDimmerDevice;
Q_OBJECT
public:
	DimmerDevice(QString where, PullMode pull = PULL_UNKNOWN, int pull_delay = PULL_DELAY_DIMMER10,
		     AdvancedMode adv = PULL_ADVANCED_UNKNOWN, PullStateManager::FrameChecker checker = &DimmerDevice::isFrameHandled);

	void increaseLevel();
	void decreaseLevel();

	static FrameHandled isFrameHandled(OpenMsg &msg);

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
	virtual int getDimmer10Level();
	virtual void delayedStatusRequest();

protected:
	QTimer delayed_level_request;
	int level;
	bool status;
};


class Dimmer100Device : public DimmerDevice
{
friend class TestDimmer100Device;
Q_OBJECT
public:
	Dimmer100Device(QString where, PullMode pull = PULL_UNKNOWN, int pull_delay = PULL_DELAY_DIMMER100,
			PullStateManager::FrameChecker checker = NULL);

	void increaseLevel100(int delta, int speed);
	void decreaseLevel100(int delta, int speed);

	void requestDimmer100Status();

protected:
	virtual void parseFrame(OpenMsg &msg, StatusList *sl);
	virtual void requestPullStatus();
	virtual int getDimmer10Level();
	virtual void delayedStatusRequest();
};

#endif // LIGHTINGDEVICE_H
