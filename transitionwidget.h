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
class TransitionWidget : public QWidget
{
friend class Page;
Q_OBJECT
public:
	TransitionWidget(QStackedWidget *win, int time);
	void setStartingPage(Page *prev);
	void startTransition(Page *next);

protected:
	QStackedWidget *main_window;
	QTimeLine timeline;
	QPixmap prev_image;
	QPixmap next_image;

	virtual void initTransition() {}

private slots:
	void transitionEnd();

private:
	Page *prev_page;
	Page *dest_page;
	QEventLoop local_loop;

	void cancelTransition();
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
	int prev_index;
	QPixmap dest_pix;
	QList<QRect> mosaic_map;
};

#endif // TRANSITIONWIDGET_H
