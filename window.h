#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class WindowContainer;


class Window : public QWidget
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

