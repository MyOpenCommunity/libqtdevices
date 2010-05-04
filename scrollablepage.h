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


#ifndef SCROLLABLEPAGE_H
#define SCROLLABLEPAGE_H

#include "page.h"

#include <QList>


class AbstractNavigationBar;
class PageTitleWidget;
class ScrollableContent;


/**
 * This abstract page is a page that manage a scrollable content.
 */
class ScrollablePage : public Page
{
public:
	// the type returned by page_content
	typedef ScrollableContent ContentType;
	virtual void activateLayout();

protected:
	ScrollablePage(QWidget *parent = 0);

	void buildPage(QWidget *main_widget, ScrollableContent *content, AbstractNavigationBar *nav_bar,
		QWidget *top_widget = 0, PageTitleWidget *title_widget = 0);
};



/**
 * The ScrollableContent is an abstract class that manages a scrollable grid of subwidgets.
 */
class ScrollableContent : public QWidget
{
friend void ScrollablePage::activateLayout();
Q_OBJECT
public:
	ScrollableContent(QWidget *parent = 0);

public slots:
	void pgUp();
	void pgDown();
	void resetIndex();

signals:
	void displayScrollButtons(bool display);
	void contentScrolled(int current, int total);

protected:
	void showEvent(QShowEvent *e);

	void prepareLayout(QList<QWidget *> items, int columns);
	void updateLayout(QList<QWidget *> items);

	// The drawContent is the place where this widget is actually drawed. In order
	// to have a correct transition effect, this method is also called by the
	// Page _before_ that the Page is showed.
	virtual void drawContent() = 0;

	int pageCount() const;

protected:
	bool need_update; // a flag to avoid useless call to drawContent
	int current_page;
	QList<int> pages;
};

#endif

