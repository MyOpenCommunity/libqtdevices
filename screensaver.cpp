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


#include "screensaver.h"
#include "page.h"
#include "timescript.h"
#include "fontmanager.h" // bt_global::font
#include "xml_functions.h"
#include "homewindow.h"
#include "pagestack.h"
#include "generic_functions.h" // checkImageLoad, getFileFilter
#include "imageselectionhandler.h"

#include <QVBoxLayout>
#include <QDomNode>
#include <QPainter>
#include <QBitmap>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QPaintEvent>
#include <qmath.h>
#include <QDir>
#include <QtConcurrentRun>


#include <stdlib.h> // RAND_MAX
#define BALL_NUM 5

namespace
{
	inline void setRandomColor(QWidget *w)
	{
		QColor bg = QColor((int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
						   (int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
						   (int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150);

		QString style = QString("QWidget {background-color:%1;}").arg(bg.name());
		w->setStyleSheet(style);
	}

	inline QPoint centeredOrigin(QRect container, QRect rect)
	{
		int x = (container.width() - rect.width()) / 2;
		int y = (container.height() - rect.height()) / 2;

		return QPoint(x, y);
	}

	inline QRect circle_bounds(const QPointF &center, qreal radius, qreal compensation)
	{
		return QRect(qRound(center.x() - radius - compensation),
					 qRound(center.y() - radius - compensation),
					 qRound((radius + compensation) * 2),
					 qRound((radius + compensation) * 2));
	}

	QImage loadImageScaled(const QString &image, const QSize &size)
	{
		if (!checkImageLoad(image))
			return QImage();

		return QImage(image).scaled(size, Qt::KeepAspectRatio);
	}
}

/*!
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

/*!
	\enum ScreenSaver::Type
	Screensaver types.
*/
/*!
	\var ScreenSaver::Type ScreenSaver::NONE
	no screensaver
*/
/*!
	\var ScreenSaver::Type ScreenSaver::LINES
	single line that goes up and down
*/
/*!
	\var ScreenSaver::Type ScreenSaver::BALLS
	many balls on screen
*/
/*!
	\var ScreenSaver::Type ScreenSaver::TIME
	a single line with a clock inside
*/
/*!
	\var ScreenSaver::Type ScreenSaver::TEXT
	a line with a text
*/
/*!
	\var ScreenSaver::Type ScreenSaver::SLIDESHOW
	image slideshow
*/
/*!
	\var ScreenSaver::Type ScreenSaver::DEFORM
	the deformer
*/

/*!
	\brief Instantiate a screensaver.

	This factory method returns an instance of the screensaver specified by
	\a type.
	If \a type doesn't exists dies with a \a qFatal().
 */
ScreenSaver *getScreenSaver(ScreenSaver::Type type)
{
	switch (type)
	{
	case ScreenSaver::BALLS:
		return new ScreenSaverBalls;
	case ScreenSaver::LINES:
		return new ScreenSaverLine;
	case ScreenSaver::TIME:
		return new ScreenSaverTime;
	case ScreenSaver::TEXT:
		return new ScreenSaverText;
	case ScreenSaver::SLIDESHOW:
		return new ScreenSaverSlideshow;
	case ScreenSaver::DEFORM:
		return new ScreenSaverDeform;
	case ScreenSaver::NONE:
		return 0;
	default:
		qFatal("Type of screensaver not handled!");
	}
	return 0; // Only to silent warning from compiler
}

//! Screensaver text
QString ScreenSaver::text;

//! Slideshow screensaver timeout between different images
int ScreenSaver::slideshow_timeout;


/*!
	\brief Constructor

	Should be called by \a ScreenSaver subclasses that need periodical refreshes.
 */
ScreenSaver::ScreenSaver(int refresh_time)
{
	window = 0;
	timer = new QTimer(this);
	timer->setInterval(refresh_time);
	connect(timer, SIGNAL(timeout()), SLOT(refresh()));
}

/*!
	\brief Starts to periodically call the \a refresh() method.

	\sa ScreenSaver::refresh(), ScreenSaver()
 */
void ScreenSaver::startRefresh()
{
	timer->start();
}

/*!
	\brief Stops to periodically call the \a refresh() method.
 */
void ScreenSaver::stopRefresh()
{
	timer->stop();
}

/*!
	\brief Sets the interval at with call the \a refersh() method.

	\sa ScreenSaver::refresh(), ScreenSaver()
 */
void ScreenSaver::setRefreshInterval(int msecs)
{
	timer->setInterval(msecs);
}

/*!
	\brief Shows the screensaver and starts the updates.

	\a w is the \a Window the screensaver will be started on.
 */
void ScreenSaver::start(Window *w)
{
	bt_global::page_stack.showUserWindow(this);
	window = w;
	timer->start();
}

/*!
	\brief Stops the screensaver.

	Updates are stopped and the \a Closed() signal is emitted.
 */
void ScreenSaver::stop()
{
	window = 0;
	timer->stop();
	emit Closed(); // for PageStack to catch
}

/*!
	\brief Returns true if the screensaver is currently running.
 */
bool ScreenSaver::isRunning()
{
	return window != 0;
}

/*!
	\fn ScreenSaver::type()
	\brief Returns the ScreenSaver::Type of the screensaver

	Must be reimplemented into subclasses.
 */

/*!
	\brief Init the static members

	Read the configuration node \a config_node and sets the screensaver static
	members.

	\sa ScreenSaver::text, ScreenSaver::slideshow_timeout
 */
void ScreenSaver::initData(const QDomNode &config_node)
{
	text = getTextChild(config_node, "text");
#ifndef CONFIG_TS_3_5
	QString time = getTextChild(config_node, "timeSlideShow");
	Q_ASSERT_X(!time.isEmpty(), "ScreenSaver::initData", "config node does not contain timeSlideShow leaf");
	slideshow_timeout = time.toInt();
#endif
}

/*!
	\brief Sets the interval between two images into the slideshow screensaver.
 */
void ScreenSaver::setSlideshowInterval(int interval)
{
	slideshow_timeout = interval;
}

/*!
	\fn ScreenSaver::refresh()
	\brief Refreshes the screensaver.

	Must be reimplemented into subclasses to perform state update operations.
 */


/*!
	\class ScreenSaverBalls
	\brief The concrete class that represent a screensaver with 'balls'.
 */
ScreenSaverBalls::ScreenSaverBalls() : ScreenSaver(120)
{
}

void ScreenSaverBalls::start(Window *w)
{
	ScreenSaver::start(w);
	for (int i = 0; i < BALL_NUM; ++i)
	{
		QLabel *l = new QLabel(w);
		ball_list[l] = BallData();
		initBall(l, ball_list[l]);
		l->show();
	}
}

void ScreenSaverBalls::initBall(QLabel *ball, BallData &data)
{
	data.x = (int)(200.0 * rand() / (RAND_MAX + 1.0));
	data.y = (int)(200.0 * rand() / (RAND_MAX + 1.0));
	data.vx = (int)(8.0 * rand() / (RAND_MAX + 1.0)) - 8;
	data.vy = (int)(8.0 * rand() / (RAND_MAX + 1.0)) - 8;
	if (data.vx == 0)
		data.vx = 1;
	if (data.vy == 0)
		data.vy = 1;

	data.dim = (int)(10.0 * rand() / (RAND_MAX + 1.0)) + 15;
	ball->resize(data.dim, data.dim);

	QBitmap mask = QBitmap(data.dim, data.dim);
	mask.clear();
	QPainter p(&mask);
	p.setRenderHints(QPainter::NonCosmeticDefaultPen | QPainter::Antialiasing, true);
	p.setBrush(QBrush(Qt::color1, Qt::SolidPattern));

	QPointF center = QPointF(data.dim / 2.0, data.dim / 2.0);
	p.drawEllipse(center, data.dim / 2.0, data.dim / 2.0);

	ball->setMask(mask);
	setRandomColor(ball);
}

void ScreenSaverBalls::stop()
{
	ScreenSaver::stop();
	QMutableHashIterator<QLabel*, BallData> it(ball_list);
	while (it.hasNext())
	{
		it.next();
		delete it.key();
	}

	ball_list.clear();
}

void ScreenSaverBalls::refresh()
{
	QMutableHashIterator<QLabel*, BallData> it(ball_list);
	while (it.hasNext())
	{
		it.next();
		BallData& data = it.value();
		data.x += data.vx;
		data.y += data.vy;

		bool change_style = false;
		if (data.x <= 0)
		{
			data.vx = static_cast<int>(8.0 * rand() / (RAND_MAX + 1.0)) + 3;
			data.x = 0;
			change_style = true;
		}
		if (data.y > window->height() - data.dim)
		{
			data.vy = static_cast<int>(8.0 * rand() / (RAND_MAX + 1.0)) - 8;
			data.y = window->height() - data.dim;
			change_style = true;
		}

		if (data.y <= 0)
		{
			data.vy = static_cast<int>(8.0 * rand() / (RAND_MAX + 1.0)) + 3;
			if (data.vy == 0)
				data.vy = 1;
			data.y = 0;
			change_style = true;
		}
		if (data.x > window->width() - data.dim)
		{
			data.vx = static_cast<int>(8.0 * rand() / (RAND_MAX + 1.0)) - 8;
			if (data.vx == 0)
				data.vx = 1;
			data.x = window->width() - data.dim;
			change_style = true;
		}

		if (change_style)
			setRandomColor(it.key());

		it.key()->move(data.x, data.y);
	}
}

/*!
	\class ScreenSaverLine
	\brief The concrete class that represent a screensaver with a scrolling line.
 */
ScreenSaverLine::ScreenSaverLine() : ScreenSaver(150)
{
	line = 0;
}

void ScreenSaverLine::start(Window *w)
{
	ScreenSaver::start(w);
	line = new QLabel(w);
	customizeLine();
	y = 0;
	up_to_down = true;
	line->setStyleSheet(styleUpToDown());
	line->show();
}

/*!
	\brief Method to customize the inner widget displayed by the \a ScreenSaver.
 */
void ScreenSaverLine::customizeLine()
{
	setLineHeight(6);
}

/*!
	\brief Method to resize the innter widget displayed by the \a ScreenSaver to
	the given height.
 */
void ScreenSaverLine::setLineHeight(int height)
{
	line->resize(window->width(), height);
	line_height = height;
}

void ScreenSaverLine::stop()
{
	ScreenSaver::stop();
	delete line;
}

void ScreenSaverLine::refresh()
{
	if (y > window->height())
	{
		y = window->height();
		up_to_down = false;
		line->setStyleSheet(styleDownToUp());
	}

	if (y + line_height < 0)
	{
		y = - line_height;
		up_to_down = true;
		line->setStyleSheet(styleUpToDown());
	}

	if (up_to_down)
		y += 3;
	else
		y -= 3;

	line->move(0, y);
}

/*!
	\brief Method to set the style of the inner widget displayed by the
	\a ScreenSaver when it is moving down to up.
 */
QString ScreenSaverLine::styleDownToUp()
{
	return "* {background-color:#000000; color:#FFFFFF; }";
}

/*!
	\brief Method to set the style of the inner widget displayed by the
	\a ScreenSaver when it is moving up to down.
 */
QString ScreenSaverLine::styleUpToDown()
{
	return "* {background-color:#FFFFFF; color:#000000; }";
}

/*!
	\class ScreenSaverTime
	\brief The concrete class that represent a screensaver with a scrolling line and a
	time clock inside.
 */
void ScreenSaverTime::customizeLine()
{
	setLineHeight(30);
	timeScript *time = new timeScript(line, 1);
	time->setFont(bt_global::font->get(FontManager::TEXT));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(0, 2, 0, 2);
	layout->addWidget(time, 0, Qt::AlignCenter);
	line->setLayout(layout);
}


/*!
	\class ScreenSaverText
	\brief The concrete class that represent a screensaver with a scrolling line and a
	text inside.
 */
void ScreenSaverText::customizeLine()
{
	setLineHeight(30);
	line->setFont(bt_global::font->get(FontManager::TEXT));
	line->setAlignment(Qt::AlignCenter);
	line->setText(text);
}


/*!
	\class ScreenSaverSlideshow
	\brief The concrete class that represent a slideshow	 screensaver.
 */
ScreenSaverSlideshow::ScreenSaverSlideshow() :
	ScreenSaver(slideshow_timeout)
{
	blending_timeline.setDuration(2000);
	blending_timeline.setFrameRange(0, 1);
	connect(&blending_timeline, SIGNAL(valueChanged(qreal)), SLOT(updateOpacity(qreal)));
}

void ScreenSaverSlideshow::start(Window *w)
{
	setRefreshInterval(slideshow_timeout);
	iter = new ImageIterator(SLIDESHOW_FILENAME);
	iter->setFileFilter(getFileFilter(IMAGE));
	ScreenSaver::start(w);
	showWindow();
	next_image = QPixmap(width(), height());
	last_image_file = QString();
	// TODO: take background from skin
	next_image.fill(QColor(Qt::black));
	// this call will update both current_image and next_image
	refresh();
}

void ScreenSaverSlideshow::stop()
{
	blending_timeline.stop();
	ScreenSaver::stop();
	if (iter)
	{
		iter->saveImagesToFile();
		delete iter;
		iter = 0;
	}

	if (async_load)
		async_load->deleteLater();
}

void ScreenSaverSlideshow::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	p.fillRect(e->rect(), Qt::black);
	p.setRenderHint(QPainter::SmoothPixmapTransform, false);

	p.setOpacity(1.0 - opacity);
	p.drawPixmap(centeredOrigin(e->rect(), current_image.rect()), current_image);

	p.setOpacity(opacity);
	p.drawPixmap(centeredOrigin(e->rect(), next_image.rect()), next_image);
}

void ScreenSaverSlideshow::refresh()
{
	QString img;

	if (iter && iter->hasNext())
		img = iter->next();
	else
	{
		qDebug() << "no images, blank screen.";
		stopRefresh();
		current_image.fill(Qt::black);
		next_image.fill(Qt::black);
		update();
		return;
	}

	if (!img.isEmpty())
	{
		// Check if the last image is equal to the current one.
		// This could be possible if there is only one image.
		// In this case, does nothing (no load, no transition, etc.)
		if (img != last_image_file)
		{
			last_image_file = img;
			current_image = next_image;
			if (QFile::exists(img))
			{
				if (async_load)
					async_load->deleteLater();

				async_load = new QFutureWatcher<QImage>();
				connect(async_load, SIGNAL(finished()), SLOT(imageReady()));

				async_load->setFuture(QtConcurrent::run(&loadImageScaled, img, size()));
			}
		}
	}
	else
	{
		qWarning() << "image is empty: ";
		refresh();
	}
}

void ScreenSaverSlideshow::imageReady()
{
	next_image = QPixmap::fromImage(async_load->result());
	async_load->deleteLater();

	if (!next_image.isNull())
		blending_timeline.start();
}

void ScreenSaverSlideshow::updateOpacity(qreal new_value)
{
	opacity = new_value;
	update();
}

ScreenSaverSlideshow::~ScreenSaverSlideshow()
{
	if (async_load)
		async_load->deleteLater();
}


/*!
	\class ScreenSaverDeform
	\brief The concrete class that represent a deformer screensaver.
 */
ScreenSaverDeform::ScreenSaverDeform() : ScreenSaver(500)
{
	radius = 30;
	current_pos = QPointF(radius, radius);
	direction = QPointF(1, 1);
	font_size = 24;
	repaint_timer.start(50, this);
	repaint_tracker.start();
	deformation = 60;
	need_refresh = true;

	buildLookupTable();
	generateLensPixmap();
}

void ScreenSaverDeform::start(Window *w)
{
	ScreenSaver::start(w);
	showWindow();
	refresh();
	raise();
}

void ScreenSaverDeform::refresh()
{
	QImage tmp_image(window->size(), QImage::Format_RGB16);
	window->render(&tmp_image);
	// Copy the QImage is not a problem, thanks to implicit data sharing.
	bg_image = tmp_image;
	need_refresh = true;
	update();
}

void ScreenSaverDeform::buildLookupTable()
{
	lens_lookup_table.resize(radius * 2);

	for (int x = -radius; x < radius; ++x)
	{
		lens_lookup_table[x + radius].resize(radius * 2);

		for (int y = -radius; y < radius; ++y)
		{
			qreal flip = deformation / qreal(100);
			qreal len = qSqrt(x * x + y * y) - radius;

			int pickx, picky;

			if (len < 0)
			{
				pickx = static_cast<int>(x - flip * x * (-len) / radius);
				picky = static_cast<int>(y - flip * y * (-len) / radius);
			}
			else
			{
				pickx = x;
				picky = y;
			}
			lens_lookup_table[x + radius][y + radius] = QPoint(pickx, picky);
		}
	}
}

void ScreenSaverDeform::generateLensPixmap()
{
	qreal rad = radius;

	QRect bounds = circle_bounds(QPointF(), rad, 0);

	QPainter painter;

	lens_pixmap = QPixmap(bounds.size());
	lens_pixmap.fill(Qt::transparent);
	painter.begin(&lens_pixmap);

	QRadialGradient gr(rad, rad, rad, 3 * rad / 5, 3 * rad / 5);
	gr.setColorAt(0.0, QColor(255, 255, 255, 191));
	gr.setColorAt(0.2, QColor(255, 255, 127, 191));
	gr.setColorAt(0.9, QColor(150, 150, 200, 63));
	gr.setColorAt(0.95, QColor(0, 0, 0, 127));
	gr.setColorAt(1, QColor(0, 0, 0, 0));
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(gr);
	painter.setPen(Qt::NoPen);
	painter.drawEllipse(0, 0, bounds.width(), bounds.height());
}

void ScreenSaverDeform::timerEvent(QTimerEvent *)
{
	if (QLineF(QPointF(0,0), direction).length() > 1)
		direction *= 0.995;
	qreal time = repaint_tracker.restart();

	QRect rectBefore = circle_bounds(current_pos, radius, font_size);

	qreal dx = direction.x();
	qreal dy = direction.y();
	if (time > 0)
	{
		dx = dx * time * .05;
		dy = dy * time * .05;
	}

	current_pos += QPointF(dx, dy);

	if (current_pos.x() - radius < 0)
	{
		direction.setX(-direction.x());
		current_pos.setX(radius);
	}
	else if (current_pos.x() + radius > width())
	{
		direction.setX(-direction.x());
		current_pos.setX(width() - radius);
	}

	if (current_pos.y() - radius < 0)
	{
		direction.setY(-direction.y());
		current_pos.setY(radius);
	}
	else if (current_pos.y() + radius > height())
	{
		direction.setY(-direction.y());
		current_pos.setY(height() - radius);
	}

	QRect rectAfter = circle_bounds(current_pos, radius, font_size);
	update(rectAfter | rectBefore);
}


void ScreenSaverDeform::paintEvent(QPaintEvent *event)
{
	if (need_refresh)
		canvas_image = bg_image;
	else
	{
		QPainter p(&canvas_image);
		p.drawImage(event->rect(), bg_image, event->rect());
	}

	QPoint topleft = current_pos.toPoint() - QPoint(radius, radius);
	QPoint bottomright = current_pos.toPoint() + QPoint(radius, radius);

	int x, y;

	for (y = topleft.y(); y < bottomright.y() - 1; ++y)
	{
		quint16 *line_colors = (quint16*)canvas_image.scanLine(y);
		for (x = topleft.x(); x < bottomright.x() - 1; ++x)
		{
			int pos_x = static_cast<int>(current_pos.x());
			int pos_y = static_cast<int>(current_pos.y());
			int lx = x - pos_x + radius;
			int ly = y - pos_y + radius;

			int pickx = lens_lookup_table[lx][ly].x() + pos_x;
			int picky = lens_lookup_table[lx][ly].y() + pos_y;

			// We pick the color from the right position (calculated by lens_lookup_table)
			// in the source QImage and put it into the dest QImage simply copying the raw
			// color data (that is stored using the 16 bit RGB color format)
			if (pickx >= 0 && pickx < bg_image.width() &&
				picky >= 0 && picky < bg_image.height())
				line_colors[x] = ((quint16*)bg_image.scanLine(picky))[pickx];
		}
	}

	QPainter painter(this);
	painter.drawImage(canvas_image.rect(), canvas_image);
	painter.drawPixmap(topleft, lens_pixmap);
}

