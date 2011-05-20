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


#ifndef PROBE_DEVICE_H
#define PROBE_DEVICE_H

#include "device.h"


/*!
	\ingroup ThermalRegulation
	\brief Manages extrenal temperature sensors and non-controlled zones.

	It receives temperature updates from the various measurement-only sensors.

	\section NonControlledProbeDevice-dimensions Dimensions
	\startdim
	\dim{DIM_TEMPERATURE,int,,Temperature in BTicino 4-digit format.}
	\enddim

	\see ::bt2Celsius()
	\see ::bt2Fahrenheit()
 */
class NonControlledProbeDevice : public device
{
friend class TestNonControlledProbeDevice;
friend class TestExternalProbeDevice;
Q_OBJECT
public:
	/*!
		\brief Probe type.
	 */
	enum ProbeType
	{
		INTERNAL = 0, /*!< Sensor for a non-controlled zone. */
		EXTERNAL = 1, /*!< External, radio-operated sensor. */
	};

	/*!
		\refdim{NonControlledProbeDevice}
	 */
	enum Type
	{
		DIM_TEMPERATURE = 1,
	};

	/*!
		\brief Constructor.
	 */
	NonControlledProbeDevice(QString where, ProbeType type, int openserver_id = 0);

	/*!
		\brief Request a temperature update.

		It should never be necessary to call this function explicitly.
	 */
	void requestStatus();
	virtual void init() { requestStatus(); }

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	ProbeType type;
};


/*!
	\ingroup ThermalRegulation
	\brief Manages temperature sensors associated to a zone.

	For all zones, allows reading current temperature and the temperature offset
	for the zone.

	For zones with a fan coil allows changing the speed of the fan coil.

	For the zones of a 99-zones central, allows changing zone mode
	(manual, automatic, ...) and target temperature (for manual mode).

	\section ControlledProbeDevice-dimensions Dimensions
	\startdim
	\dim{DIM_TEMPERATURE,int,,Temperature in BTicino 4-digit format.}
	\dim{DIM_SETPOINT,int,,Target temperature for the zone in BTicino 4-digit format.}
	\dim{DIM_STATUS,ControlledProbeDevice::ProbeStatus,,Zone setting set using the control unit.}
	\dim{DIM_LOCAL_STATUS,ControlledProbeDevice::ProbeStatus,ControlledProbeDevice::ST_NORMAL / ControlledProbeDevice::ST_OFF / ControlledProbeDevice::ST_PROTECTION,
	     Local setting of the zone (set using the physical knob on the probe.}
	\dim{DIM_OFFSET,int,-3/+3,Local offset of the zone (set using the physical knob on the probe.}
	\dim{DIM_FANCOIL_STATUS,int,1: min; 2:med; 3:max; 4:automatic,Fan coil speed.}
	\enddim

	All status updates contain DIM_STATUS, DIM_LOCAL_STATUS, DIM_OFFSET and (if != 0) DIM_SETPOINT.

	\see ::bt2Celsius()
	\see ::bt2Fahrenheit()
 */
class ControlledProbeDevice : public device
{
friend class TestControlledProbeDevice;
Q_OBJECT
public:
	enum ProbeType
	{
		NORMAL = 0,  /*!< Zone without fan coil. */
		FANCOIL = 1, /*!< Zone with fan coil. */
	};

	enum CentralType
	{
		CENTRAL_99ZONES = 0, /*!< Zone controlled by a 99-zone central. */
		CENTRAL_4ZONES = 1,  /*!< Zone controlled by a 4-zone central. */
	};

	/*!
		\refdim{ControlledProbeDevice}
	 */
	enum Type
	{
		DIM_FANCOIL_STATUS = 1,
		DIM_TEMPERATURE = 2,
		DIM_STATUS = 3,
		DIM_SETPOINT = 4,
		DIM_OFFSET = 5,
		DIM_LOCAL_STATUS = 6,
	};

	enum ProbeStatus
	{
		ST_NONE = 0,        /*!< No state received yet (only during initialization). */
		ST_NORMAL = 0,      /*!< Only for DIM_LOCAL_STATUS: zone local offset set. */
		ST_MANUAL = 1,      /*!< Manual mode. */
		ST_AUTO = 2,        /*!< Automatic mode. */
		ST_OFF = 3,         /*!< Zone off. */
		ST_PROTECTION = 4,  /*!< Antifreeze mode. */
	};

	/*!
		\brief Constructor
		\param where complete probe address (probe#central, without leading #)
		\param central_where complete central address
		\param simple_where probe address (without leading #)
		\param central_type type of the controlling central
		\param type probe with and without fancoil
	 */
	ControlledProbeDevice(QString where, QString central_where, QString simple_where, CentralType central_type, ProbeType type, int openserver_id = 0);

	virtual void init() { requestStatus(); }

	/*!
		\brief Set fancoil speed.

		Speeds:
		- 1: minimum
		- 2: medium
		- 3: maximum
		- 4: automatic
	 */
	void setFancoilSpeed(int speed);
	void requestFancoilStatus();

public slots:
	/*!
		\brief Set zone in manual mode with the given target temperature (99-zone controller).
	*/
	void setManual(unsigned setpoint);

	/*!
		\brief Set zone in automatic mode (99-zone controller).
	*/
	void setAutomatic();

	/*!
		\brief Set zone to antifreeze mode (99-zone controller).
	*/
	void setProtection();

	/*!
		\brief Set zone to off mode (99-zone controller).
	*/
	void setOff();

protected:
	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private slots:
	void requestSetpoint();

private:
	QString simple_where;
	ProbeType type;
	CentralType central_type;
	bool has_central_info;
	QString central_where;
	bool setpoint_need_update;

	// device state
	ProbeStatus status;
	ProbeStatus local_status;
	int local_offset, set_point;

	void requestStatus();

	static const int TIMEOUT_TIME = 10000;
};

#endif // PROBE_DEVICE_H
