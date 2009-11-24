#ifndef WINDOWCONTAINER_H
#define WINDOWCONTAINER_H

#include <QStackedWidget>

class PageContainer;
class HomeWindow;


// top level widget, contains the HomeWindow and other special widgets that
// need to be shown full screen (for example the screen saver and transition widgets)
class WindowContainer : public QStackedWidget
{
Q_OBJECT
public:
	WindowContainer(int width, int height);
	PageContainer *centralLayout();
	HomeWindow *homeWindow();

private:
	HomeWindow *main;
};

#endif // WINDOWCONTAINER_H
