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


#include "windowcontainer.h"
#include "page.h" // Page::setPageContainer
#include "window.h" // Window::setWindowContainer
#include "transitionwidget.h"
#include "btmain.h" // bt_global::status


#include <QStackedLayout>


WindowContainer::WindowContainer(int width, int height, QWidget *parent) : QStackedWidget(parent)
{
	transition_widget = 0;
	// needs to be done before HomeWindow is constructed
	Window::window_container = this;

	QStackedLayout *root_layout = static_cast<QStackedLayout *>(layout());
	root_layout->setSpacing(0);
	root_layout->setContentsMargins(0, 0, 0, 0);

#ifdef Q_WS_QWS
	showFullScreen();
#else
	show();
	setWindowTitle(tr("BTouch"));
#endif
	setFixedSize(width, height);
}

void WindowContainer::addWindow(Window *w)
{
	addWidget(w);
	w->resize(width(), height());
}

void WindowContainer::showWindow(Window *w)
{
	if (currentWindow() == w)
		return;

	// during calibration, do not display new window/pages; the page stack
	// will ensure that we return to the correct page after calibration
	if (bt_global::status.calibrating)
		return;

	currentWindow()->aboutToHideEvent();
	setCurrentWidget(w);
}

Window *WindowContainer::currentWindow()
{
	return static_cast<Window*>(currentWidget());
}

void WindowContainer::installTransitionWidget(TransitionWidget *tr)
{
	if (transition_widget)
	{
		removeWidget(transition_widget);
		transition_widget->disconnect();
		transition_widget->deleteLater();
	}

	transition_widget = tr;
	addWidget(transition_widget);
	transition_widget->setContainer(this);
}

