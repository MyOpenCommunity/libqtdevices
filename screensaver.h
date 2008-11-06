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
#include <QPixmap>

class QLabel;

#define BALL_NUM 5

/**
 * This abstract class is the interface of all screensavers.
 */
class ScreenSaver : protected QWidget
{
Q_OBJECT
public:
	using QWidget::hide;
	using QWidget::show;
	using QWidget::isHidden;
	using QWidget::showFullScreen;

	virtual ~ScreenSaver() {}
	virtual void refresh(const QPixmap& bg_image) = 0;

	enum Type
	{
		NONE = 0,       // no screensaver
		LINES = 1,      // single line that goes up and down
		BALLS = 2,      // many balls on screen
	};
};

/// The factory method that build the screensaver of type 'ScreenSaver::Type'
ScreenSaver *getScreenSaver(ScreenSaver::Type type);

/**
 * The concrete class that represent a screensaver with 'balls'.
 */
class ScreenSaverBalls : public ScreenSaver
{
Q_OBJECT
public:
	virtual void refresh(const QPixmap& bg_image);

private:
	// Only the factory method can instantiate it!
	friend ScreenSaver *getScreenSaver(ScreenSaver::Type type);
	ScreenSaverBalls();

	QLabel *ball[BALL_NUM];
	int x[BALL_NUM], y[BALL_NUM], vx[BALL_NUM], vy[BALL_NUM], dim[BALL_NUM];
	int backcol;
};

/**
 * The concrete class that represent a screensaver with a scrolling line.
 */
class ScreenSaverLine : public ScreenSaver
{
Q_OBJECT
public:
	virtual void refresh(const QPixmap& bg_image);

private:
	// Only the factory method can instantiate it!
	friend ScreenSaver *getScreenSaver(ScreenSaver::Type type);
	ScreenSaverLine();

	int backcol;
	QLabel *line;
	bool black_line;
	int y;
};

#endif
