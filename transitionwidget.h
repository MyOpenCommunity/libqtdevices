#ifndef TRANSITIONWIDGET_H
#define TRANSITIONWIDGET_H

#include <QEventLoop>
#include <QWidget>
#include <QTimeLine>

class Page;
class QStackedWidget;

class TransitionWidget : private QWidget
{
Q_OBJECT
public:
	TransitionWidget(QStackedWidget *win);
	void startTransition(Page *prev, Page *next);

protected:
	void paintEvent(QPaintEvent *e);

private slots:
	void triggerRepaint(qreal);

private:
	QTimeLine timeline;
	QStackedWidget *main_window;
	QPixmap prev_page;
	QPixmap next_page;
	qreal blending_factor;
	QEventLoop local_loop;
};

#endif // TRANSITIONWIDGET_H
