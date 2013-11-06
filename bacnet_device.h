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

#ifndef BACNET_DEVICE_H
#define BACNET_DEVICE_H

#include "device.h"

namespace Bacnet
{
	/*!
		\brief Invalid numeric value.

		Numeric value used as default value for variables to flag that the variable has not been written to.
	*/
	extern const int NOTSET_NUM_VALUE;


	/*!
		\brief The status of the BACNET device.
	*/
	enum Status
	{
		STATUS_NOTSET = -1,  /*!< Illegal status. This value flags that the status has not been set yet. */
		STATUS_INACTIVE = 0, /*!< The BACNET device is inactive. */
		STATUS_ACTIVE = 1    /*!< The BACNET device is active. */
	};


	/*!
		\brief The mode of the BACNET device.
	*/
	enum Mode
	{
		MODE_NOTSET = -1,           /*!< Illegal mode. This value flags that the mode has not been set yet. */
		MODE_INACTIVE = 0,          /*!< The BACNET device is in inactive mode. */
		MODE_WINTER = 1,            /*!< The BACNET device is in winter mode. */
		MODE_SUMMER = 2,            /*!< The BACNET device is in summer mode. */
		MODE_FAN = 3,               /*!< The BACNET device is in fan mode. */
		MODE_DRY = 4,               /*!< The BACNET device is in dry mode. */
		MODE_NORMAL_AUTO = 5,       /*!< The BACNET device is in normal auto mode. */
		MODE_FAST_AUTO = 6,         /*!< The BACNET device is in fast auto mode. */
		MODE_ENERGY_SAVING_AUTO = 7 /*!< The BACNET device is in energy saving auto mode. */
	};


	/*!
		\brief The speed setting of the BACNET device's fan.

		Used in AC Split with Fan Coil and Air Handling Unit BACNET devices.
	*/
	enum FanSpeed
	{
		SPEED_NOTSET = -1, /*!< Illegal fan speed. This value flags that the fan speed has not been set yet. */
		SPEED_AUTO = 0,    /*!< Automatic fan speed control. */
		SPEED_SLOW = 1,    /*!< Slow fan speed. */
		SPEED_NORMAL = 2,  /*!< Normal fan speed. */
		SPEED_FAST = 3,    /*!< Fast fan speed. */
		SPEED_SILENT = 4   /*!< Silent fan speed. */
	};


	/*!
		\ingroup BACNET
		\brief An interface that adds support for turning on and off a BACNET device.
	*/
	class BacnetDeviceInterface
	{
	public:
		/*!
			\brief Turn off the device.
		*/
		virtual void turnOff() const = 0;

		/*!
			\brief Turn on the device.
		*/
		virtual void turnOn() const = 0;
	};


	/*!
		\ingroup BACNET
		\brief A class for managing AC Split with Fan Coil BACNET devices.

		The metods allow to set or request the status of the device.

		The following parameters can be written when setting the status:
		- Actual room temperature setpoint
		- Status
		- Working mode
		- Fan speed
		- Air direction
		- Reset filter fault

		The following parameters can be read when requesting the status:
		- Actual room temperature setpoint
		- Status
		- Working mode
		- Fan speed
		- Air direction
		- Reset filter fault
		- Actual room temperature
		- Generic fault
		- Fault description

		\section ACSplitWithFanCoilDevice-dimensions Dimensions
		\startdim
		\dim{DIM_AC_FANCOIL,ACSplitWithFanCoilDevice::State,,AC Split with Fan Coil BACNET device's status dimension.}
		\enddim
	*/
	class ACSplitWithFanCoilDevice : public device, public BacnetDeviceInterface
	{
		friend class TestACSplitWithFanCoilDevice;
		Q_OBJECT
	public:
		/*!
			\refdim{ACSplitWithFanCoilDevice}
		*/
		enum Type
		{
			DIM_AC_FANCOIL = 51
		};


		/*!
			\brief The direction of the air.
		*/
		enum AirDirection
		{
			DIRECTION_NOTSET = -1,      /*!< Illegal air direction. This value flags that the air direction has not been set yet. */
			DIRECTION_AUTO = 0,         /*!< Automatic air direction control. */
			DIRECTION_FORWARD = 1,      /*!< Forward air direction. */
			DIRECTION_BACKWARD = 2,     /*!< Backward air direction. */
			DIRECTION_RANDOM = 3,       /*!< Random air direction. */
			DIRECTION_SLOW_CYCLING = 4, /*!< Slow cycling air direction. */
			DIRECTION_CYCLING = 5,      /*!< Cycling air direction. */
			DIRECTION_FAST_CYCLING = 6, /*!< Fast cycling air direction. */
			DIRECTION_STOP = 7          /*!< Air direction is stopped. */
		};


		/*!
			\brief A helper structure that packs all AC Split with Fan Coil BACNET device's parameters together.

			It is used for communicating the device's parameters with its UI.
			It is identified by the following parameters:
			- Temperature setpoint
			- Status
			- Mode
			- Fan speed
			- Air direction
			- Reset filter fault
		*/
		struct State
		{
			/*!
				\brief Constructs an invalid status for the AC Split with Fan Coil device.
			*/
			State();


			/*!
				\brief Constructs a status for the AC Split with Fan Coil device given the:
				- Temperature setpoint (\a temperature_set_point)
				- Mode (\a mode)
				- Fan speed (\a speed)
				- Air direction (\a direction)
				- Filter fault (\a filter_fault)
			*/
			State( int temp_set_point,
				   Status status,
				   Mode mode,
				   FanSpeed speed,
				   AirDirection direction,
				   bool filter_fault);

			/*!
				\brief Compares two statuses for equality.
			*/
			bool operator==(const State &other) const;

			/*!
				\brief Compares two statuses for inequality.
			*/
			bool operator!=(const State &other) const;

			/*!
				\brief The setpoint temperature of the AC Split with Fan Coil.
			*/
			int temperature_set_point;

			/*!
				\brief The status of the AC Split with Fan Coil.
			*/
			Status status;

			/*!
				\brief The operating mode of the AC Split with Fan Coil.
			*/
			Mode mode;

			/*!
				\brief The fan speed of the AC Split with Fan Coil.
			*/
			FanSpeed fan_speed;

			/*!
				\brief The air direction of the AC Split with Fan Coil.
			*/
			AirDirection air_direction;

			/*!
				\brief The reset filter fault flag of the AC Split with Fan Coil.
			*/
			bool reset_filter_fault;

			/*!
				\brief The temperature of the AC Split with Fan Coil.

				The value ranges from 0-1270 which equals to 0.0 to 127.0 degrees.
			*/
			int temperature;

			/*!
				\brief A flag indicating whether there is a fault in the AC Split with Fan Coil.
			*/
			bool hasFault;

			/*!
				\brief The description of the fault of the AC Split with Fan Coil.
			*/
			int faultDescription;
		};

		/*!
			\brief Constructs a AC Split with Fan Coil device, given the device's address (\a where) and an optional Open Web Net server id (\a openserver_id).
		*/
		ACSplitWithFanCoilDevice(QString where, int openserver_id = 0);

		/*!
			\brief Requests the AC Split with Fan Coil device's status.
		*/
		void requestStatus() const;

		/*!
			\brief Sets the status of the AC Split with Fan Coil device.

			Sets the status provided:
			- Temperature setpoint (\a temperature_set_point)
			- Status (\a status)
			- Mode (\a mode)
			- Fan speed (\a speed)
			- Air direction (\a direction)
			- Reset filter fault (\a filter_fault)
		*/
		void setStatus(
				int temperature_set_point,
				Status status,
				Mode mode,
				FanSpeed fan_speed,
				AirDirection air_direction,
				bool reset_filter_fault) const;

		/*!
			\brief Sets the status of the AC Split with Fan Coil device using an already built status.
		*/
		void setStatus(const State &state) const;

		/*!
			\brief Initialize the device.
		*/
		virtual void init();

		/*!
			\brief Turn off the device.
		*/
		virtual void turnOff() const;

		/*!
			\brief Turn on the device.
		*/
		virtual void turnOn() const;

		/*!
			\brief Turn on the reset filter fault for the current device.
		*/
		virtual void resetFilterFault() const;

		/*!
			\brief Translate the status parameters kept in \a state to a Open Web Net protocol message.
		*/
		QString statusToWhat(const State &state) const;

	protected:
		virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
	};


	/*!
		\ingroup BACNET
		\brief A class for managing Air Handling Unit BACNET devices.

		The metods allow to set or request the status of the device.

		The following parameters can be written when setting the status:
		- Room temperature setpoint
		- Status
		- Working mode
		- Fan speed
		- Air direction
		- Filter fault reset

		The following parameters can be read when requesting the status:
		- Actual room temperature setpoint
		- Automation status
		- Automation working mode
		- Fan speed
		- Air direction
		- Filter fault
		- Actual room temperature
		- Generic fault
		- Fault description

		\section AirHandlingDevice-dimensions Dimensions
		\startdim
		\dim{DIM_AHU,AirHandlingDevice::State,,Air Handling Unit BACNET device's status dimension.}
		\enddim
	*/
	class AirHandlingDevice : public device, public BacnetDeviceInterface
	{
		friend class TestAirHandlingDevice;
		Q_OBJECT
	public:
		/*!
			\refdim{AirHandlingDevice}
		*/
		enum Type
		{
			DIM_AHU = 52
		};


		/*!
			\brief A helper structure that packs all Air Handling Unit BACNET device's parameters together.

			It is used for communicating the device's parameters with its UI.
			It is identified by the following parameters:
			- Actual room temperature setpoint
			- Automation status
			- Automation working mode
			- Room relative humidity percentage setpoint
			- Air flow percentage setpoint
			- Min air flow limit
			- Max air flow limit
			- Recirculation percentage
			- Fan speed
			- Actual room temperature
			- A flag indicating whether there is generic fault
			- A description of the generic fault
			- Actual room relative humidity percentage
			- Air flow percentage
			- Air quality probe (pollution percentage)
			- Anti-freezer alarm
		*/
		struct State
		{
			/*!
				\brief Constructs an invalid status for the Air Handling Unit BACNET device.
			*/
			State();

			/*!
				\brief Constructs a status for the Air Handling Unit device given the:
				- Temperature setpoint (\a temperature_set_point)
				- Status (\a status)
				- Mode (\a mode)
				- Room relative humidity percentage setpoint (\a humidity_set_point)
				- Air flow percentage setpoint (\a air_flow_set_point)
				- Min air flow limit (\a min_air_flow)
				- Max air flow limit (\a max_air_flow)
				- Recirculation percentage (\a recirculation)
				- Fan speed (\a fan_speed)
			*/
			State(	int temperature_set_point,
					Status status,
					Mode mode,
					int humidity_set_point,
					int air_flow_set_point,
					int min_air_flow,
					int max_air_flow,
					int recirculation,
					FanSpeed fan_speed);

			/*!
				\brief Compares two statuses for equality.
			*/
			bool operator==(const State &other) const;

			/*!
				\brief Compares two statuses for inequality.
			*/
			bool operator!=(const State &other) const;

			/*!
				\brief Actual room temperature setpoint.
			*/
			int temperature_set_point;

			/*!
				\brief Automation status.
			*/
			Status status;

			/*!
				\brief Automation working mode.
			*/
			Mode mode;

			/*!
				\brief Room relative humidity percentage setpoint.
			*/
			int humidity_set_point;

			/*!
				\brief Air flow percentage setpoint.
			*/
			int air_flow_set_point;

			/*!
				\brief Min air flow limit.
			*/
			int min_air_flow;

			/*!
				\brief Max air flow limit.
			*/
			int max_air_flow;

			/*!
				\brief Recirculation percentage.
			*/
			int recirculation;

			/*!
				\brief Fan speed.
			*/
			FanSpeed fan_speed;

			/*!
				\brief Actual room temperature.
			*/
			int temperature;

			/*!
				\brief A flag indicating whether there is generic fault.
			*/
			bool hasFault;

			/*!
				\brief A description of the fault.
			*/
			int faultDescription;

			/*!
				\brief Actual room relative humidity percentage.
			*/
			int humidity;

			/*!
				\brief Air flow percentage.
			*/
			int airFlow;

			/*!
				\brief Air quality probe (pollution percentage).
			*/
			int airQuality;

			/*!
				\brief Anti-freezer alarm.
			*/
			bool isAlarmActive;
		};

		/*!
			\brief Constructs a Air Handling Unit BACNET device, given the device's address (\a where) and an optional Open Web Net server id (\a openserver_id).
		*/
		AirHandlingDevice(QString where, int openserver_id = 0);

		/*!
			\brief Requests the Air Handling Unit device's status.
		*/
		void requestStatus() const;

		/*!
			\brief Sets the status of the Air Handling Unit device by providing all parameters individually.
		*/
		void setStatus(
				int temperature_set_point,
				Status status,
				Mode mode,
				int humidity_set_point,
				int air_flow_set_point,
				int min_air_flow,
				int max_air_flow,
				int recirculation,
				FanSpeed fan_speed) const;

		/*!
			\brief Sets the status of the Air Handling Unit device by providing all parameters packed in the \struct State structure.
		*/
		void setStatus(const State &state) const;

		/*!
			\brief Initialize the device.
		*/
		virtual void init();

		/*!
			\brief Turn off the device.
		*/
		virtual void turnOff() const;

		/*!
			\brief Turn on the device.
		*/
		virtual void turnOn() const;

		/*!
			\brief Translate the status parameters kept in \a state to a Open Web Net protocol message.
		*/
		QString statusToWhat(const State &state) const;

	protected:
		virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
	};


	/*!
		\ingroup BACNET
		\brief A class for managing Variable Air Volume Unit BACNET devices.

		The metods allow to set or request the status of the device.

		The following parameters can be written when setting the status:
		- Room temperature setpoint
		- Status
		- Working mode
		- Air flow setpoint

		The following parameters can be read when requesting the status:
		- Actual room temperature setpoint
		- Automation status
		- Automation working mode
		- Air flow setpoint
		- Actual room temperature
		- Generic fault
		- Fault description

		\section VariableAirVolumeDevice-dimensions Dimensions
		\startdim
		\dim{DIM_VAV,VariableAirVolumeDevice::State,,Variable Air Volume Unit BACNET device's status dimension.}
		\enddim
	*/
	class VariableAirVolumeDevice : public device, public BacnetDeviceInterface
	{
		friend class TestVariableAirVolumeDevice;
		Q_OBJECT
	public:
		/*!
			\refdim{VariableAirVolumeDevice}
		*/
		enum Type
		{
			DIM_VAV = 53
		};


		/*!
			\brief A helper structure that packs all Variable Air Volume Unit BACNET device's parameters together.

			It is used for communicating the device's parameters with its UI.
			It is identified by the following parameters:
			- Actual Room Temperature Setpoint
			- Automation Status
			- Automation Working Mode
			- Air Flow Setpoint
			- Actual Room Temperature
			- A flag indicating whether there is a generic fault
			- A description of the generic fault
		*/
		struct State
		{
			/*!
				\brief Constructs an invalid status for the Variable Air Volume Unit device.
			*/
			State();

			/*!
				\brief Constructs a status for the Variable Air Volume Unit BACNET device given the:
				- Temperature setpoint (\a temperature_set_point)
				- Status (\a status)
				- Mode (\a mode)
				- Air flow setpoint (\a air_flow_set_point)
			*/
			State(	int temperature_set_point,
					Status status,
					Mode mode,
					int air_flow_set_point);

			/*!
				\brief Compares two statuses for equality.
			*/
			bool operator==(const State &other) const;

			/*!
				\brief Compares two statuses for inequality.
			*/
			bool operator!=(const State &other) const;

			/*!
				\brief Actual room temperature setpoint.
			*/
			int temperature_set_point;

			/*!
				\brief Automation status.
			*/
			Status status;

			/*!
				\brief Automation working mode.
			*/
			Mode mode;

			/*!
				\brief Air flow percentage setpoint.
			*/
			int air_flow_set_point;

			/*!
				\brief Actual room temperature.
			*/
			int temperature;

			/*!
				\brief A flag indicating whether there is generic fault.
			*/
			bool hasFault;

			/*!
				\brief A description of the generic fault.
			*/
			int faultDescription;
		};

		/*!
			\brief Constructs a Variable Air Volume Unit BACNET device, given the device's address (\a where) and an optional Open Web Net server id (\a openserver_id).
		*/
		VariableAirVolumeDevice(QString where, int openserver_id = 0);

		/*!
			\brief Requests the Variable Air Volume Unit device's status.
		*/
		void requestStatus() const;

		/*!
			\brief Sets the status of the Variable Air Volume Unit device by providing all parameters individually.
		*/
		void setStatus(
				int temperature_set_point,
				Status status,
				Mode mode,
				int air_flow_set_point) const;

		/*!
			\brief Sets the status of the Variable Air Volume Unit device by providing all parameters packed in the \struct State structure.
		*/
		void setStatus(const State &state) const;

		/*!
			\brief Initialize the device.
		*/
		virtual void init();

		/*!
			\brief Turn off the device.
		*/
		virtual void turnOff() const;

		/*!
			\brief Turn on the device.
		*/
		virtual void turnOn() const;

		/*!
			\brief Translate the status parameters kept in \a state to a Open Web Net protocol message.
		*/
		QString statusToWhat(const State &state) const;

	protected:
		virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
	};


	/*!
		\ingroup BACNET
		\brief A class for managing Generic and Underfloor Heating Unit BACNET devices.

		The metods allow to set or request the status of the device.

		The following parameters can be written when setting the status:
		- Room temperature setpoint
		- Status
		- Working mode

		The following parameters can be read when requesting the status:
		- Actual room temperature setpoint
		- Automation status
		- Automation working mode
		- Actual room temperature
		- A flag indicating whether there is a generic fault
		- A description of the generic fault

		\section UnderfloorHeatingDevice-dimensions Dimensions
		\startdim
		\dim{DIM_UH,UnderfloorHeatingDevice::State,,Generic and Underfloor Heating Unit BACNET device's status dimension.}
		\enddim
	*/
	class UnderfloorHeatingDevice : public device, public BacnetDeviceInterface
	{
		friend class TestUnderfloorHeatingDevice;
		Q_OBJECT
	public:
		/*!
			\refdim{UnderfloorHeatingDevice}
		*/
		enum Type
		{
			DIM_UH = 50
		};


		/*!
			\brief A helper structure that packs all Generic and Underfloor Heating Unit BACNET device's parameters together.

			It is used for communicating the device's parameters with its UI.
			It is identified by the following parameters:
			- Actual room temperature setpoint
			- Automation status
			- Automation working mode
			- Actual room temperature
			- A flag indicating whether there is a generic fault
			- A description of the generic fault
		*/
		struct State
		{
			/*!
				\brief Constructs an invalid status for the Generic and Underfloor Heating Unit device.
			*/
			State();

			/*!
				\brief Constructs a status for the Generic and Underfloor Heating Unit device given the:
				- Actual room temperature setpoint (\a temperature_set_point)
				- Automation status (\a status)
				- Automation working mode (\a mode)
		   */
			State(	int temperature_set_point,
					Status status,
					Mode mode);

			/*!
				\brief Compares two statuses for equality.
			*/
			bool operator==(const State &other) const;

			/*!
				\brief Compares two statuses for inequality.
			*/
			bool operator!=(const State &other) const;

			/*!
				\brief Actual room temperature setpoint.
			*/
			int temperature_set_point;

			/*!
				\brief Automation status.
			*/
			Status status;

			/*!
				\brief Automation working mode.
			*/
			Mode mode;

			/*!
				\brief Actual room temperature.
			*/
			int temperature;

			/*!
				\brief A flag indicating whether there is a generic fault.
			*/
			bool hasFault;

			/*!
				\brief A description of the generic fault.
			*/
			int faultDescription;
		};

		/*!
			\brief Constructs a Generic and Underfloor Heating Unit BACNET device, given the device's address (\a where) and an optional Open Web Net server id (\a openserver_id).
		*/
		UnderfloorHeatingDevice(QString where, int openserver_id = 0);

		/*!
			\brief Requests the Generic and Underfloor Heating Unit device's status.
		*/
		void requestStatus() const;

		/*!
			\brief Sets the status of the Generic and Underfloor Heating Unit device.

			Sets the status provided:
			- Temperature setpoint (\a temperature_set_point)
			- Status (\a status)
			- Mode (\a mode)
		*/
		void setStatus(int temperature_set_point, Status status, Mode mode) const;

		/*!
			\brief Sets the status of the Generic and Underfloor Heating Unit device using an already built status.
		*/
		void setStatus(const State &state) const;

		/*!
			\brief Initialize the device.
		*/
		virtual void init();

		/*!
			\brief Turn off the device.
		*/
		virtual void turnOff() const;

		/*!
			\brief Turn on the device.
		*/
		virtual void turnOn() const;

		/*!
			\brief Translate the status parameters kept in \a state to a Open Web Net protocol message.
		*/
		QString statusToWhat(const State &state) const;

	protected:
		virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);
	};

}

Q_DECLARE_METATYPE(Bacnet::ACSplitWithFanCoilDevice::State)
Q_DECLARE_TYPEINFO(Bacnet::ACSplitWithFanCoilDevice::State, Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(Bacnet::AirHandlingDevice::State)
Q_DECLARE_TYPEINFO(Bacnet::AirHandlingDevice::State, Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(Bacnet::VariableAirVolumeDevice::State)
Q_DECLARE_TYPEINFO(Bacnet::VariableAirVolumeDevice::State, Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(Bacnet::UnderfloorHeatingDevice::State)
Q_DECLARE_TYPEINFO(Bacnet::UnderfloorHeatingDevice::State, Q_MOVABLE_TYPE);

#endif // BACNET_DEVICE_H
