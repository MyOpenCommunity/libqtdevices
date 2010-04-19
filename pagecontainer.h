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


class PageContainer : public QStackedWidget
{
Q_OBJECT
public:
	PageContainer(QWidget *parent);

	// Install a widget for the transition. The transition widget must have the public functions:
	//
	// void startTransition(const QPixmap &prev, const QPixmap &dest);
	// void cancelTransition();
	// and the signal:
	// void endTransition();
	void installTransitionWidget(TransitionWidget *tr);

	// Set the argument page as the current page, without transition
	void setCurrentPage(Page *p);
	// Set the argument page as the current page, with transition if active
	void showPage(Page *p);

	// add the page at the stack of the pages
	void addPage(Page *p);
	// return the current page (or the previous if during a transition)
	Page *currentPage();

	// stop the transition effect implicity called by the showPage method.
	void blockTransitions(bool block);

	void prepareTransition();
	void startTransition(Page *p);

	void setContainerWindow(Window *window);

signals:
	void currentPageChanged(Page *p);

	// this signal is emitted when the current page is about to change; it can
	// be used by code that needs to know the new page before it is actually set;
	// to know that the current page has already changed, use the currentChanged()
	// signal from QStackedWidget
	void currentChanging(int index);

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
