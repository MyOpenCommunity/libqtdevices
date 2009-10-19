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
	int scroll_step;
	ContentWidget(QWidget *parent=0);
	void appendBanner(banner *b);
	int bannerCount();
	banner *getBanner(int i);

public slots:
	virtual void upClick();
	virtual void downClick();
	virtual void forwardClick() {} // Do nothing by default
	void resetIndex();

protected:
	void showEvent(QShowEvent *e);

private:
	// The index of the current banner
	int current_index;
	// The maximum number of banners per page
	int max_banner;

	bool need_update; // a flag to avoid useless call to updateLayout
	QList<banner*> banner_list;
	void updateLayout();
};

#endif // CONTENT_WIDGET_H
