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


#include "scrollablepage.h"
#include "navigation_bar.h"

#include <QGridLayout>


ScrollablePage::ScrollablePage(QWidget *parent) : Page(parent)
{
}

void ScrollablePage::activateLayout()
{
	if (page_content)
		page_content->updateGeometry();

	Page::activateLayout();

	if (page_content)
		page_content->drawContent();
}


void ScrollablePage::buildPage(QWidget *main_widget, ScrollableContent *content, AbstractNavigationBar *nav_bar, QWidget *top_widget, PageTitleWidget *title_widget)
{
	Page::buildPage(main_widget, content, nav_bar, top_widget, title_widget);
	if (title_widget)
		connect(content, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), content, SLOT(resetIndex()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(forwardClick()));
	connect(nav_bar, SIGNAL(upClick()), content, SLOT(pgUp()));
	connect(nav_bar, SIGNAL(downClick()), content, SLOT(pgDown()));
	connect(content, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));
}


ScrollableContent::ScrollableContent(QWidget *parent) : QWidget(parent)
{
	(void) new QGridLayout(this);

	current_page = 0;
	need_update = true;
}

void ScrollableContent::resetIndex()
{
	need_update = true;
	current_page = 0;
}

void ScrollableContent::showEvent(QShowEvent *e)
{
	if (need_update)
		drawContent();
	need_update = false;
	QWidget::showEvent(e);
}

void ScrollableContent::prepareLayout(QList<QWidget *> items, int columns)
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

void ScrollableContent::updateLayout(QList<QWidget *> items)
{
	emit displayScrollButtons(pageCount() > 1);
	emit contentScrolled(current_page, pageCount());

	need_update = false;

	// works for all pages because the last item of the pages array always
	// contains banner_list.size()
	for (int i = 0; i < items.size(); ++i)
		items[i]->setVisible(i >= pages[current_page] && i < pages[current_page + 1]);
}

void ScrollableContent::pgUp()
{
	current_page = (current_page - 1 + pageCount()) % pageCount();
	drawContent();
	need_update = false;
}

void ScrollableContent::pgDown()
{
	current_page = (current_page + 1) % pageCount();
	drawContent();
	need_update = false;
}

int ScrollableContent::pageCount() const
{
	return pages.size() - 1;
}

int ScrollableContent::currentPage() const
{
	return current_page;
}

