#ifndef TRANSITIONWIDGET_H
#define TRANSITIONWIDGET_H

#include <QEventLoop>
#include <QWidget>
#include <QTimeLine>
#include <QList>
#include <QRect>

class Page;
class MainWindow;


/**
 * This is the base (abstract) class for all transition widget.
 */
class TransitionWidget : public QWidget
{
Q_OBJECT
public:
	TransitionWidget(int time);
	void startTransition(const QPixmap &prev, const QPixmap &dest);
	void cancelTransition();

signals:
	void endTransition();

protected:
	QTimeLine timeline;
	QPixmap prev_image;
	QPixmap dest_image;

	virtual void initTransition() {}

private:
	MainWindow *main_window;
	QEventLoop local_loop;
};


/**
 * Do a blend transition between two widgets
 */
class BlendingTransition : public TransitionWidget
{
Q_OBJECT
public:
	BlendingTransition();

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
	MosaicTransition();

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
