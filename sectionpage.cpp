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


#include "sectionpage.h"
#include "hardware_functions.h" // rearmWDT
#include "xml_functions.h" // getTextChild, getChildren
#include "navigation_bar.h"
#include "main.h" // IMG_PATH
#include "pagefactory.h" // getPage
#include "skinmanager.h"
#include "state_button.h"

#include <QTime>
#include <QDomNode>
#include <QDebug>


SectionPage::SectionPage(const QDomNode &config_node)
{
#ifndef CONFIG_BTOUCH
	// only show the back icon if there is a lnk_itemID to a banner
	QString back_icon = getTextChild(config_node, "lnk_itemID").isEmpty() ? QString() : "back";
	NavigationBar *nav_bar = new NavigationBar("", "scroll_down", "scroll_up", back_icon);
	buildPage(new IconContent, nav_bar);
#endif
	loadItems(config_node);
}

SectionPage::SectionPage()
{
}

void SectionPage::addPage(Page *page, const QString &label, const QString &icon_name, const QString &icon_name_on, int x, int y)
{
	StateButton *b = new StateButton(this);
	b->setOffImage(icon_name);
	if (!icon_name_on.isEmpty())
		b->setOnImage(icon_name_on);

#ifdef LAYOUT_BTOUCH
	b->setGeometry(x, y, DIM_BUT, DIM_BUT);
#else
	Q_ASSERT_X(page_content, "SectionPage::addButton", "The SectionPage must have the page_content!");
	page_content->addButton(b, label);
#endif
	connect(b, SIGNAL(clicked()), page, SLOT(showPage()));
	connect(page, SIGNAL(Closed()), this, SLOT(showPage()));

	// NOTE: indexOfSignal doesn't work with the normal signature of a signal, that is "2<methodname>".
	// Removing the prefix of the signals (the 2) it works fine with Qt 4.5
	if (page->metaObject()->indexOfSignal(SIGNAL(changeIconState(StateButton::Status)) + 1) != -1)
		connect(page, SIGNAL(changeIconState(StateButton::Status)), b, SLOT(setStatus(StateButton::Status)));
}

void SectionPage::loadItems(const QDomNode &config_node)
{
	QTime wdtime;
	wdtime.start(); // Start counting for wd refresh

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
#ifdef CONFIG_BTOUCH
		int id = getTextChild(item, "id").toInt();
		QString img1 = IMG_PATH + getTextChild(item, "cimg1");
		int x = getTextChild(item, "left").toInt();
		int y = getTextChild(item, "top").toInt();

		// Within the pagemenu element, it can exists items that are not a page.
		if (Page *p = getPage(id))
			addPage(p, QString(), img1, QString(), x, y);
#else
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QString icon = bt_global::skin->getImage("link_icon");

		QString icon_on;
		if (bt_global::skin->exists("link_icon_on"))
			icon_on = bt_global::skin->getImage("link_icon_on");

		// Home page buttons don't have description set
		QString descr = getTextChild(item, "descr");

		// TODO some ids are not links
		int pageid = getTextChild(item, "lnk_pageID").toInt();
		if (Page *p = getPage(pageid))
			addPage(p, descr, icon, icon_on, 0, 0);
#endif

		if (wdtime.elapsed() > 1000)
		{
			wdtime.restart();
			rearmWDT();
		}
	}
}

