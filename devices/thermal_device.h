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


#ifndef THERMAL_DEVICE_H
#define THERMAL_DEVICE_H

#include "device.h"
#include "bttime.h"


enum ThermoType
{
	THERMO_Z99,  // 99 zones thermal regulator
	THERMO_Z4,   // 4 zones thermal regulator
};


/*!
	\ingroup ThermalRegulation
	\brief Common part of ThermalDevice4Zones and ThermalDevice99Zones

	Operations and status updates common to 4-zone and 99-zone thermal regulator.

	\section ThermalDevice-dimensions Dimensions
	\startdim
	\dim{DIM_STATUS,ThermalDevice::Status,,Thermal regulator mode.}
	\dim{DIM_SEASON,ThermalDevice::Season,,Thermal regulator season.}
	\dim{DIM_PROGRAM,int,,Program number; sent when DIM_STATUS is ST_PROGRAM or ST_WEEKEND; not available for ST_HOLIDAY.}
	\dim{DIM_SCENARIO,int,,%Scenario number (99-zone); sent when DIM_STATUS is ST_SCENARIO.}
	\dim{DIM_TEMPERATURE,int,,Temperature for manual modes in BTicino 4-digit format.}
	\dim{DIM_DATE,QDate,,End date for weekend/holiday mode.}
	\dim{DIM_TIME,QTime,,End time for weekend/holiday mode.}
	\dim{DIM_DURATION,BtTime,,Duration for timed manual mode (can be any value between 0 and 24 hours, 59 minutes).}
	\enddim
 */
class ThermalDevice : public device
{
friend class TestThermalDevice;
friend class TestThermalDevice4Zones;
friend class TestThermalDevice99Zones;
Q_OBJECT
public:
	/*!
		\refdim{ThermalDevice}
	 */
	enum Type
	{
		DIM_STATUS = 0,
		DIM_SEASON = 1,
		DIM_PROGRAM = 2,
		DIM_SCENARIO = 3,
		DIM_TEMPERATURE = 4,
		DIM_DATE = 5,
		DIM_TIME = 6,
		DIM_DURATION = 7,
	};

	enum Status
	{
		ST_OFF = 0,          /*!< Off. */
		ST_PROTECTION = 1,   /*!< Antifreeze mode. */
		ST_MANUAL = 2,       /*!< Manual temperature mode. */
		ST_MANUAL_TIMED = 3, /*!< Timed manual temperature mode (4-zone). */
		ST_WEEKEND = 4,      /*!< Week-end mode (preset program until a certain date). */
		ST_PROGRAM = 5,      /*!< Preset program. */
		ST_SCENARIO = 6,     /*!< Preset scenario (99-zone). */
		ST_HOLIDAY = 7,      /*!< Antifreeze mode until a certain date, then switch to preset program. */
		ST_COUNT,            /*!< Internal. */
	};

	enum Season
	{
		SE_SUMMER = 0,       /*!< Summer mode. */
		SE_WINTER = 1,       /*!< Winter mode. */
	};

	virtual void init();

	/*!
		\brief Switch the termal regulator to off mode.
	 */
	void setOff();

	/*!
		\brief Switch to summer mode.
	 */
	void setSummer();

	/*!
		\brief Switch to winter mode.
	 */
	void setWinter();

	/*!
		\brief Switch to antifreeze mode.
	 */
	void setProtection();

	/*!
		\brief Switch to holiday mode.

		Go to antifreeze mode until the specified date/time, then switch to the given program.
	 */
	void setHolidayDateTime(QDate date, BtTime time, int program);

	/*!
		\brief Switch to weekend mode.

		Run the given program until the specified date/time, then switch to the previously-selected weekly program.
	 */
	void setWeekendDateTime(QDate date, BtTime time, int program);

	/*!
		\brief Switch to preset program mode.
	 */
	void setWeekProgram(int program);

	/*!
		\brief Switch to manual mode.
		\param temperature The temperature to be set in BTicino 4-digit format.

		\see maximumTemp()
		\see minimumTemp()
	 */
	void setManualTemp(unsigned temperature);

	/*!
		\brief Maximum temperature allowed by the thermal regulator in BTicino 4-digit format.
	 */
	virtual unsigned maximumTemp() const = 0;

	/*!
		\brief Minimum temperature allowed by the thermal regulator in BTicino 4-digit format.
	 */
	virtual unsigned minimumTemp() const;

	/*!
		\brief Returns the type of thermal regulator (4-zone or 99-zone).
	 */
	virtual ThermoType type() const = 0;

protected:
	ThermalDevice(QString where, int openserver_id);

	void sendWriteRequest(const QString &what);
	bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	/**
	 * Utility function to set end date for both holiday and weekend mode
	 * \param date The end date of the mode.
	 */
	void setHolidayEndDate(QDate date);

	/**
	 * Utility function to set end time for both holiday and weekend mode
	 * \param time The end time of the mode.
	 */
	void setHolidayEndTime(BtTime time);

	/**
	 * Utility function to check if `what' is a program or scenario command. This type of commands are in this form:
	 * xynn (where x={1,2}, y={1,2,3}) where nn is the program or scenario command, or xynnn (where x={1,2}, y={1,2,3}) and
	 * nnn are the number of days.
	 * We need to take action based on xy00-type of command, this function returns the command in that form.
	 *
	 * \param what  The command to be checked
	 * \return The command in xy00 form.
	 */
	int commandRange(int what);
};

Q_DECLARE_METATYPE(ThermalDevice::Season)
Q_DECLARE_METATYPE(ThermalDevice::Status)

/*!
	\ingroup ThermalRegulation
	\brief Manages 4-zone thermal regulator
 */
class ThermalDevice4Zones : public ThermalDevice
{
Q_OBJECT
friend class TestThermalDevice4Zones;
public:
	ThermalDevice4Zones(QString where, int openserver_id = 0);

	/*!
		\brief Switch to timed manual mode.
		\param temperature The temperature to be set
		\param time The duration of the manual setting (24 hours max)
	 */
	void setManualTempTimed(int temperature, BtTime time);

	virtual unsigned maximumTemp() const;
	virtual ThermoType type() const;

private slots:
	void setEndTime();

private:
	BtTime end_time;
	static int temp_timed_delay;
};


/*!
	\ingroup ThermalRegulation
	\brief Manages 99-zone thermal regulator
 */
class ThermalDevice99Zones : public ThermalDevice
{
Q_OBJECT
public:
	ThermalDevice99Zones(QString where, int openserver_id = 0);

	/*!
		\brief Switch to preset scenario mode.
	 */
	void setScenario(int scenario);

	virtual unsigned maximumTemp() const;
	virtual ThermoType type() const;
};

#endif // THERMAL_DEVICE_H
