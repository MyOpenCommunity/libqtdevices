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


#ifndef CLEANSCREEN_H
#define CLEANSCREEN_H

#include "window.h"

#include <QDateTime>
#include <QTimer>


class QLabel;


class CleanScreen : public Window
{
Q_OBJECT
public:
	CleanScreen(QString img_clean, int clean_time);

public slots:
	virtual void showWindow();

private:
	void paintEvent(QPaintEvent *e);

	/// Timer that controls show/hide of widget
	QTimer timer;
	/// Timer that updates the widget every second
	//(see analog clock example)
	QTimer secs_timer;
	/// Wait time in seconds read from conf file
	int wait_time_sec;
	/// Used to compute the remaining time of cleanscreen
	// We can't use QTime.elapsed() because its behaviour is undefined if the system
	// date change after start()
	int secs_counter;
	/// labels to show remaining time and cleanscreen icon
	QLabel *time_label, *icon_label;

private slots:
	void handleClose();
	void updateRemainingTime();
};

#endif

