#ifndef TRANSITIONWIDGET_H
#define TRANSITIONWIDGET_H

#include <QEventLoop>
#include <QWidget>
#include <QTimeLine>
#include <QList>
#include <QRect>

class Page;
class QStackedWidget;

class TransitionWidget : protected QWidget
{
Q_OBJECT
public:
	TransitionWidget(QStackedWidget *win, int time);
	virtual void startTransition(Page *prev, Page *next) = 0;

protected:
	QStackedWidget *main_window;
	QTimeLine timeline;
	QEventLoop local_loop;
};


class BlendingTransition : public TransitionWidget
{
Q_OBJECT
public:
	BlendingTransition(QStackedWidget *win);
	virtual void startTransition(Page *prev, Page *next);

protected:
	void paintEvent(QPaintEvent *e);

private slots:
	void triggerRepaint(qreal);

private:
	QPixmap prev_page;
	QPixmap next_page;
	qreal blending_factor;

};


class MosaicTransition : public TransitionWidget
{
Q_OBJECT
public:
	MosaicTransition(QStackedWidget *win);
	virtual void startTransition(Page *prev, Page *next);

protected:
	void paintEvent(QPaintEvent *e);

private slots:
	void triggerRepaint(int index);

private:
	int curr_index;
	QList<QRect> mosaic_map;
	QPixmap prev_page;
	QPixmap next_page;
};

#endif // TRANSITIONWIDGET_H
