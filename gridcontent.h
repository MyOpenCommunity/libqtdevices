#ifndef GRIDCONTENT_H
#define GRIDCONTENT_H

#include <QWidget>


/**
 * The GridContent class manages a grid of subwidgets.
 */
class GridContent : public QWidget
{
Q_OBJECT
public:
	GridContent(QWidget *parent=0);

public slots:
	void pgUp();
	void pgDown();
	void resetIndex();

signals:
	void displayScrollButtons(bool display);
	void contentScrolled(int current, int total);

protected:
	void showEvent(QShowEvent *e);

	void prepareLayout(QList<QWidget *> items, int columns);
	void updateLayout(QList<QWidget *> items);

	// The drawContent is the place where this widget is actually drawed. In order
	// to have a correct transition effect, this method is also called by the
	// Page _before_ that the Page is showed.
	virtual void drawContent() = 0;

	int pageCount() const;

protected:
	bool need_update; // a flag to avoid useless call to drawContent
	int current_page;
	QList<int> pages;
};

#endif // GRIDCONTENT_H

