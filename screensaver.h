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


/*!
	\ingroup Core
	\class ScreenSaver
	\brief Base class for all screensavers.

	To create a new screensaver, the \a type() and the \a refresh() method must
	be reimplemented and a new entry must be added to the \a ScreenSaver::Type
	enum.

	A \a ScreenSaver is a \a Window subclass. It can be started using the
	\a start() method which takes the \a Window the screensaver is
	running on as parameter. The screensaver can be stopped by calling
	the \a stop() method. To check if the screensaver is currently running the
	method \a isRunning() can be used.

	The refresh interval can be set by the \a setRefreshInterval() method.
*/
class ScreenSaver : protected Window
{
Q_OBJECT
public:
	enum Type
	{
		NONE = 0,   /*!< no screensaver */
		LINES,      /*!< single line that goes up and down */
		BALLS,      /*!< many balls on screen */
		TIME,       /*!< a single line with a clock inside */
		TEXT,       /*!< a line with a text */
		SLIDESHOW,  /*!< image slideshow */
		DEFORM,     /*!< the deformer */
		LOGO,       /*!< bouncing logo */
	};

	/*!
		\brief Shows the screensaver and starts the updates.

		\a w is the \a Window the screensaver will be started on.
	*/
	virtual void start(Window *w);

	/*!
		\brief Stops the screensaver.

		Updates are stopped and the Closed() signal is emitted.
	*/
	virtual void stop();

	/*!
		\brief Returns true if the screensaver is currently running.
	*/
	bool isRunning();

	/*!
		\brief Returns the ScreenSaver::Type of the screensaver,

		Must be reimplemented into subclasses.
	*/
	virtual Type type() = 0;

	/*!
		\brief Init the static members.

		Read the configuration node \a config_node and sets the screensaver static
		members.

		\sa ScreenSaver::text, ScreenSaver::slideshow_timeout
	 */
	static void initData(const QDomNode &config_node);

	/*!
		\brief Sets the interval between two images into the slideshow screensaver.
	*/
	static void setSlideshowInterval(int interval);

protected:
	Window *window;
	static QString text;
	static int slideshow_timeout;

	/*!
		\brief Constructor

		Should be called by \a ScreenSaver subclasses that need periodical refreshes.
	*/
	ScreenSaver(int refresh_time);

	/*!
		\brief Starts to periodically call the refresh() method.

		\sa ScreenSaver::refresh(), ScreenSaver()
	*/
	void startRefresh();

	/*!
		\brief Stops to periodically call the refresh() method.
	*/
	void stopRefresh();

	/*!
		\brief Sets the interval at with call the refresh() method.

		\sa ScreenSaver::refresh(), ScreenSaver()
	*/
	void setRefreshInterval(int msecs);

protected slots:
	/*!
		\brief Refreshes the screensaver.

		Must be reimplemented into subclasses to perform state update operations.
	*/
	virtual void refresh() = 0;

private:
	QTimer *timer;
};


/*!
	\ingroup Core
	\brief Instantiate a screensaver.

	This factory method returns an instance of the screensaver specified by
	\a type.
	If \a type doesn't exists dies with a \a qFatal().
 */
ScreenSaver *getScreenSaver(ScreenSaver::Type type);


/*!
	\ingroup Core
	\class ScreenSaverLine
	\brief The concrete class that represent a screensaver with a scrolling line.
*/
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

	/*!
		\brief Method to set the style of the inner widget displayed by the
		ScreenSaver when it is moving down to up.
	*/
	virtual QString styleDownToUp();

	/*!
		\brief Method to set the style of the inner widget displayed by the
		ScreenSaver when it is moving up to down.
	*/
	virtual QString styleUpToDown();

	/*!
		\brief Method to resize the innter widget displayed by the \a ScreenSaver to
		the given height.
	*/
	void setLineHeight(int height);

	/*!
		\brief Method to customize the inner widget displayed by the ScreenSaver.
	*/
	virtual void customizeLine();

private:
	bool up_to_down; // true if the direction of the movement is from up to down
	int y;
};


/*!
	\ingroup Core
	\class ScreenSaverBalls
	\brief The concrete class that represent a screensaver with 'balls'.
*/
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


/*!
	\ingroup Core
	\class ScreenSaverTime
	\brief The concrete class that represent a screensaver with a scrolling line and a
	time clock inside.
*/
class ScreenSaverTime : public ScreenSaverLine
{
Q_OBJECT
public:
	virtual Type type() { return TIME; }
protected:
	virtual void customizeLine();
};

/*!
	\ingroup Core
	\class ScreenSaverText
	\brief The concrete class that represent a screensaver with a scrolling line and a
	text inside.
*/
class ScreenSaverText : public ScreenSaverLine
{
Q_OBJECT
public:
	virtual Type type() { return TEXT; }
protected:
	virtual void customizeLine();
};


/*!
	\ingroup Core
	\class ScreenSaverSlideshow
	\brief The concrete class that represent a slideshow screensaver.
*/
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


/*!
	\class ScreenSaverDeform
	\brief The screensaver composed by a lens that deforms the underlying window.
*/
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
