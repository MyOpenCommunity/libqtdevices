#include "content_widget.h"
#include "banner.h"

#include <QBoxLayout>


ContentWidget::ContentWidget(QWidget *parent) : QWidget(parent)
{
	current_index = 0;
	max_banner = 3;
	QVBoxLayout *l = new QVBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
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

	for (int idx = banner_list.size() - 2; idx >= 0; idx--)
		if (banner_list.at(idx)->getId() == b->getId())
		{
			b->setSerNum(banner_list.at(idx)->getSerNum() + 1);
			idx = -1;
		}

	QVBoxLayout *main_layout = static_cast<QVBoxLayout*>(layout());
	main_layout->addWidget(b);
}

void ContentWidget::paintEvent(QPaintEvent *e)
{
	for (int i = 0; i < banner_list.size(); ++i)
	{
		if (i >= current_index && i < current_index + max_banner)
			banner_list.at(i)->show();
		else
			banner_list.at(i)->hide();
	}
	QWidget::paintEvent(e);
}

void ContentWidget::upClick()
{
	if (current_index - max_banner >= 0)
		current_index -= max_banner;
	update();
}

void ContentWidget::downClick()
{
	if (current_index + max_banner < banner_list.size())
		current_index += max_banner;
	update();
}

