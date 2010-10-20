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

#ifndef PAGE_CONTAINER_H
#define PAGE_CONTAINER_H

#include <QStackedWidget>

class TransitionWidget;
class Page;
class Window;


/*!
	\ingroup Core
	\brief The container for pages.

	This class is very similar to the QStackedWidget one but adds the signal
	currentChanging() to notify when the Page is about to change
	and the signal currentPageChanged() when the page is actually
	changed.

	Usually you don't have to deal with this class directly, because a Page add
	itself to the container when created, and set itself as the current page
	when called its method Page::showPage().

	\warning This class contains methods to install widget for transition effects
	and deal with it:
	that code is partially unmantained and might be removed in the future, so use
	it at your own risk.
*/
class PageContainer : public QStackedWidget
{
Q_OBJECT
public:
	PageContainer(QWidget *parent);

	/*!
		\brief Install a widget for transitions.
		\warning This method is unmantained, do not use.
	*/
	void installTransitionWidget(TransitionWidget *tr);

	/*!
		\brief Set the Page \a p as the current, skipping transition if enabled.
		\warning This method is unmantained, do not use.
	*/
	void setCurrentPage(Page *p);

	/*!
		\brief Set the Page \a p as the current.
	*/
	void showPage(Page *p);

	/*!
		\brief Add the page \a p at the stack of the pages.
	*/
	void addPage(Page *p);

	/*!
		\brief Return the current page.
		\internal Or the previous if during a transition.
	*/
	Page *currentPage();

	/*!
		\brief Stop the transition effect implicity called by the showPage method.
		\warning This method is unmantained, do not use.
	*/
	void blockTransitions(bool block);

	void prepareTransition();
	void startTransition(Page *p);

	/*!
		\brief Set the Window \a window as the container of the page.

		The \a window argument is considered as the parent, and it is showed
		every time that a Page in the child PageContainer is shown.
	*/
	void setContainerWindow(Window *window);

signals:
	/*!
		\brief Emitted when the current page has changed.

		\sa currentChanging()
	*/
	void currentPageChanged(Page *p);

	/*!
		\brief Emitted when the current page is about to change.

		\sa currentPageChanged()
	*/
	void currentChanging(Page *p);

private:
	bool block_transitions;
	TransitionWidget *transition_widget;

	// The previous page if during the transition, 0 otherwise
	Page *prev_page;
	// The destination page if during the transition, 0 otherwise
	Page *dest_page;

	Window *parent_window;

private slots:
	void endTransition();
};

#endif // PAGE_CONTAINER_H
