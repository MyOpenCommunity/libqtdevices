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


#ifndef BANNERPAGE_H
#define BANNERPAGE_H

#include <QWidget>
#include <QList>


#include "scrollablepage.h"

class banner;
class NavigationBar;
class BannerContent;


/**
 * \class BannerPage
 *
 * A page containing a list of banners.
 */
class BannerPage : public ScrollablePage
{
public:
	// the type returned by page_content
	typedef BannerContent ContentType;

	BannerPage(QWidget *parent = 0);

	virtual void inizializza();

protected:
	void buildPage(BannerContent *content, NavigationBar *nav_bar, const QString &title = QString(), QWidget *top_widget=0);
	/**
	 * Utility function to build a standard banner page.
	 *
	 * Connections are created between content and navigation bar, and between content and page.
	 */
	void buildPage(const QString &title = QString(), int title_height = TITLE_HEIGHT, QWidget *top_widget = 0);

	/**
	 * Create a page with a custom content that contains a BannerContent.
	 *
	 * Some pages need to display some widgets around the content. Use this overload for such cases. Only the
	 * BannerContent will scroll using the navigation bar, the other widgets will not move.
	 * Connections are created between content and navigation bar, and between content and page.
	 */
	void buildPage(QWidget *main_widget, BannerContent *content, NavigationBar *nav_bar,
		const QString &title = QString(), int title_height = TITLE_HEIGHT, QWidget *top_widget = 0);
};


/**
 * The BannerContent class manage a list of banner usually put inside a Page.
 */
class BannerContent : public ScrollableContent
{
Q_OBJECT
public:
#ifdef LAYOUT_BTOUCH
	BannerContent(QWidget *parent=0);
#else
	BannerContent(QWidget *parent=0, int columns=2);
#endif

	void appendBanner(banner *b);
	int bannerCount();
	banner *getBanner(int i);
	void initBanners();

protected:
	void drawContent();

private:
	QList<banner*> banner_list;
	int columns;
};

#endif

