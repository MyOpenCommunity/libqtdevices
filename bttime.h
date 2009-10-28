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

class BtTimeSeconds
{
	friend class BtTime;
public:
	/**
	 * Constructs a BtTime with given hour, minute and second. The default value for
	 * max_minutes and max_seconds is 60, 24 for max_hours.
	 */
	BtTimeSeconds(int h, int m, int s);

	/**
	 * Defaults to hour = 0, minute = 0, second = 0, max_hours = 24, max_minutes = 60, max_seconds = 60
	 */
	BtTimeSeconds();

	/**
	 * Takes hour, minute and second out of t, max_hours, max_minutes and max_seconds are the default values.
	 */
	BtTimeSeconds(const QTime &t);

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
	 * Set the maximum value for seconds. The range of values allowed are [0..max_seconds-1]
	 * Default value is 60.
	 * \param max The maximum number of seconds allowed.
	 */
	void setMaxSeconds(int max);

	/**
	 * Returns a representation of time of the form h:mm:ss.
	 * \return A time in string format
	 */
	QString toString() const;

	/**
	 * Adds or subtracts a second to the current time and returns a new BtTimeSeconds instance.
	 * \param second Must be either 1 or -1
	 * \return A new BtTimeSeconds that is one second earlier or later.
	 */
	BtTimeSeconds addSecond(int second) const;

	/**
	 * Adds or subtracts a minute to the current time and returns a new BtTimeSeconds instance.
	 * \param minute Must be either 1 or -1
	 * \return A new BtTimeSeconds that is one minute earlier or later.
	 */
	BtTimeSeconds addMinute(int minute) const;

	/**
	 * Adds or subtracts a hour to the current time and returns a new BtTimeSeconds instance.
	 * \param hour Must be either 1 or -1
	 * \return A new BtTimeSeconds that is one hour earlier or later.
	 */
	BtTimeSeconds addHour(int hour) const;

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

	/**
	 * Getter method for seconds.
	 * \return Current second.
	 */
	int second() const;
private:
	int _hour, _minute, _second;
	int max_hours, max_minutes, max_seconds;
};


class BtTime : private BtTimeSeconds
{
public:
	/**
	 * Constructs a BtTime with given hour and minute. The default value for
	 * max_minutes is 60 and 24 for max_hours.
	 */
	BtTime(int h, int m);

	/**
	 * Defaults to hour = 0, minute = 0, max_hours = 24, max_minutes = 60
	 */
	BtTime();

	/**
	 * Takes hour and minute out of t,
	 * max_hours and max_minutes are the default values.
	 */
	BtTime(const QTime &t);

	/**
	 * Takes hour, minute, max_hour, max_minute out of t,
	 */
	BtTime(const BtTimeSeconds &t);

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

	using BtTimeSeconds::hour;
	using BtTimeSeconds::minute;
	using BtTimeSeconds::setMaxHours;
	using BtTimeSeconds::setMaxMinutes;
};


#endif //BTTIME_H
