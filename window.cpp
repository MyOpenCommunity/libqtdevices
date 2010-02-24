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


#include "window.h"
#include "windowcontainer.h"


WindowContainer *Window::window_container = 0;


Window::Window()
{
	Q_ASSERT_X(window_container, "Window::Window", "WindowContainer not set!");

	window_container->addWindow(this);
}

void Window::setWindowContainer(WindowContainer *container)
{
	window_container = container;
}

void Window::showWindow()
{
	window_container->showWindow(this);
}

