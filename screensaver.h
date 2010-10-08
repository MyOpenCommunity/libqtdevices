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


#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include "window.h"

#include <QObject>
#include <QHash>
#include <QBasicTimer>
#include <QDateTime>
#include <QPixmap>
#include <QImage>
#include <QPointF>
#include <QTime>
#include <QVector>
#include <QTimeLine>
#include <QPointer>
#include <QFutureWatcher>

class Page;
class QLabel;
class QTimer;
class QDomNode;
class ImageIterator;


class ScreenSaver : protected Window
{
Q_OBJECT
public:
	enum Type
	{
		NONE = 0,   // no screensaver
		LINES,      // single line that goes up and down
		BALLS,      // many balls on screen
		TIME,       // a single line with a clock inside
		TEXT,       // a line with a text
		SLIDESHOW,  // image slideshow
		DEFORM,     // the deformer
	};

	virtual void start(Window *w);
	virtual void stop();
	bool isRunning();
	virtual Type type() = 0;
	static void initData(const QDomNode &config_node);
	static void setSlideshowInterval(int interval);

protected:
	Window *window;
	static QString text;
	ScreenSaver(int refresh_time);
	static int slideshow_timeout;
	void startRefresh();
	void stopRefresh();
	void setRefreshInterval(int msecs);

protected slots:
	virtual void refresh() = 0;

private:
	QTimer *timer;
};


ScreenSaver *getScreenSaver(ScreenSaver::Type type);


class ScreenSaverLine : public ScreenSaver
{
Q_OBJECT
public:
	ScreenSaverLine();
	virtual void start(Window *w);
	virtual void stop();
	virtual Type type() { return LINES; }

protected slots:
	virtual void refresh();

protected:
	QLabel *line;
	int line_height;

	virtual QString styleDownToUp();
	virtual QString styleUpToDown();
	void setLineHeight(int height);
	virtual void customizeLine();

private:
	bool up_to_down; // true if the direction of the movement is from up to down
	int y;
};


class ScreenSaverBalls : public ScreenSaver
{
Q_OBJECT
public:
	ScreenSaverBalls();
	virtual void start(Window *w);
	virtual void stop();
	virtual Type type() { return BALLS; }

protected slots:
	virtual void refresh();

private:
	struct BallData
	{
		int x, y, vx, vy, dim;
	};

	QHash<QLabel*, BallData> ball_list;

	void initBall(QLabel* ball, BallData& data);
};


class ScreenSaverTime : public ScreenSaverLine
{
Q_OBJECT
public:
	virtual Type type() { return TIME; }
protected:
	virtual void customizeLine();
};


class ScreenSaverText : public ScreenSaverLine
{
Q_OBJECT
public:
	virtual Type type() { return TEXT; }
protected:
	virtual void customizeLine();
};


class ScreenSaverSlideshow : public ScreenSaver
{
Q_OBJECT
public:
	ScreenSaverSlideshow();
	virtual void start(Window *w);
	virtual void stop();
	virtual Type type() { return SLIDESHOW; }
	virtual ~ScreenSaverSlideshow();

protected:
	virtual void paintEvent(QPaintEvent *e);

protected slots:
	virtual void refresh();

private slots:
	void imageReady();

private:
	// iterator for images
	ImageIterator *iter;
	// shows the image on the window
	QPixmap current_image, next_image;
	QTimeLine blending_timeline;
	QPointer<QFutureWatcher<QImage> > async_load;
	qreal opacity;
	QString last_image_file;

private slots:
	void updateOpacity(qreal new_value);
};


class ScreenSaverDeform : public ScreenSaver
{
Q_OBJECT
public:
	ScreenSaverDeform();
	virtual void start(Window *w);
	virtual Type type() { return DEFORM; }

protected:
	virtual void timerEvent(QTimerEvent *e);
	virtual void paintEvent(QPaintEvent *e);

protected slots:
	virtual void refresh();

private:
	void generateLensPixmap();
	void buildLookupTable();

	int radius;
	QPointF current_pos;
	QPointF direction;
	int font_size;
	QBasicTimer repaint_timer;
	QTime repaint_tracker;
	qreal deformation;
	QPixmap lens_pixmap;
	QVector<QVector<QPoint> > lens_lookup_table;
	QImage bg_image, canvas_image;
	bool need_refresh;
};

#endif
