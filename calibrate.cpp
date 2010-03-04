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


#include "calibrate.h"
#include "fontmanager.h" // bt_global::font
#include "skinmanager.h" // bt_global::skin
#include "btbutton.h"

#include <QGlobalStatic> // qAbs
#include <QVariant> // setProperty
#include <QMouseEvent>
#include <QFile>
#include <QPainter>
#include <QTimer>
#include <QScreen>
#include <QWSServer>
#include <QDebug>

#define BUTTON_SEC_TIMEOUT 10


namespace
{
	QPoint fromDevice(const QPoint &p)
	{
		return qt_screen->mapFromDevice(p, QSize(qt_screen->deviceWidth(), qt_screen->deviceHeight()));
	}
}


Calibration::Calibration(bool minimal)
{
	int width = qt_screen->deviceWidth();
	int height = qt_screen->deviceHeight();

	pointercal_file = "/etc/pointercal";
	if (char *pointercal_file_env = getenv("POINTERCAL_FILE"))
		pointercal_file = QString(pointercal_file_env);

	int cross_margin = 30;

	QPoint *points = calibration_data.screenPoints;

#ifdef BT_HARDWARE_TOUCHX
	points[QWSPointerCalibrationData::TopLeft] = QPoint(cross_margin, cross_margin);
	points[QWSPointerCalibrationData::BottomLeft] = QPoint(cross_margin, height - cross_margin);
	points[QWSPointerCalibrationData::BottomRight] = QPoint(width - cross_margin, height - cross_margin);
	points[QWSPointerCalibrationData::TopRight] = QPoint(width - cross_margin, cross_margin);
	points[QWSPointerCalibrationData::Center] = QPoint(width / 2, height / 2);
#else
	points[QWSPointerCalibrationData::TopLeft] = QPoint(width - cross_margin, height - cross_margin);
	points[QWSPointerCalibrationData::BottomLeft] = QPoint(width - cross_margin, cross_margin);
	points[QWSPointerCalibrationData::BottomRight] = QPoint(cross_margin, cross_margin);
	points[QWSPointerCalibrationData::TopRight] = QPoint(cross_margin, height - cross_margin);
	points[QWSPointerCalibrationData::Center] = QPoint(width / 2, height / 2);
#endif

	int buttons_margin = 10;
	topleft_button = new BtButton(this);
	topleft_button->setImage(bt_global::skin->getImage("ok"));
	topleft_button->move(buttons_margin, buttons_margin);

	bottomright_button = new BtButton(this);
	bottomright_button->setImage(bt_global::skin->getImage("ok"));
	bottomright_button->move(width - buttons_margin - bottomright_button->width(),
		height - buttons_margin - bottomright_button->height());

	connect(topleft_button, SIGNAL(clicked()), topleft_button, SLOT(hide()));
	connect(topleft_button, SIGNAL(clicked()), bottomright_button, SLOT(show()));
	connect(bottomright_button, SIGNAL(clicked()), bottomright_button, SLOT(hide()));
	connect(bottomright_button, SIGNAL(clicked()), SLOT(endCalibration()));

#ifdef LAYOUT_BTOUCH
	logo.load(bt_global::skin->getImage("logo"));
#endif

	buttons_timer = new QTimer(this);
	connect(buttons_timer, SIGNAL(timeout()), SLOT(rollbackCalibration()));

	crosshair_timer = new QTimer(this);
	connect(crosshair_timer, SIGNAL(timeout()), SLOT(drawCrosshair()));

	minimal_version = minimal;
}

void Calibration::startCalibration()
{
	text = tr("Click the crosshair");
	topleft_button->hide();
	bottomright_button->hide();
	current_location = 0;
	current_position = fromDevice(calibration_data.screenPoints[current_location]);
	test_buttons = false;

	// Backup the old calibration file
	if (QFile::exists(pointercal_file))
		system(qPrintable(QString("cp %1 %1.calibrated").arg(pointercal_file)));

	QWSServer::mouseHandler()->clearCalibration();
	grabMouse();
}

void Calibration::showEvent(QShowEvent*)
{
	startCalibration();
}

void Calibration::hideEvent(QHideEvent*)
{
	// To ensure that the mouse is always released (what happen with an entry videocall?)
	releaseMouse();
}

void Calibration::rollbackCalibration()
{
	if (QFile::exists(QString("%1.calibrated").arg(pointercal_file)))
		system(qPrintable(QString("mv %1.calibrated %1").arg(pointercal_file)));

	startCalibration();
	update();
}

void Calibration::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);

	int text_height = 30;
	int text_width = 150;

#ifdef LAYOUT_BTOUCH
	int total_height = logo.height() + text_height;
	p.drawPixmap((width() - logo.width()) / 2, (height() - total_height) / 2, logo);

	QRect rect((width() - text_width) / 2, (height() - total_height) / 2 + logo.height(),
		text_width, text_height);
	p.setFont(bt_global::font->get(FontManager::TEXT));
	p.drawText(rect, Qt::AlignHCenter, text);
#else
	QRect rect((width() - text_width) / 2, height() / 4, text_width, text_height);
	p.setFont(bt_global::font->get(FontManager::TEXT));
	p.drawText(rect, Qt::AlignHCenter, text);
#endif

	if (!test_buttons)
	{
		p.drawRect(current_position.x() - 1, current_position.y() - 8, 2, 7);
		p.drawRect(current_position.x() - 1, current_position.y() + 1, 2, 7);
		p.drawRect(current_position.x() - 8, current_position.y() - 1, 7, 2);
		p.drawRect(current_position.x() + 1, current_position.y() - 1, 7, 2);
	}
}

void Calibration::startTestButtons()
{
	text = tr("Click the OK button");
	test_buttons = true;
	topleft_button->show();
	buttons_timer->setSingleShot(true);
	buttons_timer->start(BUTTON_SEC_TIMEOUT * 1000);
	update();
}

void Calibration::mouseReleaseEvent(QMouseEvent *event)
{
	if (crosshair_timer->isActive() || test_buttons)
		return;

	// Map from device coordinates in case the screen is transformed
	QSize screenSize(qt_screen->width(), qt_screen->height());
	QPoint p = qt_screen->mapToDevice(event->pos(), screenSize);

	calibration_data.devPoints[current_location] = p;

	int num_checks = minimal_version ? 4 : 5;

	if (++current_location < num_checks)
		trackCrosshair();
	else
	{
		if (sanityCheck())
		{
			releaseMouse();
			QWSServer::mouseHandler()->calibrate(&calibration_data);
			if (minimal_version)
				endCalibration();
			else
				startTestButtons();
		}
		else
		{
			current_location = 0;
			trackCrosshair();
		}
	}
}

void Calibration::trackCrosshair()
{
	QPoint target_cross = fromDevice(calibration_data.screenPoints[current_location]);
	delta_x = (target_cross.x() - current_position.x()) / 10;
	delta_y = (target_cross.y() - current_position.y()) / 10;
	crosshair_timer->start(30);
}

void Calibration::drawCrosshair()
{
	QPoint target_cross = fromDevice(calibration_data.screenPoints[current_location]);

	bool done_x = false;
	bool done_y = false;

	QPoint new_position = QPoint(current_position.x() + delta_x, current_position.y() + delta_y);

	if (qAbs(current_position.x() - target_cross.x()) <= qAbs(delta_x))
	{
		new_position.setX(target_cross.x());
		done_x = true;
	}

	if (qAbs(current_position.y() - target_cross.y()) <= qAbs(delta_y))
	{
		new_position.setY(target_cross.y());
		done_y = true;
	}

	if (done_x && done_y)
		crosshair_timer->stop();

	current_position = new_position;
	update();
}

void Calibration::endCalibration()
{
	// The calibration was done right so we can remove the old calibration file
	if (QFile::exists(QString("%1.calibrated").arg(pointercal_file)))
		system(qPrintable(QString("rm %1.calibrated").arg(pointercal_file)));

	buttons_timer->stop();
	emit Closed();
}

bool Calibration::sanityCheck()
{
	QPoint *points = calibration_data.devPoints;

	QPoint tl = points[QWSPointerCalibrationData::TopLeft];
	QPoint tr = points[QWSPointerCalibrationData::TopRight];
	QPoint bl = points[QWSPointerCalibrationData::BottomLeft];
	QPoint br = points[QWSPointerCalibrationData::BottomRight];

//	qDebug() << "TOP LEFT:" << tl.x() << tl.y();
//	qDebug() << "TOP RIGHT:" << tr.x() << tr.y();
//	qDebug() << "BOTTOM LEFT:" << bl.x() << bl.y();
//	qDebug() << "BOTTOM RIGHT:" << br.x() << br.y();

	// Calculate the error on the x axis
	int left_error = qAbs(tl.x() - bl.x());
	int right_error = qAbs(tr.x() - br.x());

	// We use the average point on the x axis as an approximation of the screen size
	// (we haven't the screen size in raw device coordinates)
	int avg = qMax((tl.x() + bl.x()) / 2, (tr.x() + br.x()) / 2);

	if (qMax(left_error, right_error) > avg / 10)
	{
		qDebug() << "Calibration: the error on the x axis is greater than 10%";
		return false;
	}

	// Calculate the error on the y axis
	left_error = qAbs(tl.y() - tr.y());
	right_error = qAbs(br.y() - bl.y());

	// See the comment above for the meaning of avg
	avg = qMax((tl.y() + tr.y()) / 2, (br.y() + bl.y()) / 2);

	if (qMax(left_error, right_error) > avg / 10)
	{
		qDebug() << "Calibration: the error on the y axis is greater than 10%";
		return false;
	}

	// The x on the left (in raw device coordinates) must be greater than
	// the x on the right
	if (tl.x() < tr.x() || bl.x() < br.x())
	{
		qDebug() << "Calibration: left and right inverted";
		return false;
	}

#ifdef BT_HARDWARE_TOUCHX
	// The y on the top (in raw device coordinates) must be greater than
	// the y on the bottom
	if (tl.y() < bl.y() || tr.y() < br.y())
#else
	// The y on the top (in raw device coordinates) must be smaller than
	// the y on the bottom
	if (tl.y() > bl.y() || tr.y() > br.y())
#endif
	{
		qDebug() << "Calibration: top and bottom inverted";
		return false;
	}

	return true;
}

