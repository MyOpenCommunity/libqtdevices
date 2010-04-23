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


#include "homewindow.h"
#include "page.h"
#ifdef LAYOUT_TOUCHX
#include "headerwidget.h"
#include "favoriteswidget.h"
#endif

#include "hardware_functions.h"
#include "skinmanager.h"
#include "main.h" // getHomepageNode
#include "xml_functions.h"
#include "pagecontainer.h"
#include "btbutton.h"
#include "pagestack.h" // bt_global::page_stack

#include <QHBoxLayout>
#include <QGridLayout>
#include <QtDebug>
#include <QVariant>


TrayBar::TrayBar()
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(2, 0, 2, 0);
	l->setSpacing(3);
}

void TrayBar::addButton(BtButton *b)
{
	layout()->addWidget(b);
}


HomeWindow::HomeWindow()
{
	header_widget = 0;
	favorites_widget = 0;
	central_widget = 0;

#ifdef LAYOUT_TOUCHX
	QDomNode pagemenu_home = getHomepageNode();
	int favourites_pageid = getTextChild(pagemenu_home, "h_lnk_pageID").toInt();
	QDomNode favourites_node = getPageNodeFromPageId(favourites_pageid);

	SkinContext cxt(getTextChild(pagemenu_home, "cid").toInt());

	QGridLayout *main_layout = new QGridLayout(this);
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);

	central_widget = new PageContainer(this);
	main_layout->addWidget(central_widget, 1, 0, 1, 1);
	// using the currentChanging signal, we ensure that the page size is the correct one
	// when showEvent is called
	connect(central_widget, SIGNAL(currentChanging(int)), SLOT(centralWidgetChanged(int)));
	Page::setPageContainer(central_widget);

	tray_bar = new TrayBar;
	header_widget = new HeaderWidget(tray_bar);
	main_layout->addWidget(header_widget, 0, 0, 1, 2);

	favorites_widget = new FavoritesWidget;

	// container widget for favorites
	QWidget *f = new QWidget;
	f->setProperty("Favorites", true); // for stylesheet
	QVBoxLayout *l = new QVBoxLayout(f);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);

	l->addStretch(1);
	l->addWidget(favorites_widget, 1);

	main_layout->addWidget(f, 1, 1, 1, 1);

	connect(header_widget, SIGNAL(aboutToChangePage()), SLOT(aboutToChangePage()));
	connect(header_widget, SIGNAL(showSectionPage(int)), SIGNAL(showSectionPage(int)));
	connect(header_widget, SIGNAL(showHomePage()), SIGNAL(showHomePage()));
#else
	central_widget = new PageContainer(this);
	Page::setPageContainer(central_widget);
	central_widget->setFixedSize(maxWidth(), maxHeight());
#endif

	central_widget->setContainerWindow(this);

	connect(&bt_global::page_stack, SIGNAL(sectionChanged(int)), SLOT(currentSectionChanged(int)));
}

void HomeWindow::aboutToChangePage()
{
	bt_global::page_stack.clear();
}

void HomeWindow::loadConfiguration()
{
#ifdef LAYOUT_TOUCHX
	QDomNode pagemenu_home = getHomepageNode();
	QDomNode favourites_node = getPageNodeFromChildNode(pagemenu_home, "h_lnk_pageID");
	QDomNode infobar_node = getPageNodeFromChildNode(favourites_node, "h_lnk_pageID");

	SkinContext cxt(getTextChild(pagemenu_home, "cid").toInt());

	header_widget->loadConfiguration(pagemenu_home, infobar_node);
	favorites_widget->loadItems(favourites_node);
#endif
}

void HomeWindow::centralWidgetChanged(int index)
{
#ifdef LAYOUT_TOUCHX
	// the check on header_widget shouldn't fail, but I don't want to rely on this...
	if (qobject_cast<Page *>(central_widget->widget(index)))
	{
		Page *p = static_cast<Page *>(central_widget->widget(index));
		header_widget->centralPageChanged(p->sectionId(), p->pageType());
		// force a relayout, because changing the central page might change
		// the window header size; see also the comment in PageContainer::showPage()
		layout()->activate();
	}
#endif
}

void HomeWindow::currentSectionChanged(int section_id)
{
#ifdef LAYOUT_TOUCHX
	header_widget->sectionChanged(section_id);
#endif
}

PageContainer *HomeWindow::centralLayout()
{
	return central_widget;
}

Page *HomeWindow::currentPage()
{
	return central_widget->currentPage();
}

