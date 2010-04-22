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


#include "pagestack.h"
#include "page.h"
#include "window.h"
#include "btmain.h"

#include <QtDebug>


PageStack::State::State(Window *_window)
{
	window = _window;
	page = NULL;
}

PageStack::State::State(Page *_page)
{
	window = NULL;
	page = _page;
}

QObject *PageStack::State::object() const
{
	return page ? static_cast<QObject*>(page) : static_cast<QObject*>(window);
}


PageStack::PageStack()
{
	states.append(State(static_cast<Window*>(NULL)));
}

void PageStack::showAlert(Page *alert_page)
{
	addState(State(alert_page));

	connect(alert_page, SIGNAL(Delete()), SLOT(closed()));

	bt_global::btmain->makeActiveAndFreeze();
}

void PageStack::showVCTPage(Page *page)
{
	addState(State(page));

	connect(page, SIGNAL(Closed()), SLOT(closed()));

	bt_global::btmain->makeActiveAndFreeze();
}

void PageStack::showKeypad(Window *window)
{
	addState(State(window));
}

void PageStack::showUserWindow(Window *window)
{
	addState(State(window));

	connect(window, SIGNAL(Closed()), SLOT(closed()));
}

void PageStack::showUserPage(Page *page)
{
	addState(State(page));

	connect(page, SIGNAL(Closed()), SLOT(closed()));
}

void PageStack::addState(const State &state)
{
	removeFromStack(state.object());
	// most of the time, the correct thing is to put the new page/window at
	// the top of the stack
	//
	// during calibration, insert new states just below the calibration page
	if (!BtMain::isCalibrating())
		states.append(state);
	else
		states.insert(states.size() - 1, state);

	connect(state.object(), SIGNAL(destroyed(QObject*)), SLOT(removeObject(QObject*)));
}

void PageStack::showState(const State &state)
{
	if (state.page)
		state.page->showPage();
	else if (state.window)
		state.window->showWindow();
}

void PageStack::currentPageChanged(Page *page)
{
	qDebug() << "PageStack::currentPageChanged on" << page;

	for (int i = 0; i < states.size(); ++i)
		if (states[i].page == page)
			return;

	states[0].page = page;
}

void PageStack::closed()
{
	removeObject(sender());
}

void PageStack::closeWindow(Window *window)
{
	removeObject(window);
}

void PageStack::closePage(Page *page)
{
	removeObject(page);
}

void PageStack::removeObject(QObject *obj)
{
	disconnect(obj, 0, this, 0);

	State top = states.back();

	if (top.object() == obj)
	{
		states.pop_back();
		showState(states.back());
	}
	else
		removeFromStack(obj);
}

void PageStack::removeFromStack(QObject *obj)
{
	for (int i = 0; i < states.size(); ++i)
	{
		if (states[i].object() == obj)
		{
			states.removeAt(i);
			break;
		}
	}
}

void PageStack::clear()
{
	while (states.size() > 1)
	{
		State el = states.takeLast();
		if (el.page)
			el.page->cleanUp();
	}
}

PageStack bt_global::page_stack;
