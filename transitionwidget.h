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
class WindowContainer;


/**
 * This is the base (abstract) class for all transition widget.
 */
class TransitionWidget : public QWidget
{
Q_OBJECT
public:
	TransitionWidget(int time);

	// must be called before altering the state of the page
	// grabs a screenshot of the current state of the HomeWidget and then
	// shows itself as the top level widget
	void prepareTransition();

	// must be called after altering the state of the page to the final
	// state; grabs a screenshot of the final state of the HomeWidget and
	// starts performing the transition
	void startTransition();

	void cancelTransition();

	void setContainer(WindowContainer *container);

signals:
	void endTransition();

protected:
	QTimeLine timeline;
	QPixmap prev_image;
	QPixmap dest_image;

	virtual void initTransition() {}

private:
	WindowContainer *container;
	QEventLoop local_loop;
};


/**
 * Do a blend transition between two widgets
 */
class BlendingTransition : public TransitionWidget
{
Q_OBJECT
public:
	BlendingTransition();

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
	MosaicTransition();

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
