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


#include "bttime.h"

#include <QDateTime>
#include <QLocale>
#include <QDebug>

// The language used for the floating point number
static QLocale loc(QLocale::Italian);


QString formatTime(const BtTime &t)
{
	QString str;
	int h = t.hour();
	int m = t.minute();
	int s = t.second();
	float ts = t.tsecond();
	if (h == 0 && m == 0 && s == 0) // time in tenths of a second
		str = QString("%1''").arg(loc.toString(ts / 10, 'f', 1));
	else if (h == 0 && m == 0)  // time in secs
		str = QString("%1''").arg(s);
	else if (h == 0) // time in mins
		str = QString("%1'").arg(m);
	else if (h < 10)   // time in hh:mm
		str = QString("%1:%2").arg(h).arg(m, 2, 10, QChar('0'));
	else
		str = QString("%1h").arg(h);
	return str;
}


BtTime::BtTime()
{
	init(0, 0, 0, 0);
}

BtTime::BtTime(int h, int m, int s, int ts)
{
	init(h, m , s, ts);
}

BtTime::BtTime(const QTime &t)
{
	init(t.hour(), t.minute(), t.second(), 0);
}

void BtTime::init(int h, int m, int s, int ts)
{
	_hour = h;
	_minute = m;
	_second = s;
	_tsecond = ts;
	max_hours = 24;
	max_minutes = 60;
	max_seconds = 60;
}

void BtTime::setMaxHours(int max)
{
	max_hours = max;
}

void BtTime::setMaxMinutes(int max)
{
	max_minutes = max;
}

void BtTime::setMaxSeconds(int max)
{
	max_seconds = max;
}

BtTime BtTime::addSecond(int s) const
{
	BtTime t = *this;

	// adds seconds
	t._second += s;

	// computes quotient and adds minutes
	int quot = t._second / max_seconds;
	t = t.addMinute(quot);

	// computes seconds value using modulus operation
	int mod = t._second % max_seconds;
	mod = mod >= 0 ? mod : max_seconds + mod;

	// minutes must be corrected by -1 if seconds are negative and multiple of max_seconds
	t = ((t._second < 0) && (mod != 0)) ? t.addMinute(-1) : t;

	// now, that we have used t._second sets it to final value
	t._second = mod;

	return t;
}

BtTime BtTime::addMinute(int m) const
{
	BtTime t = *this;

	// adds minutes
	t._minute += m;

	// computes quotient and adds hours
	int quot = t._minute / max_minutes;
	t = t.addHour(quot);

	// computes minutes value using modulus operation
	int mod = t._minute % max_minutes;
	mod = mod >= 0 ? mod : max_minutes + mod;

	// hours must be corrected by -1 if minutes are negative and multiple of max_minutes
	t = ((t._minute < 0) && (mod != 0)) ? t.addHour(-1) : t;

	// now, that we have used t._minute sets it to final value
	t._minute = mod;

	return t;
}

BtTime BtTime::addHour(int h) const
{
	BtTime t = *this;

	// adds hours modulo max_hours
	t._hour = (t._hour + h) % max_hours;

	// if hours are negative we must shift them by max_hours
	if (t._hour < 0)
		t._hour += max_hours;

	return t;
}

int BtTime::hour() const
{
	return _hour;
}

int BtTime::minute() const
{
	return _minute;
}

int BtTime::second() const
{
	return _second;
}

int BtTime::tsecond() const
{
	return _tsecond;
}

QString BtTime::toString() const
{
	QString str;
	str.sprintf("%d:%02d:%02d", _hour, _minute, _second);
	return str;
}

