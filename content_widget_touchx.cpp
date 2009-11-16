#include "content_widget.h"
#include "banner.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QDebug>


#ifdef LAYOUT_TOUCHX

ContentWidget::ContentWidget(QWidget *parent) : QWidget(parent)
{
	current_page = 0;
	QGridLayout *l = new QGridLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	need_update = true;
}

int ContentWidget::bannerCount()
{
	return banner_list.size();
}

void ContentWidget::initBanners()
{
	for (int i = 0; i < banner_list.size(); ++i)
		banner_list.at(i)->inizializza();
}

banner *ContentWidget::getBanner(int i)
{
	return banner_list.at(i);
}

void ContentWidget::appendBanner(banner *b)
{
	Q_ASSERT_X(layout() != 0, "ContentWidget::appendBanner", "Unable to call appendBanner without a layout");
	banner_list.append(b);
	b->hide();

	for (int idx = banner_list.size() - 2; idx >= 0; idx--)
		if (banner_list.at(idx)->getId() == b->getId() && b->getId() != -1)
		{
			b->setSerNum(banner_list.at(idx)->getSerNum() + 1);
			idx = -1;
		}
}

void ContentWidget::resetIndex()
{
	need_update = true;
}

void ContentWidget::showEvent(QShowEvent *e)
{
	drawContent();
	QWidget::showEvent(e);
}

void ContentWidget::drawContent()
{
	if (!need_update)
		return;

	if (pages.size() == 0)
	{
		int total_height[2] = {0, 0};
		int area_height = contentsRect().height();

		pages.append(0);

		for (int i = 0; i < banner_list.size(); i += 2)
		{
			total_height[0] += banner_list.at(i)->sizeHint().height();
			if (i + 1 < banner_list.size())
				total_height[1] += banner_list.at(i + 1)->sizeHint().height();
			if (total_height[0] > area_height || total_height[1] > area_height)
			{
				total_height[0] = total_height[1] = 0;
				pages.append(i);
				i -= 2;
			}
		}

		pages.append(banner_list.size());
	}

	emit displayScrollButtons(pageCount() > 1);

	need_update = false;

	// We want a circular list of banner, so we can't use a layout and hide/show
	// the banner to display or when you click the up button the order is wrong.
	// So we remove all the child from the layout, hide them and re-add to the
	// layout only the banner to show.
	QLayoutItem *child;
	while ((child = layout()->takeAt(0)) != 0)
		if (QWidget *w = child->widget())
			w->hide();

	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	for (int i = 0; i < pages[current_page + 1] - pages[current_page]; ++i)
	{
		banner *b = banner_list.at(i + pages[current_page]);
		b->show();
		l->addWidget(b, i / 2, i % 2);
	}
	l->setRowStretch(l->rowCount(), 1);
}

void ContentWidget::pgUp()
{
	current_page = (current_page - 1 + pageCount()) % pageCount();
	need_update = true;
	drawContent();
}

void ContentWidget::pgDown()
{
	current_page = (current_page + 1) % pageCount();
	need_update = true;
	drawContent();
}

int ContentWidget::pageCount() const
{
	return pages.size() - 1;
}

#endif // LAYOUT_TOUCHX
