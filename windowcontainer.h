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


#ifndef WINDOWCONTAINER_H
#define WINDOWCONTAINER_H

#include <QStackedWidget>

class PageContainer;
class HomeWindow;
class Window;
class TransitionWidget;
class QPixmap;



/*!
	\ingroup Core
	\brief The top level widget and container for fullscreen widgets.

*/
class WindowContainer : public QStackedWidget
{
Q_OBJECT
public:
	/*!
		\brief Build the %WindowContainer with the given \a width and \a height.

		Construct the %WindowContainer setting also the Window::window_container
		to itself and building thee HomeWindow. It also set the PageContainer
		built inside the HomeWindow as the PageContainer for all the pages.
	*/
	WindowContainer(int width, int height);

	PageContainer *centralLayout();

	/*!
		\brief Return the HomeWindow.
	*/
	HomeWindow *homeWindow();

	/*!
		\brief Return the current Window displayed.
	*/
	Window *currentWindow();

	/*!
		\brief Add a Window to the stack of the windows.
	*/
	void addWindow(Window *w);

	/*!
		\brief Set the Window \a w as the current window displayed.
	*/
	void showWindow(Window *w);

	/*!
		\brief Install a widget for transitions.
		\warning This method is unmantained, do not use.
		\internal WindowContainer take the ownership of the TransitionWidget
	*/
	void installTransitionWidget(TransitionWidget *tr);

	QPixmap grabHomeWindow();

private:
	TransitionWidget *transition_widget;

	HomeWindow *main;
};

#endif // WINDOWCONTAINER_H
