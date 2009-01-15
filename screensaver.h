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

#include <QObject>
#include <QHash>

class Page;
class QLabel;
class QTimer;


/**
 * This abstract class is the interface of all screensavers.
 */
class ScreenSaver : protected QObject
{
Q_OBJECT
public:
	enum Type
	{
		NONE = 0,   // no screensaver
		LINES,       // single line that goes up and down
		BALLS,      // many balls on screen
		TIME,       // a single line with a clock inside
	};

	virtual void start(Page *p);
	virtual void stop();
	bool isRunning();

protected:
	ScreenSaver(int refresh_time);
	Page *page;

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

protected slots:
	virtual void refresh();

protected:
	QLabel *line;

	virtual QString styleDownToUp();
	virtual QString styleUpToDown();

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

protected:
	virtual QString styleDownToUp();
	virtual QString styleUpToDown();
};

#endif
