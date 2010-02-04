#ifndef BANNERCONTENT_H
#define BANNERCONTENT_H

#include <QWidget>
#include <QList>

#include "page.h"
#include "gridcontent.h"

class banner;


/**
 * The BannerContent class manage a list of banner usually put inside a Page.
 */
class BannerContent : public GridContent
{
friend void BannerPage::activateLayout();
Q_OBJECT
public:
#ifdef LAYOUT_BTOUCH
	BannerContent(QWidget *parent=0);
#else
	BannerContent(QWidget *parent=0, int columns=2);
#endif

	void appendBanner(banner *b);
	int bannerCount();
	banner *getBanner(int i);
	void initBanners();

protected:
	void drawContent();

private:
	QList<banner*> banner_list;
	int columns;
};

#endif // BANNERCONTENT_H
