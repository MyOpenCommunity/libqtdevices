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


/*!
	\ingroup Core
	\brief A page containing a list of \ref banner%s.

	The BannerPage is a ScrollablePage subclass, so if the content is to big
	to be visualized in a single screen, it is paginated.

	As in ScrollablePage, all the logic about banner arranging into the single
	page is delegate to the content class, in this case the BannerContent class.

	\sa ScrollableContent
*/
class BannerPage : public ScrollablePage
{
public:
	// the type returned by page_content
	typedef BannerContent ContentType;

	/*!
		\brief Constructor.

		Constructs a new BannerPage with the given \a parent.
	*/
	BannerPage(QWidget *parent = 0);

	virtual void inizializza();

protected:
	/*!
		\brief Creates a page with a custom content that contains a BannerContent.

		Some pages need to display some widgets around the content. Use this overload for such cases. Only the
		BannerContent will scroll using the navigation bar, the other widgets will not move.
		Connections are created between content and navigation bar, and between content and page.
	 */
	void buildPage(QWidget *main_widget, BannerContent *content, NavigationBar *nav_bar,
		const QString &title = QString(), int title_height = TITLE_HEIGHT);
	/*!
		\overload void buildPage(BannerContent *content, NavigationBar *nav_bar, const QString &title = QString(), QWidget *top_widget=0);
	*/
	void buildPage(BannerContent *content, NavigationBar *nav_bar, const QString &title = QString());
	/*!
		\overload void buildPage(const QString &title = QString(), int title_height = TITLE_HEIGHT, QWidget *top_widget = 0);
		\brief Utility function to build a standard banner page.

		Connections are created between content and navigation bar, and between
		content and page.
	 */
	void buildPage(const QString &title = QString(), int title_height = TITLE_HEIGHT);

	/*!
		\brief Sets the vertical spacing between banners.

		\warning You must call the buildPage() method before this one.
	*/
	void setSpacing(int spacing);
};


/*!
	\ingroup Core
	\brief Manages a list of banner usually put inside a Page.

	This class extends the ScrollableContent class, inhering all the logic about
	pagination.

	The drawContent() method is reimplemented to correctly arrange banners inside
	the content and to optionally draw a separator between columns (depending on
	the platform).

	If you want to append a banner to the content you can use the appendBanner()
	method. You can get it back again using the getBanner() method.

	The number of banners actually contained in content can be retrived using the
	bannerCount() method.

	\sa ScrollableContent
 */
class BannerContent : public ScrollableContent
{
Q_OBJECT
public:
#ifdef LAYOUT_TS_3_5
	BannerContent(QWidget *parent=0);
#else
	/*!
		\brief Constructor.

		Constructs a new BannerContent with the given \a parent and \a columns.

		\note For the 3.5" the \a columns parameter does not exists because
			  banners are always arranged in one column.
	*/
	BannerContent(QWidget *parent=0, int columns=2);
#endif

	/*!
		\brief Appends \a b to the content.
	*/
	void appendBanner(banner *b);

	/*!
		\brief Returns the number of banners contained by the content.
	*/
	int bannerCount();

	/*!
		\brief Returns the banner at position \a i;
	*/
	banner *getBanner(int i);

	/*!
		\brief Initialize all the banners contained in content.
	*/
	void initBanners();

	/*!
		\brief Clear the content and destroy the all the banners contained in content.
	*/
	void clear();

	/*!
		\brief Layouts the banners into the page.

		On the 3.5" devices, the banners are arranged on one column only.
		On the 10" devices they are arranged on more columns (usually two).
	*/
	void drawContent();

private:
	QList<banner*> banner_list;
	int columns;
};

#endif

