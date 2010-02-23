#ifndef THERMAL_DEVICE_H
#define THERMAL_DEVICE_H

#include "device.h"

class BtTime;


enum thermo_type_t
{
	THERMO_Z99,  // 99 zones thermal regulator
	THERMO_Z4,   // 4 zones thermal regulator
};


class ThermalDevice : public device
{
friend class TestThermalDevice;
friend class TestThermalDevice4Zones;
friend class TestThermalDevice99Zones;
Q_OBJECT
public:

	enum Type
	{
		DIM_STATUS = 0, // see Status enum
		DIM_SEASON = 1, // see Season enum
		DIM_PROGRAM = 2, // program number
		DIM_SCENARIO = 3, // scenario number
		DIM_TEMPERATURE = 4 // temperature
	};

	enum Status
	{
		ST_OFF = 0,
		ST_PROTECTION = 1,
		ST_MANUAL = 2,
		ST_MANUAL_TIMED = 3,
		ST_WEEKEND = 4,
		ST_PROGRAM = 5,
		ST_SCENARIO = 6,
		ST_HOLIDAY = 7,
		ST_COUNT // must be the last
	};

	enum Season
	{
		SE_SUMMER = 0,
		SE_WINTER = 1
	};

	void setOff();
	void setSummer();
	void setWinter();
	void setProtection();
	void setHolidayDateTime(QDate date, BtTime time, int program);

	/**
	 * Function to set weekend mode (giorno festivo) with end date and time, and program to be executed at the end of weekend mode.
	 * \param date The end date of weekend mode.
	 * \param time The end time of weekend mode.
	 * \param program The program to be executed at the end of weekend mode.
	 */
	void setWeekendDateTime(QDate date, BtTime time, int program);
	void setWeekProgram(int program);

	/**
	 * Sends a frame to the thermal regulator to set the temperature in manual mode.
	 * The frame sent is generic (not winter nor summer).
	 * \param temperature The temperature to be set, ranges from 50 to 400, step is 5.
	 */
	void setManualTemp(unsigned temperature);

	/**
	 * Getter methods that return the maximum temperature allowed by the thermal regulator.
	 * \return The maximum temperature allowed by the thermal regulator, in 10th of Celsius degrees.
	 */
	virtual unsigned maximumTemp() const = 0;
	/**
	 * Getter method that return the minimum temperature allowed by the thermal regulator.
	 * \return The minimum temperature allowed by the thermal regulator , in 10th of Celsius degrees.
	 */
	virtual unsigned minimumTemp() const;

	/**
	 * Getter method for thermal regulator type.
	 * \return The type of thermal regulator.
	 */
	virtual thermo_type_t type() const = 0;

protected:
	ThermalDevice(QString where, int openserver_id);

	void sendWriteRequest(const QString &what);
	virtual void manageFrame(OpenMsg &msg);

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


class ThermalDevice4Zones : public ThermalDevice
{
Q_OBJECT
public:
	ThermalDevice4Zones(QString where, int openserver_id = 0);

	/**
	 * Sets the temperature for a limited time.
	 * \param temperature The temperature to be set
	 * \param time The duration of the manual setting (24 hours max?)
	 */
	void setManualTempTimed(int temperature, BtTime time);

	virtual unsigned maximumTemp() const;
	virtual thermo_type_t type() const;
};


class ThermalDevice99Zones : public ThermalDevice
{
Q_OBJECT
public:
	ThermalDevice99Zones(QString where, int openserver_id = 0);

	/**
	 * Sets the scenario on the thermal regulator.
	 * \param scenario The scenario to be activated (16 max).
	 */
	void setScenario(int scenario);

	virtual unsigned maximumTemp() const;
	virtual thermo_type_t type() const;
};

#endif // THERMAL_DEVICE_H
