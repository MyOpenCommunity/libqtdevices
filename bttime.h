/*!
 * \bttime.cpp
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A reimplementation of QTime class that accepts a custom number of hours and minutes.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BTTIME_H
#define BTTIME_H

#include <QDateTime>
#include <QString>

class BtTime
{
public:
	/**
	 * Constructs a BtTime with given hour and minute. The default value for
	 * max_minutes is 60 and max_hours is 24.
	 */
	BtTime(int h, int m);

	/**
	 * Defaults to hour = 0, minute = 0, max_hours = 24, max_minutes = 60
	 */
	BtTime();

	/**
	 * Takes hour and minute out of t, max_hours and max_minutes are the default values.
	 */
	BtTime(const QTime &t);

	/**
	 * Set the maximum value for hours. The range of values allowed are then [0..max_hours-1]
	 * Default value is 24.
	 * \param max The maximum number of hours allowed.
	 */
	void setMaxHours(int max);

	/**
	 * Set the maximum value for minutes. The range of values allowed are [0..max_minutes-1]
	 * Default value is 60.
	 * \param max The maximum number of minutes allowed.
	 */
	void setMaxMinutes(int max);

	/**
	 * Returns a representation of time of the form h:mm.
	 * \return A time in string format
	 */
	QString toString() const;

	/**
	 * Adds or subtracts a minute to the current time and returns a new BtTime instance.
	 * \param minute Must be either 1 or -1
	 * \return A new BtTime that is one minute earlier or later.
	 */
	BtTime addMinute(int minute) const;

	/**
	 * Adds or subtracts a hour to the current time and returns a new BtTime instance.
	 * \param hour Must be either 1 or -1
	 * \return A new BtTime that is one hour earlier or later.
	 */
	BtTime addHour(int hour) const;

	/**
	 * Getter method for hours.
	 * \return Current hour.
	 */
	int hour() const;

	/**
	 * Getter method for minutes.
	 * \return Current minute.
	 */
	int minute() const;
private:
	int _hour, _minute;
	int max_hours, max_minutes;
};
#endif //BTTIME_H
