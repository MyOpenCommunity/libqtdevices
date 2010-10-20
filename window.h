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


#ifndef WINDOW_H
#define WINDOW_H

#include "styledwidget.h"

class WindowContainer;


/*!
	\ingroup Core
	\brief Base class for fullscreen widgets.

	A %Window behaves like a fullscreen Page. It is contained in a QStackedWidget
	like container (the WindowContainer) and it can show itself using the
	showWindow() method. When a page is about to hide, the aboutToHideEvent()
	callback is called.
	Every time that a %Window is closed, it should be emit the signal Closed().
*/
class Window : public StyledWidget
{
friend class WindowContainer;
Q_OBJECT
public:
	/*!
	\brief Constructor.

	Build the window and add it to the WindowContainer
	*/
	Window();

	/*!
		\brief A callback method called right before the window is hidden.
	*/
	virtual void aboutToHideEvent() {};

public slots:
	/*!
		\brief Make the window the current window displayed.
	*/
	virtual void showWindow();

signals:
	void Closed();

private:
	static WindowContainer *window_container;
};

#endif // WINDOW_H

