#ifndef THERMAL_DEVICE_H
#define THERMAL_DEVICE_H

#include "device.h"

class BtTime;


class ThermalDevice : public device
{
Q_OBJECT
public:
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

	enum what_t
	{
		SUMMER = 0,
		WINTER = 1,
		GENERIC_MODE = 3,
		TEMPERATURE_SET = 14,            // set the temperature in manual operation, this is a dimension
		HOLIDAY_DATE_END = 30,           // set the end date of holiday mode, this is a dimension (grandezza)
		HOLIDAY_TIME_END = 31,           // set the end time of holiday mode, this is a dimension (grandezza)
		MANUAL_TIMED_END = 32,           // set the end time of timed manual mode

		// summer specific identifiers
		SUM_PROTECTION = 202,            // protection
		SUM_OFF = 203,                   // off
		SUM_MANUAL = 210,                // manual operation (all zones in setpoint temperature)
		SUM_MANUAL_TIMED = 212,          // manual operation (24h maximum)
		SUM_WEEKEND = 215,               // weekend operation (festivo)
		SUM_PROGRAM = 2100,              // weekly program (1 out of 3)
		SUM_SCENARIO = 2200,             // scenario (1 out of 16, 99zones thermal regulator only)
		SUM_HOLIDAY = 23000,             // holiday operation (programma ferie)

		// winter specific identifiers
		WIN_PROTECTION = 102,
		WIN_OFF = 103,                   // off
		WIN_MANUAL = 110,                // manual operation (all zones in setpoint temperature)
		WIN_MANUAL_TIMED = 112,          // manual operation (24h maximum)
		WIN_WEEKEND = 115,               // weekend operation (festivo)
		WIN_PROGRAM = 1100,              // weekly program (1 out of 3)
		WIN_SCENARIO = 1200,             // scenario (1 out of 16, 99zones thermal regulator only)
		WIN_HOLIDAY = 13000,             // holiday operation (programma ferie)

		// generic identifiers (useful for issuing commands)
		GENERIC_PROTECTION = 302,
		GENERIC_OFF = 303,
		GENERIC_MANUAL_TIMED = 312,      // timed manual operation (generic mode)
		GENERIC_WEEKEND = 315,           // command to set weekend mode
		WEEK_PROGRAM = 3100,             // command to set the program to be executed (generic mode)
						 // remember to add the program number to this number
		SCENARIO_PROGRAM = 3200,         // command to set the scenario to be executed (generic mode)
						 // remember to add the program number to this number
		HOLIDAY_NUM_DAYS = 33000,        // command to set the number of days of holiday mode (generic mode)
						 // remember to add the number of days to this number
	};

	static const QString WHO;

protected:
	ThermalDevice(QString where, bool p=false, int g=-1);
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
};


class ThermalDevice4Zones : public ThermalDevice
{
Q_OBJECT
public:
	ThermalDevice4Zones(QString where, bool p=false, int g=-1);

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
	ThermalDevice99Zones(QString where, bool p=false, int g=-1);

	/**
	 * Sets the scenario on the thermal regulator.
	 * \param scenario The scenario to be activated (16 max).
	 */
	void setScenario(int scenario);

	virtual unsigned maximumTemp() const;
	virtual thermo_type_t type() const;
};

#endif // THERMAL_DEVICE_H
