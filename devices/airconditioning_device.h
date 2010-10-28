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


#ifndef AIRCONDITIONING_DEVICE_H
#define AIRCONDITIONING_DEVICE_H

#include "device.h"

/*!
	\ingroup AirConditioning
	\brief A common interface upon AirConditioningDevice and AdvancedAirConditioningDevice.
*/
class AirConditioningInterface
{
public:
	/*!
		\brief Switch off the split.
	*/
	virtual void turnOff() const = 0;

	/*!
		\brief Activate a scenario, sending a command \a what.
	*/
	virtual void activateScenario(const QString &what) const = 0;
};


/*!
	\ingroup AirConditioning
	\brief Manages the basic split device.
*/
class AirConditioningDevice : public device, public AirConditioningInterface
{
friend class TestAirConditioningDevice;
Q_OBJECT
public:
	AirConditioningDevice(QString where, int openserver_id = 0);

	/*!
		\brief Set the 'what' command \a off_cmd to switch off the split.
	*/
	void setOffCommand(QString off_cmd);

	virtual void turnOff() const;
	virtual void activateScenario(const QString &what) const;

private:
	QString off;

	void sendCommand(const QString &cmd) const;
};


/*!
	\ingroup AirConditioning
	\brief Manages the advanced split device.
*/
class AdvancedAirConditioningDevice : public device, public AirConditioningInterface
{
friend class TestAdvancedAirConditioningDevice;
Q_OBJECT
public:
	AdvancedAirConditioningDevice(QString where, int openserver_id = 0);

	enum Error
	{
		DIM_SETSTATUS_ERROR,
	};

	/*!
		\brief The mode of the split.
	*/
	enum Mode
	{
		MODE_OFF = 0,    /*!< Switch off the split. */
		MODE_WINTER = 1, /*!< Winter mode. */
		MODE_SUMMER = 2, /*!< Summer mode. */
		MODE_FAN = 3,    /*!< Fan mode. */
		MODE_DEHUM = 4,  /*!< Dehumidification mode. */
		MODE_AUTO = 5    /*!< Automatic mode */
	};

	/*!
		\brief The velocity of the split.
	*/
	enum Velocity
	{
		VEL_AUTO = 0,     /*!< Automatic speed. */
		VEL_MIN = 1,      /*!< Minimum speed. */
		VEL_MED = 2,      /*!< Medium speed. */
		VEL_MAX = 3,      /*!< Maximum speed. */
		VEL_SILENT = 4,   /*!< A speed to minimize the noise of the split. */
		VEL_INVALID,      /*!< None of the above values. */
	};

	/*!
		\brief The swing of the split.
	*/
	enum Swing
	{
		SWING_OFF = 0,   /*!< Switch on the swing */
		SWING_ON = 1,    /*!< Switch off the swing */
		SWING_INVALID,   /*!< None of the above values. */
	};

	/*!
		\brief The status for an advanced split.

		It is identified by an AdvancedAirConditioningDevice::Mode, a temperature,
		an AdvancedAirConditioningDevice::Velocity and an AdvancedAirConditioningDevice::Swing.
	*/
	struct Status
	{
		Status() : mode(MODE_OFF) { }
		Status(Mode m, int t, Velocity v, Swing s) : mode(m), temp(t), vel(v), swing(s) { }
		Mode mode;
		int temp;
		Velocity vel;
		Swing swing;
	};

	/*!
		\brief Request the status of the adavanced split.
	*/
	void requestStatus() const;

	/*!
		Set the status of the advanced split.
	*/
	void setStatus(Mode mode, int temp, Velocity vel, Swing swing) const;

	/*!
		Set the status of the advanced split.
	*/
	void setStatus(Status st) const;

	/*!
		\brief Switch off the split.
	*/
	virtual void turnOff() const;
	virtual void activateScenario(const QString &what) const;

	QString statusToString(const Status &st) const;

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	// contains the "what" of the last set status command; used for error detection
	mutable QString last_status_set;
};

Q_DECLARE_TYPEINFO(AdvancedAirConditioningDevice::Status, Q_MOVABLE_TYPE);

#endif // AIRCONDITIONING_DEVICE_H
