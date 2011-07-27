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

// The Calibration is shown from the settings or during the boot. In the latter
// case the configuration file is not yet parsed, so we have to force the
// skin manager context in order to retrieve the images used in the calibration.
#define CALIBRATION_CONTEXT 14153

/*!
	\ingroup Core
	\ingroup Settings
	\brief Uses the CalibrationWidget to perform a fullscreen calibration.
*/
class Calibration : public Window
{
Q_OBJECT
public:
	Calibration(bool minimal = false);

	virtual void showWindow();

	// Return true if the calibration file exists.
	static bool exists();

protected:
	virtual void showEvent(QShowEvent*);

private slots:
	void calibrationEnded();
	void calibrationStarted();

private:
	CalibrationWidget *widget;
};



/*!
	\ingroup Settings
	\brief Tests calibration results: draws a red dot for every mouse event.
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


