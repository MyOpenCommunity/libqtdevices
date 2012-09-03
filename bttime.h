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
		The constructor doesn't make any checks about values passed in.
		If you want to use a BtTime with a value of max hours equal to 30, for example,
		you have to build the BtTime passing the desired values for hour, minute
		and second. Immediatly after the constructor, call appropriate setMax*
		methods otherwise subsequent operations may lead to unpredictable
		behavior.
	*/
	BtTime(int h, int m, int s, int ts = 0);

	/*!
		\brief Constructs a BtTime with hour, minutes and seconds equal to 0.

		The default value for max_minutes and max_seconds is 60, 24 for max_hours.
		See notes on first constructor about setMax* usage.
	*/
	BtTime();

	/*!
		\brief Constructs a BtBtime taking hour, minutes and seconds from the given \a t.

		The default value for max_minutes and max_seconds is 60, 24 for max_hours.
		See notes on first constructor about setMax* usage.
	*/
	BtTime(const QTime &t);

	/*!
		\brief Set the maximum value for hours.

		The range of values allowed are then [0..\a max-1]. Default value is 24.
		See notes on first constructor about setMax* usage.
	*/
	void setMaxHours(int max);

	/*!
		\brief Set the maximum value for minutes.

		The range of values allowed are [0..\a max-1]. Default value is 60.
		See notes on first constructor about setMax* usage.
	*/
	void setMaxMinutes(int max);

	/*!
		\brief Set the maximum value for seconds.

		The range of values allowed are [0..\a max-1]. Default value is 60.
		See notes on first constructor about setMax* usage.
	*/
	void setMaxSeconds(int max);

	/*!
		\brief Returns a representation of time of the form h:mm:ss.
	*/
	QString toString() const;

	/*!
		\brief Returns a BtTime object equal to this, but with second seconds added in.

		The value passed can be any integer, even negative.
		Hours are updated considering max minutes value.
		Minutes are updated considering max seconds value.
		For example, if max seconds is set to 100 and you add 250 seconds, the
		returned object is equal to this with 2 minutes and 50 seconds added in.
		The same hold true for max minutes.
		This may be or may not be the desired behavior.
	*/
	BtTime addSecond(int second) const;

	/*!
		\brief Returns a BtTime object equal to this, but with minute minutes added in.

		The value passed can be any integer, even negative.
		Seconds are not taken into consideration.
		Hours are updated considering max minutes value.
		For example, if max minutes is set to 100 and you add 250 minutes, the
		returned object is equal to this with 2 hours and 50 minutes added in.
		This may be or may not be the desired behavior.
	*/
	BtTime addMinute(int minute) const;

	/*!
		\brief Returns a BtTime object equal to this, but with hour hours added in.

		The value passed can be any integer, even negative.
		Seconds and minutes are not taken into consideration.
		This may be or may not be the desired behavior.
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
