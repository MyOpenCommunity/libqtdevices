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

