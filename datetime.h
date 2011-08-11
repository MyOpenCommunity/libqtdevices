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
#include "page.h"

#include <QWidget>
#include <QDateTime>
#include <QHBoxLayout>

class QLabel;
class QLCDNumber;


/*!
	\ingroup Core
	\brief Emulates a QTimeEdit using a BtTime.

	This widget allows the user to edit a time value in the day, ie. between
	0:00 and 23:59. The current time set can be retrieved with the time() function.
	The default value for time is 0:00.
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

	BtTimeEdit(QWidget *parent = 0, DisplayType type = DISPLAY_MINUTES);

	/*!
		\brief Set the \a time on the widget.
	*/
	void setTime(const BtTime& time);

	/*!
		\brief Returns the time set on the widget.
	*/
	BtTime time();

	/*!
		\brief Set the maximum number of hours
		\sa BtTime::setMaxHours()
	*/
	void setMaxHours(int hours);

	/*!
		\brief Set the maximum number of minutes.
		\sa BtTime::setMaxMinutes()
	*/
	void setMaxMinutes(int minutes);

	/*!
		\brief Set the maximum number of seconds.
		\sa BtTime::setMaxSeconds()
	*/
	void setMaxSeconds(int seconds);

private slots:
	void incHours();
	void decHours();
	void incMin();
	void decMin();
	void incSec();
	void decSec();

private:
	void displayTime();

	BtTime _time;
#ifdef LAYOUT_TS_10
	QLabel *hour, *minute, *second;
#else
	QLCDNumber *num;
#endif
	DisplayType _display_type;
};

/*
 * A widget that emulates QDateEdit
 *
 * The current date set can be retrived with the function date().
 * The default value for date is currentDate + 1 day.
 */
class BtDateEdit : public QWidget
{
Q_OBJECT
public:
	BtDateEdit(QWidget *parent = 0);

	void setDate(const QDate& date);

	/**
	 * Returns the date set on the widget.
	 */
	QDate date();

	void setAllowPastDates(bool v);

private:
	void displayDate();

private:
	/// display date set
#ifdef LAYOUT_TS_10
	QLabel *year, *month, *day;
#else
	QLCDNumber *date_display;
#endif
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


class PageSetDateTime : public Page
{
Q_OBJECT
public:
	PageSetDateTime(const QString &extra_button_icon, bool allow_past_dates);

	QDate date();
	BtTime time();
	void setTitle(QString title);
	void setDateTime(const QDateTime &dt);

signals:
	void dateTimeSelected(QDate, BtTime);

private slots:
	void performAction();

private:
	PageTitleWidget title_widget;
	QWidget content;
	QVBoxLayout main_layout;
	QHBoxLayout top_layout;
	BtDateEdit *date_edit;
	BtTimeEdit *time_edit;
};

#endif // DATETIME_H

