#include "bannercontent.h"
#include "banner.h"
#include "fontmanager.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QVariant>
#include <QFontMetrics>


#ifdef LAYOUT_TOUCHX

BannerContent::BannerContent(QWidget *parent) : GridContent(parent)
{
	QGridLayout *l = static_cast<QGridLayout *>(layout());
	l->setContentsMargins(18, 0, 17, 0);
	l->setSpacing(0);
	// use column 1 for the vertical separator bar
	l->setColumnStretch(0, 1);
	l->setColumnStretch(2, 1);
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
		prepareLayout(items, 2);

		// add items to the layout
		for (int i = 0; i < pages.size() - 1; ++i)
		{
			int base = pages[i];
			for (int j = 0; base + j < pages[i + 1]; ++j)
				l->addWidget(banner_list.at(base + j), j / 2, (j % 2) * 2);
		}

		l->setRowStretch(l->rowCount(), 1);

		// construct the vertical separator widget
		QWidget *vertical_bar = new QWidget;
		vertical_bar->setProperty("VerticalSeparator", true);

		// create the layout with a spacer at the bottom, to
		// mimick the layout of current code
		QFont label_font = bt_global::font->get(FontManager::BANNERDESCRIPTION);
		QVBoxLayout *bar_layout = new QVBoxLayout;
		bar_layout->addWidget(vertical_bar, 1);
		bar_layout->addItem(new QSpacerItem(20, QFontMetrics(label_font).height()));

		// add the vertical bar to the layout
		l->addLayout(bar_layout, 0, 1);
	}

	updateLayout(items);

	// resize the vertical separator to span all completely filled rows
	// and ignore the last row if it only contains a single item; pages with only
	// one item need to be handled as a special case because QGridLayout does not
	// support items wiht colspan 0
	bool show_vertical_bar = pages[current_page + 1] - pages[current_page] >= 2;

	QLayoutItem *vertical_separator = l->itemAtPosition(0, 1);
	l->removeItem(vertical_separator);
	vertical_separator->layout()->itemAt(0)->widget()->setVisible(show_vertical_bar);
	l->addItem(vertical_separator, 0, 1,
		   (pages[current_page + 1] - pages[current_page]) / 2, 1);
}

#endif // LAYOUT_TOUCHX
