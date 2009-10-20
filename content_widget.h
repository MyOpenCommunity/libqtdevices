#ifndef CONTENT_WIDGET_H
#define CONTENT_WIDGET_H

#include <QWidget>
#include <QList>

#include "page.h"

class banner;
class QShowEvent;


/**
 * The ContentWidget class manage a list of banner usually put inside a Page.
 */
class ContentWidget : public QWidget
{
friend void Page::activateLayout();
Q_OBJECT
public:
	ContentWidget(QWidget *parent=0);
	void appendBanner(banner *b);
	int bannerCount();
	banner *getBanner(int i);

public slots:
	void pgUp();
	void pgDown();
	void resetIndex();

protected:
	void showEvent(QShowEvent *e);

private:
	// The index of the banner showed at the top of the ContentWidget
	int current_index;

	bool need_update; // a flag to avoid useless call to updateLayout
	QList<banner*> banner_list;
	void updateLayout();
	int calculateNextIndex(bool up_to_down);
};

#endif // CONTENT_WIDGET_H
