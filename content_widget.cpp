#include "content_widget.h"
#include "banner.h"

#include <QBoxLayout>
#include <QDebug>


ContentWidget::ContentWidget(QWidget *parent) : QWidget(parent)
{
	current_index = 0;
	max_banner = 3;
	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	need_update = true;
	scroll_step = max_banner;
}

int ContentWidget::bannerCount()
{
	return banner_list.size();
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
		if (banner_list.at(idx)->getId() == b->getId())
		{
			b->setSerNum(banner_list.at(idx)->getSerNum() + 1);
			idx = -1;
		}
}

void ContentWidget::resetIndex()
{
	current_index = 0;
	need_update = true;
	updateLayout();
}

void ContentWidget::showEvent(QShowEvent *e)
{
	updateLayout();
	QWidget::showEvent(e);
}

void ContentWidget::updateLayout()
{
	if (!need_update)
		return;

	need_update = false;

	// We want a circular list of banner, so we can't use a layout and hide/show
	// the banner to display or when you click the up button the order is wrong.
	// So we remove all the child from the layout, hide them and re-add to the
	// layout only the banner to show.
	QLayoutItem *child;
	while ((child = layout()->takeAt(0)) != 0)
		child->widget()->hide();

	for (int i = 0; i < max_banner; ++i)
	{
		int index = (i + current_index) % banner_list.size();
		layout()->addWidget(banner_list.at(index));
		banner_list.at(index)->show();
	}
}

void ContentWidget::upClick()
{
	current_index -= scroll_step;
	// We add "banner_list.size()" to ensure that the module is a positive number
	current_index = (current_index + banner_list.size()) % banner_list.size();
	need_update = true;
	updateLayout();
}

void ContentWidget::downClick()
{
	current_index += scroll_step;
	current_index = current_index % banner_list.size();
	need_update = true;
	updateLayout();
}

