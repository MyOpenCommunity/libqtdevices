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
#include "main.h" // getConfElement, (*bt_global::config)

#include <QTimerEvent>


timeScript::timeScript(QWidget *parent, uchar tipo, QDateTime* mioOrol)
    : QLCDNumber(parent)
{
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

void timeScript::timerEvent(QTimerEvent *e)
{
	if (mioClock)
	{
		static QDateTime prevDateTime = QDateTime();
		QDateTime now = QDateTime::currentDateTime();
		if ((prevDateTime.secsTo(now) > 2) || (prevDateTime.secsTo(now) < -2))
		{
			qDebug("timeScript::timerEvent() : Updating mioClock");
			*mioClock = now;
		}
		else
			*mioClock=mioClock->addSecs(1);
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

void timeScript::mousePressEvent(QMouseEvent *)
{
}

void timeScript::showDate()
{
	QDate date;

	if ((showDateTimer != -1) && (!type))
		return;
	if (mioClock)
		date = mioClock->date();
	else
		date = QDate::currentDate();

	QString s;

	if ((*bt_global::config)[DATE_FORMAT].toInt() == USA_DATE)
		s = date.toString("MM.dd.yy");
	else
		s = date.toString("dd.MM.yy");

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

void timeScript::stopDate()
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

void timeScript::reset()
{
	if (mioClock)
	{
		QDateTime OroTemp = QDateTime::currentDateTime();
		delete mioClock;
		mioClock = new QDateTime(OroTemp);
	}

	stopDate();
}

void timeScript::showTime()
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

void timeScript::diminSec()
{
	*mioClock = mioClock->addSecs(-1);
	showTime();
}

void timeScript::diminMin()
{
	*mioClock = mioClock->addSecs(-60);
	showTime();
}

void timeScript::diminOra()
{
	*mioClock = mioClock->addSecs(-3600);
	showTime();
}

void timeScript::diminDay()
{
	*mioClock = mioClock->addDays(-1);
	showDate();
}

void timeScript::diminMonth()
{
	*mioClock = mioClock->addMonths(-1);
	showDate();
}

void timeScript::diminYear()
{
	*mioClock = mioClock->addYears(-1);
	showDate();
}

void timeScript::aumSec()
{
	*mioClock = mioClock->addSecs(1);
	showTime();
}

void timeScript::aumMin()
{
	*mioClock = mioClock->addSecs(60);
	showTime();
}

void timeScript::aumOra()
{
	*mioClock = mioClock->addSecs(3600);
	showTime();
}

void timeScript::aumDay()
{
	*mioClock = mioClock->addDays(1);
	showDate();
}

void timeScript::aumMonth()
{
	*mioClock = mioClock->addMonths(1);
	showDate();
}

void timeScript::aumYear()
{
	*mioClock = mioClock->addYears(1);
	showDate();
}

QDateTime timeScript::getDataOra()
{
	if (type)
		return *mioClock;
	return QDateTime::currentDateTime();
}

void timeScript::setDataOra(QDateTime d)
{
	*mioClock = d;
}

timeScript::~timeScript()
{
	if (mioClock)
		delete mioClock;
}
