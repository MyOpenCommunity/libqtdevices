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


#ifndef	CALIBRATE_H
#define	CALIBRATE_H

#include "window.h"

class CalibrationWidget;


class Calibration : public Window
{
Q_OBJECT
public:
	Calibration(bool minimal = false);

	virtual void showWindow();

	// Return true if the calibration file exists.
	static bool exists();

private slots:
	void calibrationEnded();
	void calibrationStarted();

private:
	CalibrationWidget *widget;
};



/**
 * Simple page to test calibration results: draws a red dot for every mouse event
 */
class CalibrationTest : public Window
{
Q_OBJECT
public:
	CalibrationTest();

	virtual void showWindow();

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);

private:
	bool first_time;
	QList<QPoint> points;
};

#endif //CALIBRATE_H


