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


#include "transitionwidget.h"

#include <QPainter>
#include <QDebug>

#define BLENDING_DURATION 200
#define MOSAIC_DURATION 200
#define ENLARGE_DURATION 300


TransitionWidget::TransitionWidget(int time) : timeline(time, this)
{
	connect(&timeline, SIGNAL(finished()), SLOT(transitionFinished()));
}

void TransitionWidget::transitionFinished()
{
	local_loop.quit();
	emit endTransition();
}

void TransitionWidget::cancelTransition()
{
	local_loop.quit();
	timeline.stop();
}

void TransitionWidget::prepareTransition(const QPixmap &image)
{
	prev_image = image;
}

void TransitionWidget::startTransition(const QPixmap &image)
{
	dest_image = image;
	initTransition();

	timeline.start();
	local_loop.exec();
	prev_image = dest_image = QPixmap();
}


BlendingTransition::BlendingTransition() : TransitionWidget(BLENDING_DURATION)
{
	connect(&timeline, SIGNAL(valueChanged(qreal)), SLOT(triggerRepaint(qreal)));
	blending_factor = 0.0;
}

void BlendingTransition::initTransition()
{
	blending_factor = 0.0;
}

void BlendingTransition::triggerRepaint(qreal value)
{
	blending_factor = value;
	update();
}

void BlendingTransition::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform, true);
	p.drawPixmap(QPoint(0,0), prev_image);
	p.setOpacity(blending_factor);
	p.drawPixmap(QPoint(0,0), dest_image);
}


MosaicTransition::MosaicTransition() : TransitionWidget(MOSAIC_DURATION)
{
	// be careful: changing the parameters of the timeline has severe impacts on performance and
	// smoothness of transition
	timeline.setCurveShape(QTimeLine::LinearCurve);

	connect(&timeline, SIGNAL(frameChanged(int)), SLOT(triggerRepaint(int)));
	curr_index = 0;
	prev_index = 0;
}

void MosaicTransition::initTransition()
{
	curr_index = 0;
	prev_index = 0;
	mosaic_map.clear();
	const int SQUARE_DIM = 10;

	int num_x = width() / SQUARE_DIM;
	int num_y = height() / SQUARE_DIM;

	QList<QRect> ordered_list;
	for (int i = 0; i < num_x; ++i)
		for (int j = 0; j < num_y; ++j)
			ordered_list.append(QRect(i * SQUARE_DIM, j * SQUARE_DIM, SQUARE_DIM, SQUARE_DIM));

	while (ordered_list.size() > 0)
	{
		int index = static_cast<int>(static_cast<float>(ordered_list.size()) * (rand() / (RAND_MAX + 1.0)));
		mosaic_map.append(ordered_list.takeAt(index));
	}

	timeline.setFrameRange(0, mosaic_map.size() - 1);
	timeline.setStartFrame(0);
	dest_pix = prev_image;
}

void MosaicTransition::triggerRepaint(int index)
{
	Q_ASSERT_X(index < mosaic_map.size(), "MosaicTransition::triggerRepaint",
		"Invalid index value!");
	prev_index = curr_index;
	curr_index = index;
	update();
}

void MosaicTransition::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	if (mosaic_map.size() == 0)
		return;

	QPainter paint(&dest_pix);
	for (int i = prev_index; i < curr_index; ++i)
		paint.drawPixmap(mosaic_map.at(i), dest_image.copy(mosaic_map.at(i)));

	QPainter p(this);
	p.drawPixmap(QPoint(0,0), dest_pix);
}


EnlargeTransition::EnlargeTransition() : TransitionWidget(ENLARGE_DURATION)
{
	percentage = 1;
	timeline.setCurveShape(QTimeLine::LinearCurve);
	connect(&timeline, SIGNAL(frameChanged(int)), SLOT(triggerRepaint(int)));
}

QString EnlargeTransition::borderColor()
{
	return border_color;
}

void EnlargeTransition::setBorderColor(QString color)
{
	border_color = color;
}

void EnlargeTransition::initTransition()
{
	timeline.setFrameRange(1, 100);
	timeline.setStartFrame(1);
}

void EnlargeTransition::triggerRepaint(int value)
{
	percentage = value;
	update();
}

void EnlargeTransition::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform, true);
	p.drawPixmap(QPoint(0,0), prev_image);

	int rect_width = width() / 100.0 * percentage;
	int rect_height = height() / 100.0 * percentage;
	// We put the rectangle of the new image aligned on the center of the widget
	QRect target_rect(width() / 2 - rect_width / 2, height() / 2 - rect_height / 2, rect_width, rect_height);
	QRect line_rect(target_rect.x() -1, target_rect.y() -1, target_rect.width() + 2, target_rect.height() + 2);
	p.drawPixmap(target_rect, dest_image);
	p.setPen(QColor(border_color));
	p.drawRect(line_rect);
}
