#ifndef WINDOW_H
#define WINDOW_H

#include "styledwidget.h"

class WindowContainer;


class Window : public StyledWidget
{
Q_OBJECT
public:
	Window();

	static void setWindowContainer(WindowContainer *container);

public slots:
	virtual void showWindow();

private:
	static WindowContainer *window_container;
};

#endif // WINDOW_H

