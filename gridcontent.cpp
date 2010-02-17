#include "gridcontent.h"

#include <QGridLayout>


GridContent::GridContent(QWidget *parent) : QWidget(parent)
{
	QGridLayout *l = new QGridLayout(this);

	current_page = 0;
	need_update = true;
}

void GridContent::resetIndex()
{
	need_update = true;
	current_page = 0;
}

void GridContent::showEvent(QShowEvent *e)
{
	if (need_update)
		drawContent();
	need_update = false;
	QWidget::showEvent(e);
}

void GridContent::prepareLayout(QList<QWidget *> items, int columns)
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
			total_height[j] += items.at(i + j)->sizeHint().height() + l->verticalSpacing();

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

void GridContent::updateLayout(QList<QWidget *> items)
{
	emit displayScrollButtons(pageCount() > 1);
	emit contentScrolled(current_page, pageCount());

	need_update = false;

	// works for all pages because the last item of the pages array always
	// contains banner_list.size()
	for (int i = 0; i < items.size(); ++i)
		items[i]->setVisible(i >= pages[current_page] && i < pages[current_page + 1]);
}

void GridContent::pgUp()
{
	current_page = (current_page - 1 + pageCount()) % pageCount();
	drawContent();
	need_update = false;
}

void GridContent::pgDown()
{
	current_page = (current_page + 1) % pageCount();
	drawContent();
	need_update = false;
}

int GridContent::pageCount() const
{
	return pages.size() - 1;
}
