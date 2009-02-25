#ifndef TRANSITIONWIDGET_H
#define TRANSITIONWIDGET_H

#include <QEventLoop>
#include <QWidget>
#include <QTimeLine>
#include <QList>
#include <QRect>

class Page;
class QStackedWidget;


/**
 * This is the base (abstract) class for all transition widget.
 */
class TransitionWidget : protected QWidget
{
Q_OBJECT
public:
	TransitionWidget(QStackedWidget *win, int time);
	void setStartingImage(const QPixmap &prev);
	void startTransition(const QPixmap &next);

protected:
	QStackedWidget *main_window;
	QTimeLine timeline;
	QEventLoop local_loop;
	QPixmap prev_page;
	QPixmap next_page;

	virtual void initTransition() {}
};


/**
 * Do a blend transition between two widgets
 */
class BlendingTransition : public TransitionWidget
{
Q_OBJECT
public:
	BlendingTransition(QStackedWidget *win);

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void initTransition();

private slots:
	void triggerRepaint(qreal);

private:
	qreal blending_factor;

};

/**
 * Do a mosaic transition between two widgets
 */
class MosaicTransition : public TransitionWidget
{
Q_OBJECT
public:
	MosaicTransition(QStackedWidget *win);

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void initTransition();

private slots:
	void triggerRepaint(int index);

private:
	int curr_index;
	QList<QRect> mosaic_map;
};

#endif // TRANSITIONWIDGET_H
