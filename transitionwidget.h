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


#ifndef TRANSITIONWIDGET_H
#define TRANSITIONWIDGET_H

#include <QEventLoop>
#include <QWidget>
#include <QTimeLine>
#include <QList>
#include <QRect>

class Page;
class QStackedWidget;


/**
 * This is the base (abstract) class for all transition widget.
 */
class TransitionWidget : public QWidget
{
friend class Page;
Q_OBJECT
public:
	TransitionWidget(QStackedWidget *win, int time);
	void setStartingPage(Page *prev);
	void startTransition(Page *next);

protected:
	QStackedWidget *main_window;
	QTimeLine timeline;
	QPixmap prev_image;
	QPixmap next_image;

	virtual void initTransition() {}

private slots:
	void transitionEnd();

private:
	Page *prev_page;
	Page *dest_page;
	QEventLoop local_loop;

	void cancelTransition();
};


/**
 * Do a blend transition between two widgets
 */
class BlendingTransition : public TransitionWidget
{
Q_OBJECT
public:
	BlendingTransition(QStackedWidget *win);

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void initTransition();

private slots:
	void triggerRepaint(qreal);

private:
	qreal blending_factor;

};

/**
 * Do a mosaic transition between two widgets
 */
class MosaicTransition : public TransitionWidget
{
Q_OBJECT
public:
	MosaicTransition(QStackedWidget *win);

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void initTransition();

private slots:
	void triggerRepaint(int index);

private:
	int curr_index;
	int prev_index;
	QPixmap dest_pix;
	QList<QRect> mosaic_map;
};

#endif // TRANSITIONWIDGET_H
