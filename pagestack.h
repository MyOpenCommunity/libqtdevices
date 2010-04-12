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


#ifndef PAGESTACK_H
#define PAGESTACK_H

#include <QList>
#include <QObject>

class Page;
class Window;


// manages the stack of pages/window active on the screen and decides which
// is the next window/page to be shown when the top page is closed/destroyed
class PageStack : public QObject
{
	struct State
	{
		State(Window *window);
		State(Page *page);

		// either window or page are set, not both
		Window *window;
		Page *page;

		// returns either window or page, depending on which is set
		QObject *object() const;
	};

Q_OBJECT
public:
	PageStack();

	// register when different types of window/pages are shown; must be
	// called before Page::showPage() and Window::showWindow()

	// for alarm pages; stops the screen saver but keeps the screen frozen
	// if it already is
	void showAlarm(Page *alarm_page);

	// for VCTCallPage and IntercomPage; stops the screen saver but keeps the
	// screen frozen if it already is
	void showVCTPage(Page *page);

	// for windows that do not need any special treatment
	void showUserWindow(Window *window);

	// for pages that do not need any special treatment
	void showUserPage(Page *window);

	// for the keypad
	void showKeypad(Window *keypad);

	// removes the given object from the page stack and shows the
	// topmost stack object; when an object is removed from the middle
	// of the stack, no action is performed
	void closeWindow(Window *window);
	void closePage(Page *page);

public slots:
	// tracks the currently displayed page
	void currentPageChanged(Page *page);

private:
	void showState(const State &state);
	void addState(const State &state);
	void removeFromStack(QObject *obj);

private slots:
	void closed();
	void removeObject(QObject *obj);

private:
	QList<State> states;
};

namespace bt_global { extern PageStack page_stack; }

#endif // PAGESTACK_H
