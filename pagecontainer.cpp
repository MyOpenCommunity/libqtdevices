#include "pagecontainer.h"
#include "transitionwidget.h"
#include "sottomenu.h"
#include "page.h"
#include "window.h"

#include <QDebug>
#include <QLayout>


namespace
{
	void fixVisualization(Page *p, QSize size)
	{
		p->resize(size);

		if (p->layout())
			p->activateLayout();
		else if (sottoMenu *s = qobject_cast<sottoMenu*>(p))
			s->forceDraw();
	}
}


PageContainer::PageContainer(QWidget *parent) : QStackedWidget(parent)
{
	transition_widget = 0;
	block_transitions = false;
	prev_page = 0;
	dest_page = 0;

	// TODO: this ugly workaround is needed because the QStackedWidget in some ways
	// invalidate the first widget inserted. FIX it asap!
	addWidget(new QWidget);
}

void PageContainer::installTransitionWidget(TransitionWidget *tr)
{
	transition_widget = tr;
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
	if (transition_widget && !block_transitions)
	{
		prev_page = currentPage();

		transition_widget->prepareTransition();

		// Before grab the screenshot of the next page, we have to ensure that its
		// visualization is correct and that it is shown.
		setCurrentWidget(p);
		// fixVisualization must be after setCurrentWidget, because setCurrentWidget
		// might cause the window header to change size, and fixVisualization must
		// run with the correct page size
		fixVisualization(p, size());
		startTransition(p);
	}
	else
		setCurrentPage(p);

	// This makes showPage() always show the HomeWindow as a side effect.
	// If this is removed, the code in vctcall.cpp, slideshow.cpp and
	// videoplayer.cpp must be changed to show the HomeWindow.
	parent_window->showWindow();
}

void PageContainer::prepareTransition()
{
	if (transition_widget && !block_transitions)
		transition_widget->prepareTransition();
}

void PageContainer::startTransition(Page *p)
{
	if (transition_widget)
	{
		dest_page = p;
		transition_widget->startTransition();
	}
}

Page *PageContainer::currentPage()
{
	// if we are in the middle of a transition, we use the previous page as the current page
	return prev_page ? prev_page : static_cast<Page*>(currentWidget());
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
