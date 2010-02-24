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


// top level widget, contains the HomeWindow and other special widgets that
// need to be shown full screen (for example the screen saver and transition widgets)
class WindowContainer : public QStackedWidget
{
Q_OBJECT
public:
	WindowContainer(int width, int height);
	PageContainer *centralLayout();
	HomeWindow *homeWindow();

	void addWindow(Window *w);
	void showWindow(Window *w);

	// NOTE: WindowContainer take the ownership of the TransitionWidget
	void installTransitionWidget(TransitionWidget *tr);

	QPixmap grabHomeWindow();

private:
	TransitionWidget *transition_widget;

	HomeWindow *main;
};

#endif // WINDOWCONTAINER_H
