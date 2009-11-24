#ifndef CONTENT_WIDGET_H
#define CONTENT_WIDGET_H

#include <QWidget>
#include <QList>

#include "page.h"

class banner;
class QShowEvent;


/**
 * The BannerContent class manage a list of banner usually put inside a Page.
 */
class BannerContent : public QWidget
{
friend void BannerPage::activateLayout();
Q_OBJECT
public:
	BannerContent(QWidget *parent=0);
	void appendBanner(banner *b);
	int bannerCount();
	banner *getBanner(int i);
	void initBanners();

public slots:
	void pgUp();
	void pgDown();
	void resetIndex();

signals:
	void displayScrollButtons(bool display);
	void currentPageChanged(int current, int total);

protected:
	void showEvent(QShowEvent *e);

private:
	// The drawContent is the place where this widget is actually drawed. In order
	// to have a correct transition effect, this method is also called by the
	// Page _before_ that the Page is showed.
	void drawContent();

	int pageCount() const;
	int calculateNextIndex(bool up_to_down);

private:
	bool need_update; // a flag to avoid useless call to drawContent

	QList<banner*> banner_list;

#ifdef LAYOUT_BTOUCH
	// The index of the banner showed at the top of the BannerContent
	int current_index;

	bool first_time; // true if the page is shown for the first time

	// true if the banners height exceed the BannerContent height (so we have to do a pagination)
	bool need_pagination;
#endif
#ifdef LAYOUT_TOUCHX
	int current_page;
	QList<int> pages;
#endif
};

#endif // CONTENT_WIDGET_H
