#include "transitionwidget.h"
#include "page.h"

#include <QPainter>
#include <QStackedWidget>

#define TRANSITION_TIME 400 // ms

TransitionWidget::TransitionWidget(QStackedWidget *win) :
	timeline(TRANSITION_TIME, this)
{
	main_window = win;
	connect(&timeline, SIGNAL(valueChanged(qreal)), SLOT(triggerRepaint(qreal)));
	connect(&timeline, SIGNAL(finished()), &local_loop, SLOT(quit()));

	blending_factor = 0.0;
}

void TransitionWidget::startTransition(Page *prev, Page *next)
{
	blending_factor = 0.0;
	prev_page = QPixmap::grabWidget(prev);
	// this shows the transition widget (that now shows the prev page)
	showFullScreen();
	update();
	local_loop.processEvents();

	// this sets the next page and applies all layout computation before starting the transition
	main_window->setCurrentWidget(next);
	local_loop.processEvents();
	next_page = QPixmap::grabWidget(next);

	timeline.start();
	local_loop.exec();
	hide();
}

void TransitionWidget::triggerRepaint(qreal value)
{
	blending_factor = value;
	update();
}

void TransitionWidget::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform, true);
	p.drawPixmap(QPoint(0,0), prev_page);
	p.setOpacity(blending_factor);
	p.drawPixmap(QPoint(0,0), next_page);
}
