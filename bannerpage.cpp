/*
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "bannerpage.h"
#include "navigation_bar.h"
#include "banner.h"
#include "fontmanager.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QVariant>
#include <QFontMetrics>

#define VERTICAL_BAR_WIDTH 20


BannerPage::BannerPage(QWidget *parent) : ScrollablePage(parent)
{
}

void BannerPage::inizializza()
{
	if (page_content)
		page_content->initBanners();
}

void BannerPage::buildPage(BannerContent *content, NavigationBar *nav_bar, const QString &title, QWidget *top_widget)
{
	buildPage(content, content, nav_bar, title, TITLE_HEIGHT, top_widget);
}

void BannerPage::buildPage(QWidget *main_widget, BannerContent *content, NavigationBar *nav_bar,
	const QString &title, int title_height, QWidget *top_widget)
{
	PageTitleWidget *title_widget = 0;
#ifdef LAYOUT_TS_10
	title_widget = new PageTitleWidget(title, title_height);
#endif

	ScrollablePage::buildPage(main_widget, content, nav_bar, top_widget, title_widget);
}

void BannerPage::buildPage(const QString &title, int title_height, QWidget *top_widget)
{
	BannerContent *content = new BannerContent;

	buildPage(content, content, new NavigationBar, title, title_height, top_widget);
}

void BannerPage::setSpacing(int spacing)
{
	Q_ASSERT_X(page_content != 0, "BannerPage::setSpacing()", "page_content is uninitialized.");

	static_cast<QGridLayout *>(page_content->layout())->setVerticalSpacing(spacing);
}


#ifdef LAYOUT_TS_3_5
BannerContent::BannerContent(QWidget *parent) : ScrollableContent(parent), columns(1)
#else
BannerContent::BannerContent(QWidget *parent, int _columns) : ScrollableContent(parent), columns(_columns)
#endif
{
	QGridLayout *l = static_cast<QGridLayout *>(layout());
#ifdef LAYOUT_TS_3_5
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
#else
	l->setContentsMargins(18, 0, 17, 0);
	l->setHorizontalSpacing(0);
	l->setVerticalSpacing(5);
#endif
	// use column 1 for the vertical separator bar
	l->setColumnStretch(0, 1);
	if (columns == 2)
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
	need_update = true;
	banner_list.append(b);
	b->hide();

	for (int idx = banner_list.size() - 2; idx >= 0; idx--)
		if (banner_list.at(idx)->getId() == b->getId() && b->getId() != -1)
		{
			b->setSerNum(banner_list.at(idx)->getSerNum() + 1);
			idx = -1;
		}
}

void BannerContent::clear()
{
	for (int i = 0; i < banner_list.size(); ++i)
		banner_list[i]->deleteLater();
	banner_list.clear();
	pages.clear();
	current_page = 0;
}

void BannerContent::drawContent()
{
	QGridLayout *l = qobject_cast<QGridLayout*>(layout());

	// copy the list to pass it to ScrollableContent methods; as an alternative
	// we could change them to template methods
	QList<QWidget *> items;
	for (int i = 0; i < banner_list.size(); ++i)
		items.append(banner_list[i]);

	if (pages.size() == 0)
	{
		// prepare the page list
		prepareLayout(items, columns);

		// add items to the layout
		for (int i = 0; i < pages.size() - 1; ++i)
		{
			int base = pages[i];
			for (int j = 0; base + j < pages[i + 1]; ++j)
				l->addWidget(banner_list.at(base + j), j / columns, (j % columns) * columns);
		}

		l->setRowStretch(l->rowCount(), 1);

		if (columns == 2)
		{
			// construct the vertical separator widget
			QWidget *vertical_bar = new QWidget;
			vertical_bar->setProperty("VerticalSeparator", true);

			// create the layout with a spacer at the bottom, to
			// mimick the layout of current code
			QFont label_font = bt_global::font->get(FontManager::BANNERDESCRIPTION);
			QVBoxLayout *bar_layout = new QVBoxLayout;
			bar_layout->addWidget(vertical_bar, 1);
			bar_layout->addItem(new QSpacerItem(VERTICAL_BAR_WIDTH, QFontMetrics(label_font).height()));

			// add the vertical bar to the layout
			l->addLayout(bar_layout, 0, 1);
		}
	}

	updateLayout(items);

	// resize the vertical separator to span all rows that contain a banner, except for
	// the case when there is a single banner in the page; the latter case
	// needs to be handled as a special case because QGridLayout does not
	// support items wiht colspan 0
	bool show_vertical_bar = columns == 2 && pages[current_page + 1] - pages[current_page] >= 2;

	if (columns == 2)
	{
		QLayoutItem *vertical_separator = l->itemAtPosition(0, 1);
		l->removeItem(vertical_separator);
		vertical_separator->layout()->itemAt(0)->widget()->setVisible(show_vertical_bar);
		l->addItem(vertical_separator, 0, 1,
			   (pages[current_page + 1] - pages[current_page] + 1) / 2, 1);

		// We want the columns with the same size, so we assign a minimum size.
		// Note that in this phase (during the building of the page), the Qt functions
		// to get the size of the margins and vertical bar are not working.
		int total_width = contentsRect().width();
		if (show_vertical_bar)
			total_width -= VERTICAL_BAR_WIDTH;

		l->setColumnMinimumWidth(0, total_width / 2);
		l->setColumnMinimumWidth(2, total_width / 2);
	}
}
