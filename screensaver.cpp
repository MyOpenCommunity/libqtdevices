#include "screensaver.h"
#include "main.h"
#include "page.h"
#include "timescript.h"
#include "fontmanager.h"
#include "xml_functions.h"
#include "titlelabel.h"

#include <QVBoxLayout>
#include <QDomNode>
#include <QPainter>
#include <QBitmap>
#include <QLabel>
#include <QTimer>
#include <QDebug>

#include <stdlib.h> // RAND_MAX
#include <assert.h>

#define BALL_NUM 5


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
	case ScreenSaver::NONE:
		return 0;
	default:
		assert(!"Type of screensaver not handled!");
	}
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

void ScreenSaver::start(Page *p)
{
	page = p;
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

void ScreenSaverBalls::start(Page *p)
{
	ScreenSaver::start(p);
	for (int i = 0; i < BALL_NUM; ++i)
	{
		QLabel *l = new QLabel(p);
		ball_list[l] = BallData();
		initBall(l, ball_list[l]);
		l->show();
	}
}

void ScreenSaverBalls::initBall(QLabel *ball, BallData &data)
{
	data.x = (int)(200.0 * rand() / (RAND_MAX + 1.0));
	data.y = (int)(200.0 * rand() / (RAND_MAX + 1.0));
	data.vx = (int)(30.0 * rand() / (RAND_MAX + 1.0)) - 15;
	data.vy = (int)(30.0 * rand() / (RAND_MAX + 1.0)) - 15;
	if (data.vx == 0)
		data.vx = 1;
	if (data.vy == 0)
		data.vy = 1;

	data.dim = (int)(10.0 * rand() / (RAND_MAX + 1.0)) + 15;
	ball->resize(data.dim, data.dim);

	QBitmap mask = QBitmap(data.dim, data.dim);
	mask.clear();
	QPainter p(&mask);
	p.setBrush(QBrush(Qt::color1, Qt::SolidPattern));
	for (int i = 2; i <= data.dim; ++i)
		p.drawEllipse((data.dim - i) / 2, (data.dim - i) / 2, i, i);
	ball->setMask(mask);
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
			data.vx = static_cast<int>(10.0 * rand() / (RAND_MAX + 1.0)) + 5;
			data.x = 0;
			change_style = true;
		}
		if (data.y > MAX_HEIGHT - data.dim)
		{
			data.vy = static_cast<int>(10.0 * rand() / (RAND_MAX + 1.0)) - 15;
			data.y = MAX_HEIGHT - data.dim;
			change_style = true;
		}

		if (data.y <= 0)
		{
			data.vy = static_cast<int>(10.0 * rand() / (RAND_MAX + 1.0)) + 5;
			if (data.vy == 0)
				data.vy = 1;
			data.y = 0;
			change_style = true;
		}
		if (data.x > MAX_WIDTH - data.dim)
		{
			data.vx = static_cast<int>(10.0 * rand() / (RAND_MAX + 1.0)) - 15;
			if (data.vx == 0)
				data.vx = 1;
			data.x = MAX_WIDTH - data.dim;
			change_style = true;
		}

		if (change_style)
		{
			QColor ball_bg = QColor((int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
									(int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
									(int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150);

			QString ball_style = QString("QLabel {background-color:%1;}").arg(ball_bg.name());
			it.key()->setStyleSheet(ball_style);
		}
		it.key()->move(data.x, data.y);
	}
}


ScreenSaverLine::ScreenSaverLine() : ScreenSaver(150)
{
	line = 0;
	up_to_down = true;
}

void ScreenSaverLine::start(Page *p)
{
	ScreenSaver::start(p);
	line = new QLabel(p);
	line->resize(MAX_WIDTH, 6);
	line->setStyleSheet(styleUpToDown());
	line->show();
}

void ScreenSaverLine::stop()
{
	delete line;
	ScreenSaver::stop();
}

void ScreenSaverLine::refresh()
{
	if (y > MAX_HEIGHT)
	{
		y = MAX_HEIGHT;
		up_to_down = false;
		line->setStyleSheet(styleDownToUp());
	}

	if (y < 0)
	{
		y = 0;
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


void ScreenSaverTime::start(Page *p)
{
	ScreenSaverLine::start(p);
	line->resize(MAX_WIDTH, 30);

	timeScript *time = new timeScript(line, 1);
	time->setFrameStyle(QFrame::Plain);

	QFont aFont;
	FontManager::instance()->getFont(font_banner_SecondaryText, aFont);
	time->setFont(aFont);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(0, 2, 0, 2);
	layout->addWidget(time, 0, Qt::AlignCenter);
	line->setLayout(layout);
}


void ScreenSaverText::start(Page *p)
{
	ScreenSaverLine::start(p);
	line->resize(MAX_WIDTH, 30);

	QFont aFont;
	FontManager::instance()->getFont(font_banner_SecondaryText, aFont);
	line->setFont(aFont);
	line->setAlignment(Qt::AlignCenter);
	line->setText(text);
}


