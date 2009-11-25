#include "windowcontainer.h"
#include "homewindow.h"
#include "page.h" // Page::setPageContainer
#include "window.h" // Window::setWindowContainer

#include <QStackedLayout>


WindowContainer::WindowContainer(int width, int height)
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
