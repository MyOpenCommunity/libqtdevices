#include "windowcontainer.h"
#include "homewindow.h"
#include "page.h" // Page::setPageContainer
#include "window.h" // Window::setWindowContainer
#include "transitionwidget.h"

#include <QStackedLayout>


WindowContainer::WindowContainer(int width, int height)
	: transition_widget(0)
{
	// needs to be done before HomeWindow is constructed
	Window::setWindowContainer(this);

	main = new HomeWindow;
	addWidget(main);
	QStackedLayout *root_layout = static_cast<QStackedLayout *>(layout());
	root_layout->setSpacing(0);
	root_layout->setContentsMargins(0, 0, 0, 0);

	showFullScreen();
	setFixedSize(width, height);

	Page::setPageContainer(main->centralLayout());
}

void WindowContainer::addWindow(Window *w)
{
	addWidget(w);
	w->resize(width(), height());
}

void WindowContainer::showWindow(Window *w)
{
	setCurrentWidget(w);
}

PageContainer *WindowContainer::centralLayout()
{
	return main->centralLayout();
}

HomeWindow *WindowContainer::homeWindow()
{
	return main;
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
	// TODO will need to replicate the current_page logic in btmain.cpp
	connect(transition_widget, SIGNAL(endTransition()), main, SLOT(showWindow()));
}

QPixmap WindowContainer::grabHomeWindow()
{
	return QPixmap::grabWidget(main);
}
