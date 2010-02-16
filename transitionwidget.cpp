#include "transitionwidget.h"
#include "windowcontainer.h"

#include <QPainter>
#include <QDebug>


TransitionWidget::TransitionWidget(int time) : timeline(time, this)
{
	connect(&timeline, SIGNAL(finished()), &local_loop, SLOT(quit()));
	connect(&timeline, SIGNAL(finished()), SIGNAL(endTransition()));
}

void TransitionWidget::cancelTransition()
{
	local_loop.quit();
	timeline.stop();
}

void TransitionWidget::prepareTransition()
{
	prev_image = container->grabHomeWindow();
	container->setCurrentWidget(this);
}

void TransitionWidget::startTransition()
{
	dest_image = container->grabHomeWindow();
	initTransition();

	timeline.start();
	local_loop.exec();
	prev_image = dest_image = QPixmap();
}

void TransitionWidget::setContainer(WindowContainer *c)
{
	container = c;
}


BlendingTransition::BlendingTransition() : TransitionWidget(400)
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


MosaicTransition::MosaicTransition() : TransitionWidget(500)
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

