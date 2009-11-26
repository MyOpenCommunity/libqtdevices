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

