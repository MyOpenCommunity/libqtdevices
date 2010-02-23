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


#ifndef DATETIME_H
#define DATETIME_H

#include "bttime.h"

#include <QWidget>
#include <QLCDNumber>
#include <QDateTime>

/**
 * A widget that emulates QTimeEdit
 *
 * This widget allows the user to edit a time value in the day, ie. between
 * 0:00 and 23:59. The current time set can be retrieved with the time() function.
 * The default value for time is 0:00.
 */
class BtTimeEdit : public QWidget
{
Q_OBJECT
public:
	enum DisplayType
	{
		DISPLAY_MINUTES = 0,
		DISPLAY_SECONDS = 1
	};

	BtTimeEdit(QWidget *parent, DisplayType type = DISPLAY_MINUTES);

	void setTime(const BtTime& time);

	/**
	 * Returns the time set on the widget.
	 */
	BtTime time();

	void setMaxHours(int hours);
	void setMaxMinutes(int minutes);
	void setMaxSeconds(int minutes);
private slots:
	void incHours();
	void decHours();
	void incMin();
	void decMin();
	void incSec();
	void decSec();
private:
	void displayTime();
private:
	BtTime _time;
	QLCDNumber *num;
	DisplayType _display_type;
};

/**
 * A widget that emulates QDateEdit
 *
 * The current date set can be retrived with the function date().
 * The default value for date is currentDate + 1 day.
 */
class BtDateEdit : public QWidget
{
Q_OBJECT
public:
	BtDateEdit(QWidget *parent);

	void setDate(const QDate& date);

	/**
	 * Returns the date set on the widget.
	 */
	QDate date();

	void setAllowPastDates(bool v);
private:
	/// display date set
	QLCDNumber *date_display;
	QDate _date;
	bool _allow_past_dates;
	static QString FORMAT_STRING;
private slots:
	void incDay();
	void incMonth();
	void incYear();
	void decDay();
	void decMonth();
	void decYear();
};
#endif // DATETIME_H

