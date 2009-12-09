#ifndef ICONWINDOW_H
#define ICONWINDOW_H

#include "window.h"

// Window to display a centered icon at full screen (for loading/wait pages)
class IconWindow : public Window
{
Q_OBJECT
public:
	IconWindow(const QString &icon, const QString &style = QString());
};

#endif // ICONWINDOW_H
