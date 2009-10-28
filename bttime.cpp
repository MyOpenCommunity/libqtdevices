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

// BtTimeSeconds implementation

BtTimeSeconds::BtTimeSeconds()
		: _hour(0),
		_minute(0),
		_second(0),
		max_hours(24),
		max_minutes(60),
		max_seconds(60)
{
}

BtTimeSeconds::BtTimeSeconds(int h, int m, int s)
		: _hour(h),
		_minute(m),
		_second(s),
		max_hours(24),
		max_minutes(60),
		max_seconds(60)
{
}

BtTimeSeconds::BtTimeSeconds(const QTime &t)
		: max_hours(24),
		max_minutes(60),
		max_seconds(60)
{
	_hour = t.hour();
	_minute = t.minute();
	_second = t.second();
}

void BtTimeSeconds::setMaxHours(int max)
{
	max_hours = max;
}

void BtTimeSeconds::setMaxMinutes(int max)
{
	max_minutes = max;
}

void BtTimeSeconds::setMaxSeconds(int max)
{
	max_seconds = max;
}

BtTimeSeconds BtTimeSeconds::addSecond(int s) const
{
	BtTimeSeconds t(this->_hour, this->_minute, this->_second);
	t.setMaxHours(max_hours);
	t.setMaxMinutes(max_minutes);
	t.setMaxSeconds(max_seconds);
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
		qFatal("BtTimeSeconds::addSecond(): _second != +-1");
	return t;
}

BtTimeSeconds BtTimeSeconds::addMinute(int m) const
{
	BtTimeSeconds t(this->_hour, this->_minute, this->_second);
	t.setMaxHours(max_hours);
	t.setMaxMinutes(max_minutes);
	t.setMaxSeconds(max_seconds);
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

BtTimeSeconds BtTimeSeconds::addHour(int h) const
{
	BtTimeSeconds t(this->_hour, this->_minute, this->_second);
	t.setMaxHours(max_hours);
	t.setMaxMinutes(max_minutes);
	t.setMaxSeconds(max_seconds);
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

int BtTimeSeconds::hour() const
{
	return _hour;
}

int BtTimeSeconds::minute() const
{
	return _minute;
}

int BtTimeSeconds::second() const
{
	return _second;
}

QString BtTimeSeconds::toString() const
{
	QString str;
	str.sprintf("%u:%02u:%02u", _hour, _minute, _second);
	return str;
}

// BtTime implementation

BtTime::BtTime()
	: BtTimeSeconds()
{
}

BtTime::BtTime(int h, int m)
	: BtTimeSeconds(h, m, 0)
{
}

BtTime::BtTime(const QTime &t)
	: BtTimeSeconds(t.hour(), t.minute(), 0)
{
}

BtTime::BtTime(const BtTimeSeconds &t)
	: BtTimeSeconds(t.hour(), t.minute(), 0)
{
	setMaxMinutes(t.max_minutes);
	setMaxHours(t.max_hours);
}

QString BtTime::toString() const
{
	QString str;
	str.sprintf("%u:%02u", _hour, _minute);
	return str;
}

BtTime BtTime::addMinute(int minute) const
{
	return BtTime(BtTimeSeconds::addMinute(minute));
}

BtTime BtTime::addHour(int hour) const
{
	return BtTime(BtTimeSeconds::addHour(hour));
}
