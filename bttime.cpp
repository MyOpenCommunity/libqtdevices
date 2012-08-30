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
	if (s == 1)
	{
		if (t._second == max_seconds - 1)
		{
			t._second = 0;
			t = t.addMinute(s);
		}
		else
			++t._second;
	}
	else if (s == -1)
	{
		if (t._second == 0)
		{
			t._second = max_seconds - 1;
			t = t.addMinute(s);
		}
		else
			--t._second;
	}
	else
		qFatal("BtTime::addSecond(): _second != +-1");
	return t;
}

BtTime BtTime::addMinute(int m) const
{
	if (-m > max_minutes)
		qFatal("You can't subtract more than max_minutes.");

	BtTime t = *this;
	if (m == 1)
	{
		if (t._minute == max_minutes - 1)
		{
			t._minute = 0;
			t = t.addHour(m);
		}
		else
			++t._minute;
	}
	else if (m == -1)
	{
		if (t._minute == 0)
		{
			t._minute = max_minutes - 1;
			t = t.addHour(m);
		}
		else
			--t._minute;
	}
	else
	{
		int tmp = t._minute + m;
		t._minute = (tmp + max_minutes) % max_minutes;
		int div = 0;
		if (m < 0 && tmp < 0)
		{
			div = -((max_minutes - m) / max_minutes);
		}
		else
			div = tmp / max_minutes;
		t = t.addHour(div);
	}
	return t;
}

BtTime BtTime::addHour(int h) const
{
	BtTime t = *this;
	if (h == 1)
	{
		if (t._hour == max_hours - 1)
		{
			t._hour = 0;
		}
		else
			++t._hour;
	}
	else if (h == -1)
	{
		if (t._hour == 0)
		{
			t._hour = max_hours - 1;
		}
		else
			--t._hour;
	}
	else
		t._hour = (t._hour + h + max_hours) % max_hours;
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

