/*!
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef CLEANSCREEN_H
#define CLEANSCREEN_H

#include "page.h"

#include <QTimer>

#include <time.h> //time_t and time()


class QLabel;


class CleanScreen : public Page
{
Q_OBJECT
public:
	CleanScreen(int clean_time);
private:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void showEvent(QShowEvent *e);
	void paintEvent(QPaintEvent *e);
	void resetTime();
	/// Timer that controls show/hide of widget
	QTimer timer;
	/// Timer that updates the widget every second
	//(see analog clock example)
	QTimer secs_timer;
	/// Wait time in seconds read from conf file
	int wait_time_sec;
	/// Used to compute the remaining time of cleanscreen
	time_t end_time;
	/// labels to show remaining time and cleanscreen icon
	QLabel *time_label, *icon_label;

	static const unsigned TIME_LABEL_X = 80;
	static const unsigned TIME_LABEL_Y = 160;
	static const unsigned TIME_LABEL_WIDTH = 80;
	static const unsigned TIME_LABEL_HEIGHT = 40;

	static const unsigned ICON_LABEL_X = 80;
	static const unsigned ICON_LABEL_Y = 20;
	static const unsigned ICON_LABEL_WIDTH = 80;
	static const unsigned ICON_LABEL_HEIGHT = 80;

signals:
	void Closed();
};

#endif

