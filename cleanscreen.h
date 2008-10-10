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

#include <qwidget.h>
#include <qtimer.h>

class CleanScreen : public QWidget
{
Q_OBJECT
public:
    CleanScreen(QWidget *parent=0);
private:
	void mousePressEvent(QMouseEvent *e);
	void showEvent(QShowEvent *e);
	QTimer timer;

signals:
	void Closed();
};

#endif

