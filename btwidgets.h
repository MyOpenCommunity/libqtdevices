/*!
 * \btwidgets.h
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \brief Implementation of useful widgets for BTouch
 *
 *
 * \author Luca Ottaviano <lottaviano@develer.com>
 */
#ifndef BTWIDGETS_H
#define BTWIDGETS_H

#include <qwidget.h>
#include <qlcdnumber.h>
#include <qdatetime.h>

/**
 * A widget that emulates QTimeEdit
 *
 * This widget allows the user to edit 2 time values (hours and minutes).
 * Maximum values for hours and minutes can be set using appropriate functions; by default
 * it's possible to insert any value between 00:00 and 23:59 (inclusive), i.e. maximum hours are 23 and
 * maximun minutes are 59.
 * Whenever the value displayed changes, the signal valueChanged(int, int) is emitted
 * The default value is 0:0.
 */
class BtTimeEdit : public QWidget
{
Q_OBJECT
public:
	BtTimeEdit(QWidget *parent, const char *name);

	/**
	 * Set the maximum value for hours. If \a h is negative, no action is performed.
	 *
	 * \param h The maximum value allowed for the hours field.
	 */
	void setMaxHours(int h);

	/**
	 * Set the maximum value for minutes. If \a m is negative, no action is performed.
	 *
	 * \param m The maximum value allowed for the minutes field.
	 */
	void setMaxMins(int m);

	QTime time();
private slots:
	void incHours();
	void decHours();
	void incMin();
	void decMin();
private:
	QTime _time;
	int max_hours, max_minutes;
	QLCDNumber *num;
signals:
	/**
	 * This signal is emitted whenever the value changes.
	 *
	 * \param The value for hours.
	 * \param The value for minutes.
	 */
	void valueChanged(int, int);
};

/**
 * A widget that emulates QDateEdit
 *
 * The default value for date is currentDate + 1 day.
 */
class BtDateEdit : public QWidget
{
Q_OBJECT
public:
	BtDateEdit(QWidget *parent, const char *name);
	QDate date();
private:
	/// display date set
	QLCDNumber *date_display;
	QDate _date;
private slots:
	void incDay();
	void incMonth();
	void incYear();
	void decDay();
	void decMonth();
	void decYear();
signals:
	void valueChanged(QDate);
};
#endif // BTWIDGETS_H

