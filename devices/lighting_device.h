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

// Convert a dimmer10 level into range 0-100 with the formula given on the specs
int dimmerLevelTo100(int level);


#define PULL_DELAY_LIGHT       1500
#define PULL_DELAY_DIMMER10    4000
#define PULL_DELAY_DIMMER100   9000


/*!
	\ingroup Lighting
	\brief Manages light actuators.

	The methods allow to turn on/off the light and to turn on the light for a fixed time.

	\section LightingDevice-dimensions Dimensions
	\startdim
	\dim{DIM_DEVICE_ON,bool,,The light status.}
	\dim{DIM_VARIABLE_TIMING,BtTime,,The remaining time for which the light will stay on.}
	\enddim
*/
class LightingDevice : public PullDevice
{
friend class TestLightingDevice;
Q_OBJECT
public:
	/*!
		\refdim{LightingDevice}
	*/
	enum Type
	{
		DIM_DEVICE_ON = 1,
		DIM_VARIABLE_TIMING = 2,
		DIM_DIMMER_PROBLEM = 19,
		DIM_DIMMER_LEVEL,
		DIM_DIMMER100_LEVEL,
		DIM_DIMMER100_SPEED,
	};

	enum Timed
	{
		NOT_TIMED_LIGHT,
		TIMED_LIGHT,
	};

	LightingDevice(QString where, PullMode pull = PULL_UNKNOWN, int openserver_id = 0, int pull_delay = PULL_DELAY_LIGHT,
		       AdvancedMode adv = PULL_ADVANCED_UNKNOWN, PullStateManager::FrameChecker checker = &LightingDevice::isFrameHandled);

	virtual void init();

	/*!
		\brief Turn on the light.
	*/
	void turnOn();

	/*!
		\brief Turn off the light.
	*/
	void turnOff();

	/*!
		\brief Turn on the light for the given time.
		The mapping between \a value and the light time is as follows:
		- 0: 1 minute
		- 1: 2 minutes
		- 2: 3 minutes
		- 3: 4 minutes
		- 4: 5 minutes
		- 5: 15 minutes
		- 6: 30 seconds

		Use requestVariableTiming() to periodically request the remaining time.
		\sa variableTiming()
	*/
	void fixedTiming(int value);

/*!
	\brief Turn on the light for the given time, where \a h represents hours
	(between 0 and 255), \a m the minutes (between 0 and 59) and \a s the seconds
	(between 0 and 59).

	Use requestVariableTiming() to periodically request the remaining time.
	\sa fixedTiming()
*/
	void variableTiming(int h, int m, int s);

	/*!
		\brief Requests a status update.

		It should never be necessary to call this function explicitly.
	*/
	void requestStatus();

	/*!
		\brief Requests the remaining time for a timed light.
	*/
	void requestVariableTiming();

	void setTimingBehaviour(Timed t);

	/*!
		\brief Used by the basic/advanced actuator detection.

		\sa PullDevice
	*/
	static FrameHandled isFrameHandled(OpenMsg &msg);

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
	virtual void requestPullStatus();

private:
	Timed timed_light;
};


/*!
	\ingroup Lighting
	\brief Manages a 10-level dimmer actuators.

	The metods allow to increase/decrease the dimmer level.  Use LightingDevice
	methods to turn on/off the dimmer.

	\section dimensions Dimensions
	\startdim
	\dim{DIM_DIMMER_PROBLEM,no value,,Set if there is a problem with the dimmer.}
	\dim{DIM_DIMMER_LEVEL,int,2-10,The 10-level dimmer level.}
	\enddim

	When the status list contains the DIM_DIMMER_LEVEL dimension, it will always
	contain the dimension DIM_DEVICE_ON with value \c true. The reverse holds nearly
	all the time, (except for the case when the device is turned on with a
	global/environment command before receiving any status update from the dimmer).
*/
class DimmerDevice : public LightingDevice
{
friend class TestDimmerDevice;
Q_OBJECT
public:
	DimmerDevice(QString where, PullMode pull = PULL_UNKNOWN, int openserver_id = 0, int pull_delay = PULL_DELAY_DIMMER10,
		     AdvancedMode adv = PULL_ADVANCED_UNKNOWN, PullStateManager::FrameChecker checker = &DimmerDevice::isFrameHandled);

	/*!
		\brief Increase dimmer 10 level (also turns on the device).
	*/
	void increaseLevel();

	/*!
		\brief Decrease dimmer 10 level (also turns on the device).
	*/
	void decreaseLevel();

	/*!
		\brief Used by the basic/advanced actuator detection.

		\see PullDevice
	*/
	static FrameHandled isFrameHandled(OpenMsg &msg);

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

protected slots:
	virtual void delayedLevelRequest();

protected:
	QTimer delayed_level_request;
	int level;
	bool status;
};


/*!
	\ingroup Lighting
	\brief Manages 100-level dimmer actuators.

	The methods allow to turn on/off the actuator and increase/decrease the dimmer
	level with a specified fade speed.

	\section dimensions Dimensions
	\startdim
	\dim{DIM_DIMMER100_LEVEL,int,1-100,The 100-level dimmer level.}
	\dim{DIM_DIMMER100_SPEED,int,,The 100-level dimmer on/off speed.}
	\enddim

	When the status list contains the DIM_DIMMER100_LEVEL dimension, it will always
	contain a DIM_DIMMER_LEVEL with a corresponding 10-level dimmer level (2-10).
*/
class Dimmer100Device : public DimmerDevice
{
friend class TestDimmer100Device;
Q_OBJECT
public:
	Dimmer100Device(QString where, PullMode pull = PULL_UNKNOWN, int openserver_id = 0, int pull_delay = PULL_DELAY_DIMMER100,
			PullStateManager::FrameChecker checker = NULL);

	virtual void init();

	/*!
		\brief Turn on the light with the given speed.
	*/
	void turnOn(int speed);

	/*!
		\brief Turn off the light with the given speed.
	*/
	void turnOff(int speed);

	/*!
		\brief Increase the dimmer level of the given \a delta and \a speed.
	*/
	void increaseLevel100(int delta, int speed);

	/*!
		\brief Decrease the dimmer level of the given \a delta and \a speed.
	*/
	void decreaseLevel100(int delta, int speed);

	/*!
		\brief Requests a status update.

		It should never be necessary to call this function explicitly.
	*/
	void requestDimmer100Status();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
	virtual void requestPullStatus();
	virtual void delayedLevelRequest();
};

#endif // LIGHTINGDEVICE_H
