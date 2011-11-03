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
#include <QLabel>


namespace
{
	void fixVisualization(Page *p, QSize size)
	{
		p->resize(size);

		if (p->layout())
			p->activateLayout();
	}
}


// If the transition widget is a top-level window, for some reason, it's not shown
// directly after calling show()/raise(), and there is a small but visible flicker
//
// If a page is logically visible (isVisible() == true) during a transition, even if the
// page is completely covered by the transition widget, the transition is slower (it _might_
// be a missing optimization for overlapping siblings)
//
// The code below works around both problems:
// - an intermediate widget (QLabel) is shown at full screen
//   and it displays the parts of the screen not covered by the transition
// - the transition widget becomes a child of the background QLabel
// - the page container is hidden at the start of a transition and shown again
//   at the end of the transition
//
// The added constraint is that the background QLabel must be a sibling of
// the PageContainer; the same schema will not work for TS 10 (because the
// WindowContainer has no parent)

TransitionManager::TransitionManager(QWidget *page_container) : QObject(page_container)
{
	background_widget = new QLabel();
	transition_widget = 0;
	block_transitions = false;
}

void TransitionManager::setTransitionWidget(TransitionWidget *tr)
{
	Q_ASSERT_X(pageContainer()->parentWidget(), "TransitionManager::setTransitionWidget",
		   "PageContainer must have a parent before setting the transition widget");

	if (transition_widget)
	{
		transition_widget->disconnect();
		transition_widget->deleteLater();
	}
	transition_widget = tr;
	if (!transition_widget)
		return;

	background_widget->setParent(pageContainer()->parentWidget());
	transition_widget->setParent(background_widget);
	transition_widget->show();
	connect(transition_widget, SIGNAL(endTransition()), SLOT(endTransition()));
}

bool TransitionManager::isActive() const
{
	return transition_widget != 0 && !block_transitions;
}

void TransitionManager::prepareTransition(QWidget *prev)
{
	QPixmap full_pixmap = QPixmap::grabWidget(pageContainer());
	QPoint pos = prev->mapTo(pageContainer(), QPoint(0, 0));

	transition_widget->prepareTransition(full_pixmap.copy(QRect(pos, prev->size())));
	transition_widget->move(pos);
	transition_widget->resize(prev->size());

	background_widget->setPixmap(full_pixmap);
	background_widget->setGeometry(pageContainer()->geometry());
	background_widget->show();
	background_widget->raise();
	pageContainer()->hide();
}

void TransitionManager::startTransition(QWidget *next)
{
	QPoint pos = next->mapTo(pageContainer(), QPoint(0, 0));
	// calling grabWidget(next) does not paint background correctly if the background
	// color is defined on the parent widget, hence we grab a sub-rect of the page container
	QPixmap pixmap = QPixmap::grabWidget(pageContainer(), QRect(pos, next->size()));

	transition_widget->startTransition(pixmap);
}

void TransitionManager::endTransition()
{
	pageContainer()->show();
	background_widget->hide();
}

void TransitionManager::blockTransitions(bool block)
{
	block_transitions = block;
	if (block && transition_widget)
	{
		transition_widget->cancelTransition();
		// show again page container if there is a transition in progress
		if (background_widget->isVisible())
			endTransition();
	}
}

QWidget *TransitionManager::pageContainer()
{
	return qobject_cast<QWidget*>(parent());
}


PageContainer::PageContainer(QWidget *parent) : QStackedWidget(parent)
{
	transition_manager = new TransitionManager(this);
}

void PageContainer::installTransitionWidget(TransitionWidget *tr)
{
	transition_manager->setTransitionWidget(tr);
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

		if (transition_manager->isActive())
		{
			prepareTransition(currentPage());
			fixVisualization(p, size());
			setCurrentPage(p);
			startTransition(p);
		}
		else
			setCurrentPage(p);
	}

	// This makes showPage() always show the HomeWindow as a side effect.
	// If this is removed, the code in vctcall.cpp, slideshow.cpp, pagestack.cpp
	// videoplayer.cpp must be changed to show the HomeWindow.
	parent_window->showWindow();
}

void PageContainer::prepareTransition(QWidget *w)
{
	if (transition_manager->isActive())
		transition_manager->prepareTransition(w);
}

void PageContainer::startTransition(QWidget *w)
{
	if (transition_manager->isActive())
		transition_manager->startTransition(w);
}

void PageContainer::blockTransitions(bool block)
{
	transition_manager->blockTransitions(block);
}

Page *PageContainer::currentPage()
{
	return qobject_cast<Page*>(currentWidget());
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
