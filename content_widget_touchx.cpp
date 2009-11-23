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
	l->setColumnStretch(0, 1);
	l->setColumnStretch(1, 1);
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

	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	if (pages.size() == 0)
	{
		int total_height[2] = {0, 0};
		int area_height = contentsRect().height();

		pages.append(0);

		for (int i = 0; i < banner_list.size(); i += 2)
		{
			for (int j = 0; j < 2 && i + j < banner_list.size(); ++j)
			{
				total_height[j] += banner_list.at(i + j)->sizeHint().height();
				l->addWidget(banner_list.at(i + j), i / 2, j);
			}
			if (total_height[0] > area_height || total_height[1] > area_height)
			{
				total_height[0] = total_height[1] = 0;
				pages.append(i);
				i -= 2;
			}
		}

		pages.append(banner_list.size());
		l->setRowStretch(l->rowCount(), 1);
	}

	emit displayScrollButtons(pageCount() > 1);

	need_update = false;

	for (int i = 0; i < banner_list.size(); ++i)
		banner_list[i]->setVisible(i >= pages[current_page] && i < pages[current_page + 1]);
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
