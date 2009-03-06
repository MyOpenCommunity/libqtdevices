#include "transitionwidget.h"
#include "page.h"
#include "main.h"
#include "sottomenu.h"

#include <QPainter>
#include <QStackedWidget>
#include <QLayout>
#include <QDebug>

#include <assert.h>


TransitionWidget::TransitionWidget(QStackedWidget *win, int time) : timeline(time, this)
{
	main_window = win;
	main_window->addWidget(this);
	connect(&timeline, SIGNAL(finished()), &local_loop, SLOT(quit()));
	connect(&timeline, SIGNAL(finished()), SLOT(transitionEnd()));
}

void TransitionWidget::setStartingImage(const QPixmap &prev)
{
	prev_page = prev;
	// this shows the transition widget (that now shows the prev page)
	main_window->setCurrentWidget(this);
}

void TransitionWidget::transitionEnd()
{
	dest_page->show();
	main_window->setCurrentWidget(dest_page);
}

void TransitionWidget::startTransition(Page *next)
{
	dest_page = next;
	initTransition();

	// Before grab the screenshot of the next page, we have to ensure that its
	// visualization is correct.
	next->resize(main_window->size());
	if (QLayout *layout = next->layout())
	{
		layout->activate();
		layout->update();
	}
	else if (sottoMenu *p = qobject_cast<sottoMenu*>(next))
		p->forceDraw();

	// this sets the next page and applies all layout computation before starting the transition
	next_page = QPixmap::grabWidget(next);

	timeline.start();
	local_loop.exec();
}

BlendingTransition::BlendingTransition(QStackedWidget *win) : TransitionWidget(win, 400)
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
	p.drawPixmap(QPoint(0,0), prev_page);
	p.setOpacity(blending_factor);
	p.drawPixmap(QPoint(0,0), next_page);
}


MosaicTransition::MosaicTransition(QStackedWidget *win) : TransitionWidget(win, 500)
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

	int num_x = MAX_WIDTH / SQUARE_DIM;
	int num_y = MAX_HEIGHT / SQUARE_DIM;

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
	dest_pix = prev_page;
}

void MosaicTransition::triggerRepaint(int index)
{
	assert(index < mosaic_map.size() && "Invalid index value!");
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
		paint.drawPixmap(mosaic_map.at(i), next_page.copy(mosaic_map.at(i)));

	QPainter p(this);
	p.drawPixmap(QPoint(0,0), dest_pix);
}

