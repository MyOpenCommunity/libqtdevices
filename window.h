#ifndef WINDOW_H
#define WINDOW_H

#include "styledwidget.h"

class WindowContainer;


class Window : public StyledWidget
{
Q_OBJECT
public:
	Window();

	virtual void showWindow();

	static void setWindowContainer(WindowContainer *container);

private:
	static WindowContainer *window_container;
};

#endif // WINDOW_H

