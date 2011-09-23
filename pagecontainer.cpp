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


#include "pagecontainer.h"
#include "transitionwidget.h"
#include "page.h"
#include "window.h"
#include "btmain.h" // bt_global::status

#ifdef LAYOUT_TS_10
#include "vctcall.h"
#include "videodoorentry.h"
#endif

#include <QDebug>
#include <QLayout>
#include <QTimer>


namespace
{
	void fixVisualization(Page *p, QSize size)
	{
		p->resize(size);

		if (p->layout())
			p->activateLayout();
	}
}


PageContainer::PageContainer(QWidget *parent) : QStackedWidget(parent)
{
	transition_widget = 0;
	block_transitions = false;
	prev_page = 0;
	dest_page = 0;
}

void PageContainer::installTransitionWidget(TransitionWidget *tr)
{
	if (transition_widget)
	{
		transition_widget->disconnect();
		transition_widget->deleteLater();
	}

	transition_widget = tr;
	if (!transition_widget)
		return;

	addWidget(transition_widget);
	connect(transition_widget, SIGNAL(endTransition()), SLOT(endTransition()));
}

void PageContainer::endTransition()
{
	setCurrentPage(dest_page);
	parent_window->showWindow();
	prev_page = 0;
	dest_page = 0;
}

void PageContainer::setCurrentPage(Page *p)
{
	qDebug() << "PageContainer::setCurrentPage on" << p;
	setCurrentWidget(p);
	emit currentPageChanged(p);
}

void PageContainer::showPage(Page *p)
{
	// the current page can be the same, but we have also to call the showWindow()
	// on the parent window because it can be hidden (for example when we move from
	// the VctCallWindow to the VctCallPage).
	if (currentPage() != p)
	{
		emit currentChanging(p);
		if (Page *curr = currentPage())
		{
			curr->aboutToHideEvent();
#ifdef LAYOUT_TS_10
			// We want to close the call if some events (like a new scs message
			// or an alarm) trigger a showPage.
			if (bt_global::status.vde_call_active)
			{
				if (VCTCallPage *p = qobject_cast<VCTCallPage*>(curr))
					p->closeCall();
				else if (IntercomCallPage *p = qobject_cast<IntercomCallPage*>(curr))
					p->closeCall();
			}
#endif
		}

		if (transition_widget && !block_transitions)
		{
			prepareTransition();
			setCurrentWidget(transition_widget);

			fixVisualization(p, size());
			startTransition(p);
			return;
		}
		else
			setCurrentPage(p);
	}

	// This makes showPage() always show the HomeWindow as a side effect.
	// If this is removed, the code in vctcall.cpp, slideshow.cpp, pagestack.cpp
	// videoplayer.cpp must be changed to show the HomeWindow.
	parent_window->showWindow();
}

void PageContainer::prepareTransition()
{
	if (transition_widget && !block_transitions)
	{
		prev_page = currentPage();
		transition_widget->prepareTransition(QPixmap::grabWidget(prev_page));

	}
}

void PageContainer::startTransition(Page *p)
{
	if (transition_widget && !block_transitions)
	{
		dest_page = p;
		transition_widget->startTransition(QPixmap::grabWidget(dest_page));
	}
}

Page *PageContainer::currentPage()
{
	// if we are in the middle of a transition, we use the previous page as the current page
	return prev_page ? prev_page : qobject_cast<Page*>(currentWidget());
}

void PageContainer::blockTransitions(bool block)
{
	block_transitions = block;
	if (block && transition_widget)
	{
		transition_widget->cancelTransition();
		if (prev_page)
			setCurrentWidget(prev_page);
	}
}

void PageContainer::addPage(Page *p)
{
	addWidget(p);
	p->resize(width(), height());
}

void PageContainer::setContainerWindow(Window *window)
{
	parent_window = window;
}
