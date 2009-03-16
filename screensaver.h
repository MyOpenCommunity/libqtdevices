/*!
 * \file
 * <!--
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 * All rights reserved.
 * -->
 *
 * \author Gianni Valdambrini <aleister@develer.com>
 */

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QWidget>
#include <QObject>
#include <QHash>
#include <QBasicTimer>
#include <QDateTime>
#include <QPixmap>
#include <QImage>
#include <QPointF>
#include <QTime>
#include <QVector>

class Page;
class QLabel;
class QTimer;
class QDomNode;


/**
 * This abstract class is the interface of all screensavers.
 */
class ScreenSaver : protected QWidget
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
		DEFORM,     // the deformer
	};

	virtual void start(Page *p);
	virtual void stop();
	bool isRunning();
	virtual Type type() = 0;
	Page *target() { return page; }
	static void initData(const QDomNode &config_node);

protected:
	Page *page;
	static QString text;
	ScreenSaver(int refresh_time);

protected slots:
	virtual void refresh() = 0;

private:
	QTimer *timer;
};


/// The factory method that build the screensaver of type 'ScreenSaver::Type'
ScreenSaver *getScreenSaver(ScreenSaver::Type type);


/**
 * The concrete class that represent a screensaver with a scrolling line.
 */
class ScreenSaverLine : public ScreenSaver
{
Q_OBJECT
public:
	ScreenSaverLine();
	virtual void start(Page *p);
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

private:
	bool up_to_down; // true if the direction of the movement is from up to down
	int y;
};


/**
 * The concrete class that represent a screensaver with 'balls'.
 */
class ScreenSaverBalls : public ScreenSaver
{
Q_OBJECT
public:
	ScreenSaverBalls();
	virtual void start(Page *p);
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


/**
 * The concrete class that represent a screensaver with a scrolling line and a
 *.time clock inside.
 */
class ScreenSaverTime : public ScreenSaverLine
{
Q_OBJECT
public:
	virtual void start(Page *p);
	virtual Type type() { return TIME; }
};


/**
 * The concrete class that represent a screensaver with a scrolling line and a
 *.text inside.
 */
class ScreenSaverText : public ScreenSaverLine
{
Q_OBJECT
public:
	virtual void start(Page *p);
	virtual Type type() { return TEXT; }
};


/**
 * The concrete class that represent a deformer screensaver.
 */
class ScreenSaverDeform : public ScreenSaver
{
Q_OBJECT
public:
	ScreenSaverDeform();
	virtual void start(Page *p);
	virtual void stop();
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
