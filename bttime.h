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


#ifndef BTTIME_H
#define BTTIME_H

#include <QString>
#include <QVariant>

class QTime;

namespace DateConversions
{
	QString formatDateConfig(const QDate &date);
}

class BtTime
{
friend class TestBtTime;
public:
	/**
	 * Constructs a BtTime with given hour, minute and second. The default value for
	 * max_minutes and max_seconds is 60, 24 for max_hours.
	 */
	BtTime(int h, int m, int s);

	/**
	 * Defaults to hour = 0, minute = 0, second = 0, max_hours = 24, max_minutes = 60, max_seconds = 60
	 */
	BtTime();

	/**
	 * Takes hour, minute and second out of t, max_hours, max_minutes and max_seconds are the default values.
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
	 * Adds or subtracts a second to the current time and returns a new BtTime instance.
	 * \param second Must be either 1 or -1
	 * \return A new BtTime that is one second earlier or later.
	 */
	BtTime addSecond(int second) const;

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

	/**
	 * Getter method for seconds.
	 * \return Current second.
	 */
	int second() const;

private:
	// used by constructors
	void init(int h, int m, int s);

private:
	int _hour, _minute, _second;
	int max_hours, max_minutes, max_seconds;
};

Q_DECLARE_METATYPE(BtTime);

inline bool operator ==(const BtTime &f, const BtTime &s)
{
	return f.hour() == s.hour() && f.minute() == s.minute() && f.second() == s.second();
}

inline bool operator !=(const BtTime &f, const BtTime &s)
{
	return !(f == s);
}

QString formatTime(const BtTime &t);


#endif //BTTIME_H
