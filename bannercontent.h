#ifndef CONTENT_WIDGET_H
#define CONTENT_WIDGET_H

#include <QWidget>
#include <QList>

#include "page.h"

class banner;
class QShowEvent;

class BannerContentBase : public QWidget
{
Q_OBJECT
public:
	BannerContentBase(QWidget *parent=0);
	void appendBanner(banner *b);
	int bannerCount();
	banner *getBanner(int i);
	void initBanners();

public slots:
	virtual void pgUp() = 0;
	virtual void pgDown() = 0;
	virtual void resetIndex() = 0;

protected:
	// The drawContent is the place where this widget is actually drawed. In order
	// to have a correct transition effect, this method is also called by the
	// Page _before_ that the Page is showed.
	virtual void drawContent() = 0;
	QList<banner*> banner_list;

signals:
	void displayScrollButtons(bool display);
	void contentScrolled(int current, int total);
};

/**
 * The BannerContent class manage a list of banner usually put inside a Page.
 */
class BannerContent : public BannerContentBase
{
friend void BannerPage::activateLayout();
Q_OBJECT
public:
	BannerContent();

public slots:
	virtual void pgUp();
	virtual void pgDown();
	virtual void resetIndex();

protected:
	void showEvent(QShowEvent *e);
	virtual void drawContent();

private:
	int pageCount() const;
	int calculateNextIndex(bool up_to_down);

	bool need_update; // a flag to avoid useless call to drawContent

	// The index of the banner showed at the top of the BannerContent
	int current_index;

	bool first_time; // true if the page is shown for the first time

	// true if the banners height exceed the BannerContent height (so we have to do a pagination)
	bool need_pagination;
};

#endif // CONTENT_WIDGET_H
