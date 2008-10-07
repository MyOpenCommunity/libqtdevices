#include "screensaver.h"
#include "main.h"

#include <qpainter.h>
#include <qbitmap.h>

#include <stdlib.h> // RAND_MAX
#include <assert.h>

ScreenSaver *getScreenSaver(ScreenSaver::Type type)
{
	switch (type)
	{
	case ScreenSaver::BALLS:
		return new ScreenSaverBalls();
	case ScreenSaver::LINES:
		return new ScreenSaverLine();
	default:
		assert(!"Type of screensaver not handled!");
	}
}


ScreenSaverBalls::ScreenSaverBalls()
{
	backcol = 0;
	y[0] = 0;
	for (int idx = 0; idx < BALL_NUM; idx++)
	{
		ball[idx] = new BtLabel(this);
		// TODO: sistemare con i metodi qt4!
		//ball[idx]->setBackgroundMode(Qt::NoBackground);
	}
}

void ScreenSaverBalls::refresh(const QPixmap &bg_image)
{
	if (backcol < 3)
	{
		backcol = 4;
		// TODO: sistemare con i metodi qt4!
		// setPaletteBackgroundPixmap(bg_image);
		for (int idx = 0; idx < BALL_NUM; idx++)
		{
			x[idx] = (int)(200.0 * rand() / (RAND_MAX + 1.0));
			y[idx] = (int)(200.0 * rand() / (RAND_MAX + 1.0));
			vx[idx] = (int)(30.0 * rand() / (RAND_MAX + 1.0)) - 15;
			vy[idx] = (int)(30.0 * rand() / (RAND_MAX + 1.0)) - 15;
			if (!vy[idx])
				vy[idx] = 1;
			if (!vx[idx])
				vx[idx] = 1;
			dim[idx] = (int)(10.0 * rand() / (RAND_MAX + 1.0)) + 15;

			QBitmap mask = QBitmap(dim[idx], dim[idx]);
			mask.clear();
			QPainter p(&mask);
			p.setBrush(QBrush(Qt::color1, Qt::SolidPattern));
			for (int idy = 2; idy <= dim[idx]; idy++)
				p.drawEllipse((dim[idx]-idy)/2, (dim[idx]-idy)/2, idy, idy);
			ball[idx]->setMask(mask);
		}
	}
	else
	{
		backcol++;
		if (backcol == 9)
		{
			backcol = 4;
			// TODO: sistemare con i metodi qt4!
			// setPaletteBackgroundPixmap(bg_image);
		}

		for (int idx = 0; idx < BALL_NUM; idx++)
		{
			x[idx] += vx[idx];
			y[idx] += vy[idx];

			QColor ball_bg = QColor((int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
									(int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150,
									(int) (100.0 * rand() / (RAND_MAX + 1.0)) + 150);
			int rand_number = (int)(10.0 * rand() / (RAND_MAX + 1.0));
			
			if  (x[idx] <= 0)
			{
				vx[idx] = rand_number + 5;
				x[idx] = 0;
				ball[idx]->setPaletteBackgroundColor(ball_bg);
			}
			if  (y[idx] > (MAX_HEIGHT-dim[idx]))
			{
				vy[idx] = rand_number - 15;
				y[idx] = MAX_HEIGHT-dim[idx];
				ball[idx]->setPaletteBackgroundColor(ball_bg);
			}
			if   (y[idx] <= 0)
			{
				vy[idx] = rand_number + 5;
				if (!vy[idx])
					vy[idx] = 1;
				y[idx] = 0;
				ball[idx]->setPaletteBackgroundColor(ball_bg);
			}
			if  (x[idx] > (MAX_WIDTH-dim[idx]))
			{
				vx[idx] = rand_number - 15;
				if (!vx[idx])
					vx[idx] = 1;
				x[idx] = MAX_WIDTH - dim[idx];
				ball[idx]->setPaletteBackgroundColor(ball_bg);
			}
			ball[idx]->setGeometry(x[idx], y[idx], dim[idx], dim[idx]);
			ball[idx]->show();
		}
	}
}


ScreenSaverLine::ScreenSaverLine()
{
	backcol = 10;
	line = new BtLabel(this);
	black_line = false;
}

void ScreenSaverLine::refresh(const QPixmap &bg_image)
{
	if (backcol >= 5)
	{
		// TODO: sistemare con i metodi qt4!
		// setPaletteBackgroundPixmap(bg_image);
		backcol = 0;
	}

	++backcol;

	if (y > MAX_HEIGHT)
	{
		y = MAX_HEIGHT;
		black_line = true;
	}

	if (y < 0)
	{
		y = 0;
		black_line = false;
	}

	if (!black_line)
		y += 3;
	else
		y -= 3;

	line->setGeometry(0, y, MAX_WIDTH, 6);
	line->setPaletteBackgroundColor(QColor::QColor(black_line ? Qt::black : Qt::white));
	line->show();
}
