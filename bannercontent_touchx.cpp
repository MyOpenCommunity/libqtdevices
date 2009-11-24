#include "bannercontent.h"
#include "banner.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QDebug>


#ifdef LAYOUT_TOUCHX

BannerContent::BannerContent(QWidget *parent) : QWidget(parent)
{
	current_page = 0;
	QGridLayout *l = new QGridLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	l->setColumnStretch(0, 1);
	l->setColumnStretch(1, 1);
	need_update = true;
}

int BannerContent::bannerCount()
{
	return banner_list.size();
}

void BannerContent::initBanners()
{
	for (int i = 0; i < banner_list.size(); ++i)
		banner_list.at(i)->inizializza();
}

banner *BannerContent::getBanner(int i)
{
	return banner_list.at(i);
}

void BannerContent::appendBanner(banner *b)
{
	banner_list.append(b);
	b->hide();

	for (int idx = banner_list.size() - 2; idx >= 0; idx--)
		if (banner_list.at(idx)->getId() == b->getId() && b->getId() != -1)
		{
			b->setSerNum(banner_list.at(idx)->getSerNum() + 1);
			idx = -1;
		}
}

void BannerContent::resetIndex()
{
	need_update = true;
}

void BannerContent::showEvent(QShowEvent *e)
{
	drawContent();
	QWidget::showEvent(e);
}

// TODO try to unify layout computation with IconContent
void BannerContent::drawContent()
{
	const int LEFT_COLUMN = 0, RIGHT_COLUMN = 1, COLUMN_COUNT = 2;

	if (!need_update)
		return;

	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	if (pages.size() == 0)
	{
		int total_height[2] = {0, 0};
		int area_height = contentsRect().height();

		// the pages array contains the starting indices of each page in banner_list
		// to simplify the last page case, an additional item is added to the array
		// and it contains banner_list.size()
		// for example for a BannerContent with 15 items and 6 items per page, pages will
		// contain: 0, 6, 12, 15
		pages.append(0);

		for (int i = 0; i < banner_list.size(); i += COLUMN_COUNT)
		{
			// compute the height the two columns would have if adding the next
			// two items
			for (int j = 0; j < COLUMN_COUNT && i + j < banner_list.size(); ++j)
				total_height[j] += banner_list.at(i + j)->sizeHint().height();
			// if the height of one of the two columns exceeds the page height,
			// start a new page, otherwise add the widgets to the layout
			if (total_height[LEFT_COLUMN] > area_height || total_height[LEFT_COLUMN] > area_height)
			{
				total_height[LEFT_COLUMN] = total_height[LEFT_COLUMN] = 0;
				pages.append(i);
				i -= COLUMN_COUNT;
			}
			else
				for (int j = 0; j < COLUMN_COUNT && i + j < banner_list.size(); ++j)
					l->addWidget(banner_list.at(i + j), i / COLUMN_COUNT, j);
		}

		pages.append(banner_list.size());
		l->setRowStretch(l->rowCount(), 1);
	}

	emit displayScrollButtons(pageCount() > 1);

	need_update = false;

	// works for all pages because the last item of the pages array always
	// contains banner_list.size()
	for (int i = 0; i < banner_list.size(); ++i)
		banner_list[i]->setVisible(i >= pages[current_page] && i < pages[current_page + 1]);
}

void BannerContent::pgUp()
{
	current_page = (current_page - 1 + pageCount()) % pageCount();
	need_update = true;
	drawContent();
}

void BannerContent::pgDown()
{
	current_page = (current_page + 1) % pageCount();
	need_update = true;
	drawContent();
}

int BannerContent::pageCount() const
{
	return pages.size() - 1;
}

#endif // LAYOUT_TOUCHX
