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

#include <QDateTime>

QString formatTime(const BtTime &t)
{
	QString str;
	int h = t.hour();
	int m = t.minute();
	int s = t.second();
	if (h == 0 && m == 0)  // time in secs
		str = QString("%1''").arg(s);
	else if (h == 0) // time in mins
		str = QString("%1'").arg(m);
	else if (h < 10)   // time in hh:mm
		str = QString("%1:%2").arg(h).arg(m, 2, 10, QChar('0'));
	else
		str = QString("%1h").arg(h);
	return str;
}



// BtTime implementation

BtTime::BtTime()
{
	init(0, 0, 0);
}

BtTime::BtTime(int h, int m, int s)
{
	init(h, m , s);
}

BtTime::BtTime(const QTime &t)
{
	init(t.hour(), t.minute(), t.second());
}

void BtTime::init(int h, int m, int s)
{
	_hour = h;
	_minute = m;
	_second = s;
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
		qFatal("BtTime::addHour(): _minute != +-1");
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

int BtTime::second() const
{
	return _second;
}

QString BtTime::toString() const
{
	QString str;
	str.sprintf("%u:%02u:%02u", _hour, _minute, _second);
	return str;
}

