#include "screensaver.h"
#include "page.h"
#include "timescript.h"
#include "fontmanager.h" // bt_global::font
#include "xml_functions.h"
#include "titlelabel.h"
#include "homewindow.h"

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

#include <stdlib.h> // RAND_MAX
#define BALL_NUM 5

inline void setRandomColor(QWidget *w)
{
	QColor bg = QColor((int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
						(int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
						(int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150);

	QString style = QString("QWidget {background-color:%1;}").arg(bg.name());
	w->setStyleSheet(style);
}


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

// Definition of static member
QString ScreenSaver::text;


ScreenSaver::ScreenSaver(int refresh_time)
{
	page = 0;
	timer = new QTimer(this);
	timer->setInterval(refresh_time);
	connect(timer, SIGNAL(timeout()), SLOT(refresh()));
}

void ScreenSaver::start(Window *w)
{
	window = w;
	// TODO maybe we can assume that the Window will always be an HomeWindow
	//      and page will always be != 0 and remove the checks in btmain.cpp
	if (HomeWindow *hw = qobject_cast<HomeWindow*>(w))
		page = hw->currentPage();
	timer->start();
}

void ScreenSaver::stop()
{
	page = 0;
	timer->stop();
}

bool ScreenSaver::isRunning()
{
	return page != 0;
}

void ScreenSaver::initData(const QDomNode &config_node)
{
	text = getTextChild(config_node, "text");
}


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
	QMutableHashIterator<QLabel*, BallData> it(ball_list);
	while (it.hasNext())
	{
		it.next();
		delete it.key();
	}

	ball_list.clear();
	ScreenSaver::stop();
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

void ScreenSaverLine::customizeLine()
{
	setLineHeight(6);
}

void ScreenSaverLine::setLineHeight(int height)
{
	line->resize(window->width(), height);
	line_height = height;
}

void ScreenSaverLine::stop()
{
	delete line;
	ScreenSaver::stop();
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

QString ScreenSaverLine::styleDownToUp()
{
	return "* {background-color:#000000; color:#FFFFFF; }";
}

QString ScreenSaverLine::styleUpToDown()
{
	return "* {background-color:#FFFFFF; color:#000000; }";
}


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

void ScreenSaverText::customizeLine()
{
	setLineHeight(30);
	line->setFont(bt_global::font->get(FontManager::TEXT));
	line->setAlignment(Qt::AlignCenter);
	line->setText(text);
}


ScreenSaverSlideshow::ScreenSaverSlideshow() : ScreenSaver(10000)
{
	QDir image_dir("cfg/slideshow");
	QStringList name_filter;
	name_filter << "*.[Jj][Pp][Gg]" << "*.[Pp][Nn][Gg]";

	QFileInfoList fl = image_dir.entryInfoList(name_filter);
	foreach (const QFileInfo &fi, fl)
		images << fi.absoluteFilePath();

	if (images.isEmpty())
		qWarning() << "Slideshow directory empty or no usable file";
	image_on_screen = new QLabel(this);
	image_on_screen->setGeometry(0, 0, width(), height());
	image_index = 0;
	blending_timeline.setDuration(2000);
	blending_timeline.setFrameRange(1, 30);
	connect(&blending_timeline, SIGNAL(valueChanged(qreal)), SLOT(updateImage(qreal)));
}

void ScreenSaverSlideshow::start(Window *w)
{
	ScreenSaver::start(w);
	showWindow();
	next_image = QPixmap(width(), height());
	// TODO: take background from skin
	next_image.fill(QColor(Qt::black));
	// this call will update both current_image and next_image
	refresh();
}

void ScreenSaverSlideshow::stop()
{
	// TODO: add code here
	ScreenSaver::stop();
}

void ScreenSaverSlideshow::refresh()
{
	if (!images.isEmpty())
	{
		current_image = next_image;
		image_index = (image_index + 1) % images.size();
		next_image.load(images[image_index]);
		next_image = next_image.scaled(this->size(), Qt::KeepAspectRatio);
		qDebug() << "Showing image: " << images[image_index];
		blending_timeline.start();
	}
	// else turn off screen
}

void ScreenSaverSlideshow::updateImage(qreal new_value)
{
	QPixmap pix(current_image);
	QPainter p(&pix);
	p.setRenderHint(QPainter::SmoothPixmapTransform, false);
	p.setOpacity(new_value);
	p.drawPixmap(QPoint(0,0), next_image);
	image_on_screen->setPixmap(pix);
}


static inline QRect circle_bounds(const QPointF &center, qreal radius, qreal compensation)
{
	return QRect(qRound(center.x() - radius - compensation),
				 qRound(center.y() - radius - compensation),
				 qRound((radius + compensation) * 2),
				 qRound((radius + compensation) * 2));
}


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

