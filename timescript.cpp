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


#include "timescript.h"
#include "generic_functions.h" // DateConversions::formatDateConfig

#include <QTimerEvent>


TimeScript::TimeScript(QWidget *parent, uchar tipo, QDateTime* mioOrol)
    : QLCDNumber(parent)
{
	setFrameStyle(QFrame::Plain);
	type = tipo;
	if (tipo != 2)
		setNumDigits(8);
	else
		setNumDigits(5);
	if (type != 2)
		normalTimer = startTimer(1000);
	showDateTimer = -1;
	setSegmentStyle(Flat);
	mioClock = NULL;

	if (mioOrol)
		mioClock = new QDateTime(*mioOrol);

	if (type != 25)
		showTime();
	else
		showDate();
}

void TimeScript::timerEvent(QTimerEvent *e)
{
	if (mioClock)
	{
		static QDateTime prevDateTime = QDateTime();
		QDateTime now = QDateTime::currentDateTime();
		if (prevDateTime.secsTo(now) > 2 || prevDateTime.secsTo(now) < -2)
		{
			qDebug("TimeScript::timerEvent() : Updating mioClock");
			*mioClock = now;
		}
		else
			*mioClock = mioClock->addSecs(1);
		prevDateTime = now;
	}
	if (e->timerId() == showDateTimer)
		if (!type)
			stopDate();
		else
			showDate();
	else
	{
		if (showDateTimer == -1)
			showTime();
	}
}

void TimeScript::mousePressEvent(QMouseEvent *)
{
}

void TimeScript::showDate()
{
	QDate date;

	if (showDateTimer != -1 && !type)
		return;

	if (mioClock)
		date = mioClock->date();
	else
		date = QDate::currentDate();

	QString s = DateConversions::formatDateConfig(date, '.');

	setNumDigits(s.length());
	display(s);

	if (showDateTimer == -1)
	{
		if (!type)
			showDateTimer = startTimer(4000);
		else
			showDateTimer = startTimer(1000);
	}
}

void TimeScript::stopDate()
{
	killTimer(showDateTimer);
	showDateTimer = -1;

	if (type != 25)
	{
		showTime();
		if (type != 2)
			setNumDigits(8);
		else
			setNumDigits(5);
	}
	else
		showDate();
}

void TimeScript::reset()
{
	if (mioClock)
	{
		QDateTime OroTemp = QDateTime::currentDateTime();
		delete mioClock;
		mioClock = new QDateTime(OroTemp);
	}

	stopDate();
}

void TimeScript::showTime()
{
	QString s;

	if (mioClock)
	{
		if (type == 2)
			s = mioClock->toString("h:mm");
		else
			s = mioClock->toString("h:mm:ss");
	}
	else
	{
		if (type == 2)
			s = QTime::currentTime().toString("h:mm");
		else
			s = QTime::currentTime().toString("h:mm:ss");
	}
	display(s);
}

void TimeScript::diminSec()
{
	*mioClock = mioClock->addSecs(-1);
	showTime();
}

void TimeScript::diminMin()
{
	*mioClock = mioClock->addSecs(-60);
	showTime();
}

void TimeScript::diminOra()
{
	*mioClock = mioClock->addSecs(-3600);
	showTime();
}

void TimeScript::diminDay()
{
	*mioClock = mioClock->addDays(-1);
	showDate();
}

void TimeScript::diminMonth()
{
	*mioClock = mioClock->addMonths(-1);
	showDate();
}

void TimeScript::diminYear()
{
	*mioClock = mioClock->addYears(-1);
	showDate();
}

void TimeScript::aumSec()
{
	*mioClock = mioClock->addSecs(1);
	showTime();
}

void TimeScript::aumMin()
{
	*mioClock = mioClock->addSecs(60);
	showTime();
}

void TimeScript::aumOra()
{
	*mioClock = mioClock->addSecs(3600);
	showTime();
}

void TimeScript::aumDay()
{
	*mioClock = mioClock->addDays(1);
	showDate();
}

void TimeScript::aumMonth()
{
	*mioClock = mioClock->addMonths(1);
	showDate();
}

void TimeScript::aumYear()
{
	*mioClock = mioClock->addYears(1);
	showDate();
}

TimeScript::~TimeScript()
{
	if (mioClock)
		delete mioClock;
}
