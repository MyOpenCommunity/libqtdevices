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


/*!
	\ingroup Core
	\brief A replacement for the standard QTime that can have custom limits
	for hours, minutes and seconds.
*/
class BtTime
{
friend class TestBtTime;
public:
	/*!
		\brief Constructs a BtTime with given hour, minutes, seconds and tenths of seconds.

		The default value for max_minutes and max_seconds is 60, 24 for max_hours.
	*/
	BtTime(int h, int m, int s, int ts = 0);

	/*!
		\brief Constructs a BtTime with hour, minutes and seconds equal to 0.

		The default value for max_minutes and max_seconds is 60, 24 for max_hours.
	*/
	BtTime();

	/*!
		\brief Constructs a BtBtime taking hour, minutes and seconds from the given \a t.

		The default value for max_minutes and max_seconds is 60, 24 for max_hours.
	*/
	BtTime(const QTime &t);

	/*!
		\brief Set the maximum value for hours.

		The range of values allowed are then [0..\a max-1]. Default value is 24.
	*/
	void setMaxHours(int max);

	/*!
		\brief Set the maximum value for minutes.

		The range of values allowed are [0..\a max-1]. Default value is 60.
	*/
	void setMaxMinutes(int max);

	/*!
		\brief Set the maximum value for seconds.

		The range of values allowed are [0..\a max-1]. Default value is 60.
	*/
	void setMaxSeconds(int max);

	/*!
		\brief Returns a representation of time of the form h:mm:ss.
	*/
	QString toString() const;

	/* TODO: it should allow to add an arbitrary number of seconds.
	 * Adds or subtracts a second to the current time and returns a new BtTime instance.
	 * \param second Must be either 1 or -1
	 * \return A new BtTime that is one second earlier or later.
	 */
	BtTime addSecond(int second) const;

	/* TODO: it should allow to add an arbitrary number of minutes.
	 * Adds or subtracts a minute to the current time and returns a new BtTime instance.
	 * \param minute Must be either 1 or -1
	 * \return A new BtTime that is one minute earlier or later.
	 */
	BtTime addMinute(int minute) const;

	/* TODO: it should allow to add an arbitrary number of hours.
	 * Adds or subtracts a hour to the current time and returns a new BtTime instance.
	 * \param hour Must be either 1 or -1
	 * \return A new BtTime that is one hour earlier or later.
	 */
	BtTime addHour(int hour) const;

	/*!
		\brief Return the hours.
	*/
	int hour() const;

	/*!
		\brief Return the minutes.
	*/
	int minute() const;

	/*!
		\brief Return the seconds.
	*/
	int second() const;

	/*!
		\brief Return the thens of a seconds.
	*/
	int tsecond() const;

private:
	// used by constructors
	void init(int h, int m, int s, int ts);

private:
	int _hour, _minute, _second, _tsecond;
	int max_hours, max_minutes, max_seconds;
};

Q_DECLARE_METATYPE(BtTime)


inline bool operator ==(const BtTime &f, const BtTime &s)
{
	return f.hour() == s.hour() && f.minute() == s.minute() && f.second() == s.second() && f.tsecond() == s.tsecond();
}

inline bool operator !=(const BtTime &f, const BtTime &s)
{
	return !(f == s);
}

QString formatTime(const BtTime &t);


#endif // BTTIME_H
