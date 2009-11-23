#include "content_widget.h"
#include "banner.h"

#include <QBoxLayout>
#include <QDebug>


// TODO: add an abstract base class for the BTouch and TouchX content
// widgets and remove this #ifdef

#ifdef LAYOUT_BTOUCH

ContentWidget::ContentWidget(QWidget *parent) : QWidget(parent)
{
	current_index = 0;
	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	need_update = true;
	first_time = true;
	need_pagination = false;
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
	current_index = 0;
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

	if (first_time)
	{
		int total_height = 0;
		int area_height = contentsRect().height();

		for (int i = 0; i < banner_list.size(); ++i)
			total_height += banner_list.at(i)->sizeHint().height();

		need_pagination = total_height > area_height;
		emit displayScrollButtons(need_pagination);
	}

	need_update = false;

	// We want a circular list of banner, so we can't use a layout and hide/show
	// the banner to display or when you click the up button the order is wrong.
	// So we remove all the child from the layout, hide them and re-add to the
	// layout only the banner to show.
	QLayoutItem *child;
	while ((child = layout()->takeAt(0)) != 0)
		if (QWidget *w = child->widget())
			w->hide();

	if (need_pagination)
	{
		int next_index = calculateNextIndex(true);
		Q_ASSERT_X(current_index != -1, "ContentWidget::drawContent", "calculateNextIndex return -1!");
		int index = current_index;
		while (true)
		{
			banner *b = banner_list.at(index);
			b->show();
			layout()->addWidget(b);
			index = (index + 1) % banner_list.size();
			if (index == current_index || index == next_index)
				break;
		}
	}
	else
	{
		QBoxLayout *l = qobject_cast<QVBoxLayout*>(layout());
		for (int i = 0; i < banner_list.size(); ++i)
		{
			banner *b = banner_list.at(i);
			b->show();
			l->addWidget(b);
		}
		l->addStretch(1);
	}
}

void ContentWidget::pgUp()
{
	current_index = calculateNextIndex(false);
	Q_ASSERT_X(current_index != -1, "ContentWidget::pgUp", "calculateNextIndex return -1!");
	need_update = true;
	drawContent();
}

void ContentWidget::pgDown()
{
	current_index = calculateNextIndex(true);
	Q_ASSERT_X(current_index != -1, "ContentWidget::pgDown", "calculateNextIndex return -1!");
	need_update = true;
	drawContent();
}

int ContentWidget::calculateNextIndex(bool up_to_down)
{
	int area_height = contentsRect().height();
	int banners_height = 0;
	int index = current_index;

	while (true)
	{
		banner *b = banner_list.at(index);
		banners_height += b->sizeHint().height();
		if (banners_height > area_height)
			return index;

		// We add "banner_list.size()" to ensure that the module is always a positive number
		index = (index + (up_to_down ? 1 : -1) + banner_list.size()) % banner_list.size();
	}
	return -1;
}

#endif // LAYOUT_BTOUCH
