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

#include <time.h> //time_t and time()
#include <qwidget.h>
#include <qtimer.h>

class BtLabelEvo;

class CleanScreen : public QWidget
{
Q_OBJECT
public:
	CleanScreen(QWidget *parent=0);
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
	unsigned wait_time_sec;
	/// Used to compute the remaining time of cleanscreen
	time_t end_time;
	/// labels to show remaining time and cleanscreen icon
	BtLabelEvo *time_label, *icon_label;

	static const unsigned TIME_LABEL_X = 80;
	static const unsigned TIME_LABEL_Y = 160;
	static const unsigned TIME_LABEL_WIDTH = 80;
	static const unsigned TIME_LABEL_HEIGHT = 40;

	static const unsigned ICON_LABEL_X = 80;
	static const unsigned ICON_LABEL_Y = 20;
	static const unsigned ICON_LABEL_WIDTH = 80;
	static const unsigned ICON_LABEL_HEIGHT = 80;

private slots:
	void timerElapsed();

signals:
	void Closed();
};

#endif

