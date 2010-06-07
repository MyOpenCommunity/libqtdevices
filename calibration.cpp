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


#include "calibration.h"
#include "calibration_widget.h"
#include "btmain.h" // startCalib/endCalib
#include "pagestack.h"

#include <QMouseEvent>
#include <QBoxLayout>
#include <QPainter>
#include <QPen>

#define GRID_X 10
#define GRID_Y 10



Calibration::Calibration(bool minimal)
{
	widget = new CalibrationWidget(minimal);
	connect(widget, SIGNAL(calibrationStarted()), SLOT(calibrationStarted()));
	connect(widget, SIGNAL(calibrationEnded()), SLOT(calibrationEnded()));
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setMargin(0);
	layout->addWidget(widget);
}

bool Calibration::exists()
{
	return CalibrationWidget::exists();
}

void Calibration::showWindow()
{
	bt_global::page_stack.showUserWindow(this);
	Window::showWindow();
}

void Calibration::showEvent(QShowEvent*)
{
	widget->startCalibration();
}

void Calibration::calibrationEnded()
{
	BtMain::calibrationEnded();
	emit Closed();
}

void Calibration::calibrationStarted()
{
	BtMain::calibrationStarted();
}


CalibrationTest::CalibrationTest()
{
	first_time = false;
	setAttribute(Qt::WA_OpaquePaintEvent);
}

void CalibrationTest::showWindow()
{
	first_time = true;
	points.clear();
	bt_global::page_stack.showUserWindow(this);
	Window::showWindow();
}

void CalibrationTest::paintEvent(QPaintEvent *e)
{
	QPainter p(this);

	if (first_time)
	{
		QRect win = rect().adjusted(0, 0, -1, -1);

		p.setBrush(QBrush(QColor(0x00, 0x00, 0x00)));
		p.drawRect(win);

		p.setPen(QPen(QColor(0xff, 0xff, 0xff)));

		int w = win.width() - 1, h = win.height() - 1;
		for (int i = 0; i <= GRID_X; ++i)
			p.drawLine(w * i / GRID_X, 0, w * i / GRID_X, h);
		for (int i = 0; i <= GRID_Y; ++i)
			p.drawLine(0, h * i / GRID_Y, w, h * i / GRID_Y);

		first_time = false;
	}

	p.setPen(QPen(QColor(0xff, 0x00, 0x00)));
	foreach (const QPoint &pt, points)
		p.drawPoint(pt);

}

void CalibrationTest::mousePressEvent(QMouseEvent *e)
{
	points.append(e->pos());
	update();
}

void CalibrationTest::mouseMoveEvent(QMouseEvent *e)
{
	points.append(e->pos());
	update();
}

void CalibrationTest::mouseDoubleClickEvent(QMouseEvent *e)
{
	emit Closed();
}
