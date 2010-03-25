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

#include "itemlistpage.h"
#include "navigation_bar.h"

#include <QLayout>


ItemListPage::ItemListPage()
{
}

void ItemListPage::buildPage(int items, const QString &title, int title_height)
{
	PageTitleWidget *title_widget = new PageTitleWidget(title, title_height);
	ItemList *item_list = new ItemList(NULL, items);
	NavigationBar *nav_bar = new NavigationBar;

	// scrolling
	connect(nav_bar, SIGNAL(upClick()), item_list, SLOT(prevItem()));
	connect(nav_bar, SIGNAL(downClick()), item_list, SLOT(nextItem()));
	connect(item_list, SIGNAL(displayScrollButtons(bool)), nav_bar, SLOT(displayScrollButtons(bool)));

	// current page
	connect(item_list, SIGNAL(contentScrolled(int, int)), title_widget, SLOT(setCurrentPage(int, int)));

	// close page on back
	connect(nav_bar, SIGNAL(backClick()), this, SIGNAL(Closed()));

	Page::buildPage(item_list, nav_bar, 0, title_widget);
	layout()->setContentsMargins(0, 5, 25, 10);
}
