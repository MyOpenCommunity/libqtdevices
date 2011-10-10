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


// Manage the transition effects. This class is designed to be used indirectly
// through the PageContainer.
class TransitionManager : public QObject
{
Q_OBJECT
public:
	TransitionManager(QObject *parent = 0);
	// Set or unset the transition widget.
	void setTransitionWidget(TransitionWidget *tr);
	// Return true if a transition widget is installed and the transition effects
	// are not disabled
	bool isActive() const;

	// Prepare the transition using a snapshot of the prev widget as the starting one.
	void prepareTransition(QWidget *prev);

	// Start the transition using a snapshot of the next widget as target.
	void startTransition(QWidget *next);

	// Disable or enable the transition effects.
	void blockTransitions(bool block);

private slots:
	void endTransition();

private:
	TransitionWidget *transition_widget;
	bool block_transitions;
};


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
		\brief Install a widget for transitions, replacing the old one if exists.
		\note The ownership of the widget argument is taken from the PageContainer.
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
		\brief Block the transition effect.

		Block the transition effect implicity called (if the transition effects
		are active) by the showPage method.
	*/
	void blockTransitions(bool block);

	/*!
		\brief Prepare a transition using a snapshot of the widget argument as
		starting widget.
	*/
	void prepareTransition(QWidget *w);

	/*!
		\brief Start a transition using a snapshot of the widget argument as
		target widget.
	*/
	void startTransition(QWidget *w);

	/*!
		\brief Set the Window \a window as the container of the page.

		The \a window argument is considered as the parent, and it is shown
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
	Window *parent_window;

	TransitionManager* transition_manager;
};

#endif // PAGE_CONTAINER_H
