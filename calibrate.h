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

#include <QWSPointerCalibrationData>
#include <QWidget>
#include <QPixmap>


class BtButton;
class QLabel;
class QTimer;


/*!
  \class calibrate
  \brief This is a class that does the calibration of the device.

  According to the forth argument it is possible to choose if the calibration process must have for or five pressions.  
*/

class Calibrate : public QWidget
{
	Q_OBJECT
public:
	Calibrate(QWidget* parent=0, unsigned char manut=0);
	~Calibrate();

private:
	QPoint fromDevice(const QPoint &p);
	/// Check if the calibration is ok
	bool sanityCheck();
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	/// Start the buttons test
	void buttonsTest();
	void trackCrosshair();
	void startCalibration();
	BtButton *createButton(const char* icon_name, int x, int y);

private slots:
	void timeout();
	void endCalibration();
	void rollbackCalibration();

signals:
	void inizioCalib();
	void fineCalib();

private:
	QPixmap logo;
#if defined (BTWEB) ||  defined (BT_EMBEDDED)
	QWSPointerCalibrationData cd;
	QWSPointerCalibrationData::Location location;
#endif
	QPoint crossPos;
	// the new position of the crosshair
	QPoint newPos;
	QPoint penPos;
	QTimer *timer, *button_timer;
	QLabel *box_text;
	BtButton *b1, *b2;
	unsigned char manut;
	int dx;
	int dy;
	bool button_test;
};


#endif //CALIBRATE_H


