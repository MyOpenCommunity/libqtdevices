/*!
 * \bttime.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief A reimplementation of QTime class that accepts a custom number of hours and minutes.
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */

#include "bttime.h"

BtTime::BtTime()
	: _hour(0),
	_minute(0),
	max_hours(24),
	max_minutes(60)
{
}

BtTime::BtTime(int h, int m)
	: max_hours(24),
	max_minutes(60)
{
	_hour = h;
	_minute = m;
}

BtTime::BtTime(const QTime &t)
	: max_hours(24),
	max_minutes(60)
{
	_hour = t.hour();
	_minute = t.minute();
}

void BtTime::setMaxHours(int max)
{
	max_hours = max;
}

void BtTime::setMaxMinutes(int max)
{
	max_minutes = max;
}

BtTime BtTime::addMinute(int m) const
{
	BtTime t(this->_hour, this->_minute);
	t.setMaxHours(max_hours);
	t.setMaxMinutes(max_minutes);
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
		qFatal("BtTime::addHour(): _minute != +-1");
	return t;
}

BtTime BtTime::addHour(int h) const
{
	BtTime t(this->_hour, this->_minute);
	t.setMaxHours(max_hours);
	t.setMaxMinutes(max_minutes);
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
		qFatal("BtTime::addHour(): _hour != +-1");
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

QString BtTime::toString() const
{
	QString str;
	str.sprintf("%u:%02u", _hour, _minute);
	return str;
}
