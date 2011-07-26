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


/*!
	\ingroup Core
	\brief Abstract page to manage a scrollable content.

	Like in the Page class you have to use the buildPage() method to make your
	custom page. This method layouts correctly the various component of the page
	depending on the hardware the application is running on, and connects the
	various component together.

	All the logic about scrolling is delegated to the ScrollableContent class,
	which is connected to the page navigation bar.

	\sa Page::buildPage(), ScrollableContent
 */
class ScrollablePage : public Page
{
public:
	// the type returned by page_content
	typedef ScrollableContent ContentType;
	virtual void activateLayout();

protected:
	/*!
		\brief Constructor.

		Protected to make the class not directly instantiable.
	*/
	ScrollablePage(QWidget *parent = 0);

	/*!
		\brief Connects the various element of the page each other.

		The pecularity of this method is that makes \a content to instruct
		the \a nav_bar to show the scroll buttons when needed, and the navbar to
		scroll \a content when the appropriate buttons are clicked.
	*/
	void buildPage(QWidget *main_widget, ScrollableContent *content, AbstractNavigationBar *nav_bar,
		PageTitleWidget *title_widget = 0);
};



/*!
	\ingroup Core
	\brief Abstract class that manages a scrollable grid of subwidgets.

	The ScrollableContent class manages all the logic about paginate
	its children widgets calculating the number of the pages on the widget
	basing it on the children size, number, and the number of columns the
	children are aranged in (prepareLayout() method).

	If the calculated page number is greater than 1, the signal
	displayScrollButtons() is emitted with true as parameter to inform the
	appropriate gui component (usually the navigation bar) that scrolling on the
	content is possible.

	To know the calculated number of pages you can use the pageCount() method.

	To create a new ScrollableContent subclass, you must implement the pure
	virtual method drawContent(), which is expected to be the one that performs
	widgets layout into the pages.

	You can switch between pages using the pgUp() and pgDown() methods, which
	are also slots, and are usually connected to the appropriate buttons on
	the navigation bar. When the page is switched the contentScrolled() signal
	is emitted.
 */
class ScrollableContent : public QWidget
{
friend void ScrollablePage::activateLayout();
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Construct a new ScrollableContent with the given \a parent.
	*/
	ScrollableContent(QWidget *parent = 0);

	/*!
		\brief Returns the current page index.
	*/
	int currentPage() const;

	/*!
		\brief The drawContent is the place where the content is actually drawn.

		When reimplemented into a subclass, this method is expected to arrange
		the children into the single page.

		\note In order to have a correct transition effect, this method is also
		called by the Page \b before that the Page is shown.
	*/
	virtual void drawContent() = 0;

public slots:
	/*!
		\brief Shows the previous page of items.
	*/
	void pgUp();
	/*!
		\brief Shows the next page of items.
	*/
	void pgDown();
	/*!
		\brief Returns to the first page.
	*/
	void resetIndex();

signals:
	/*!
		\brief Emitted when the layout is updated.

		\a display is true if scrolling is possible, false otherwise.
	*/
	void displayScrollButtons(bool display);
	/*!
		\brief Emitted when the content is scrolled.

		\a current is the index of the current displayed page,
		\a total is the total number of pages.
	*/
	void contentScrolled(int current, int total);


protected:
	void showEvent(QShowEvent *e);

	/*!
		\brief Calculates the number of pages needed to paginate the children.

		Using \a items number and sizes and \a column value, calculates the
		number of pages to make possible to display them all.
	*/
	void prepareLayout(QList<QWidget *> items, int columns);

	/*!
		\brief Shows the subset of \a items for the current page.
	*/
	void updateLayout(QList<QWidget *> items);

	/*!
		\brief Returns the page count calculated by the number of items and
		their size.
	*/
	virtual int pageCount() const;

protected:
	/*!
		\brief Flag that indicates if call to drawContent during the showEvent callback.
	*/
	bool need_update;
	/*!
		\brief Current page.
	*/
	int current_page;
	/*!
		\brief Pages indexes.
	*/
	QList<int> pages;
};

#endif

