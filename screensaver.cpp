#include "screensaver.h"
#include "main.h"
#include "page.h"

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
		return new ScreenSaverBalls();
	case ScreenSaver::LINES:
		return new ScreenSaverLine();
	case ScreenSaver::NONE:
		return 0;
	default:
		assert(!"Type of screensaver not handled!");
	}
}


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


ScreenSaverBalls::ScreenSaverBalls() : ScreenSaver(120)
{
	backcol = 0;
}

void ScreenSaverBalls::start(Page *p)
{
	ScreenSaver::start(p);
	for (int i = 0; i < BALL_NUM; ++i)
	{
		QLabel *l = new QLabel(p);
		ball_list[l] = BallData();
		l->show();
	}
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
	if (backcol < 3)
	{
		backcol = 4;
		QMutableHashIterator<QLabel*, BallData> it(ball_list);
		while (it.hasNext())
		{
			it.next();
			BallData& data = it.value();
			data.x = (int)(200.0 * rand() / (RAND_MAX + 1.0));
			data.y = (int)(200.0 * rand() / (RAND_MAX + 1.0));
			data.vx = (int)(30.0 * rand() / (RAND_MAX + 1.0)) - 15;
			data.vy = (int)(30.0 * rand() / (RAND_MAX + 1.0)) - 15;
			if (data.vx == 0)
				data.vx = 1;
			if (data.vy == 0)
				data.vy = 1;

			data.dim = (int)(10.0 * rand() / (RAND_MAX + 1.0)) + 15;
			it.key()->resize(data.dim, data.dim);

			QBitmap mask = QBitmap(data.dim, data.dim);
			mask.clear();
			QPainter p(&mask);
			p.setBrush(QBrush(Qt::color1, Qt::SolidPattern));
			for (int i = 2; i <= data.dim; ++i)
				p.drawEllipse((data.dim - i) / 2, (data.dim - i) / 2, i, i);
			it.key()->setMask(mask);
		}
	}
	else
	{
		++backcol;
		if (backcol == 9)
			backcol = 4;

		QMutableHashIterator<QLabel*, BallData> it(ball_list);
		while (it.hasNext())
		{
			it.next();
			BallData& data = it.value();
			data.x += data.vx;
			data.y += data.vy;

			QColor ball_bg = QColor((int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
									(int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
									(int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150);
			int rand_number = (int)(10.0 * rand() / (RAND_MAX + 1.0));

			QLabel *ball = it.key();
			QString ball_style = QString("QLabel {background-color:%1;}").arg(ball_bg.name());
			if (data.x <= 0)
			{
				data.vx = rand_number + 5;
				data.x = 0;
				ball->setStyleSheet(ball_style);
			}
			if (data.y > MAX_HEIGHT - data.dim)
			{
				data.vy = rand_number - 15;
				data.y = MAX_HEIGHT - data.dim;
				ball->setStyleSheet(ball_style);
			}

			if (data.y <= 0)
			{
				data.vy = rand_number + 5;
				if (data.vy == 0)
					data.vy = 1;
				data.y = 0;
				ball->setStyleSheet(ball_style);
			}
			if (data.x > MAX_WIDTH - data.dim)
			{
				data.vx = rand_number - 15;
				if (data.vx == 0)
					data.vx = 1;
				data.x = MAX_WIDTH - data.dim;
				ball->setStyleSheet(ball_style);
			}
			ball->move(data.x, data.y);
		}
	}
}


ScreenSaverLine::ScreenSaverLine() : ScreenSaver(150)
{
	line = 0;
	backcol = 10;
	up_to_down = true;
}

void ScreenSaverLine::start(Page *p)
{
	ScreenSaver::start(p);
	line = new QLabel(p);
	line->resize(MAX_WIDTH, 6);
	line->setStyleSheet(QString("QLabel {background-color:#FFFFFF;}"));
	line->show();
}

void ScreenSaverLine::stop()
{
	delete line;
	ScreenSaver::stop();
}

void ScreenSaverLine::refresh()
{
	if (backcol >= 5)
		backcol = 0;

	++backcol;

	if (y > MAX_HEIGHT)
	{
		y = MAX_HEIGHT;
		up_to_down = false;
		line->setStyleSheet(QString("QLabel {background-color:#000000;}"));
	}

	if (y < 0)
	{
		y = 0;
		up_to_down = true;
		line->setStyleSheet(QString("QLabel {background-color:#FFFFFF;}"));
	}

	if (up_to_down)
		y += 3;
	else
		y -= 3;

	line->move(0, y);
}

