#include "bannercontent.h"
#include "banner.h"

#include <QBoxLayout>
#include <QDebug>


BannerContentBase::BannerContentBase(QWidget *parent) : QWidget(parent)
{
}

int BannerContentBase::bannerCount()
{
	return banner_list.size();
}

void BannerContentBase::initBanners()
{
	for (int i = 0; i < banner_list.size(); ++i)
		banner_list.at(i)->inizializza();
}

banner *BannerContentBase::getBanner(int i)
{
	return banner_list.at(i);
}

void BannerContentBase::appendBanner(banner *b)
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



BannerContent::BannerContent()
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	resetIndex();
}

void BannerContent::pgUp()
{
	current_page = (current_page - 1 + pageCount()) % pageCount();
	drawContent();
	need_update = false;
}

void BannerContent::pgDown()
{
	current_page = (current_page + 1) % pageCount();
	drawContent();
	need_update = false;
}

void BannerContent::resetIndex()
{
	current_page = 0;
	need_update = true;
}

int BannerContent::pageCount() const
{
	return pages.size() - 1;
}

void BannerContent::drawContent()
{
	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	// copy the list to pass it to GridContent methods; as an alternative
	// we could change them to template methods
	QList<QWidget *> items;
	for (int i = 0; i < banner_list.size(); ++i)
		items.append(banner_list[i]);

	if (pages.size() == 0)
	{
		// prepare the page list
		prepareLayout(items, 1);

		// add items to the layout
		for (int i = 0; i < pages.size() - 1; ++i)
		{
			int base = pages[i];
			for (int j = 0; base + j < pages[i + 1]; ++j)
				l->addWidget(banner_list.at(base + j), j, 1);
		}

		l->setRowStretch(l->rowCount(), 1);
	}

	updateLayout(items);
}

void BannerContent::updateLayout(QList<QWidget *> items)
{
	emit displayScrollButtons(pageCount() > 1);
	emit contentScrolled(current_page, pageCount());

	need_update = false;

	// works for all pages because the last item of the pages array always
	// contains banner_list.size()
	for (int i = 0; i < items.size(); ++i)
		items[i]->setVisible(i >= pages[current_page] && i < pages[current_page + 1]);
}

// This is exactly the same as GridContent found in master; I hope we can easily merge this code
// with master once we decide that all banner pages on btouch must scroll by page
void BannerContent::prepareLayout(QList<QWidget *> items, int columns)
{
	QGridLayout *l = static_cast<QGridLayout *>(layout());
	QList<int> total_height;
	int area_height = contentsRect().height();

	for (int k = 0; k < columns; ++k)
		total_height.append(0);

	// the pages array contains the starting indices of each page in banner_list
	// to simplify the last page case, an additional item is added to the array
	// and it contains banner_list.size()
	// for example for a BannerContent with 15 items and 6 items per page, pages will
	// contain: 0, 6, 12, 15
	pages.append(0);

	for (int i = 0; i < items.size(); i += columns)
	{
		// compute the height the columns would have if adding the next items
		for (int j = 0; j < columns && i + j < items.size(); ++j)
		{
			total_height[j] += items.at(i + j)->sizeHint().height() + l->spacing();
		}

		// if the height of one of the two columns exceeds the page height,
		// start a new page, otherwise add the widgets to the layout
		for (int j = 0; j < columns; ++j)
		{
			if (total_height[j] > area_height)
			{
				for (int k = 0; k < columns; ++k)
					total_height[k] = 0;
				pages.append(i);
				i -= columns;
				break;
			}
		}
	}

	pages.append(items.size());
}

