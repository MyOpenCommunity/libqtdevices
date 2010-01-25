#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include "window.h"

#include <QWidget>

class PageContainer;
class HeaderWidget;
class FavoritesWidget;
class Page;
class BtButton;


class TrayBar : public QWidget
{
Q_OBJECT
public:
	TrayBar();
	void addButton(BtButton *b);
};


// controls the layout of "normal" pages; contains the stack of pages and the
// header and favorites widgets
class HomeWindow : public Window
{
Q_OBJECT
public:
	HomeWindow();
	PageContainer *centralLayout();
	Page *currentPage();

	void loadConfiguration();
	TrayBar *tray_bar;

signals:
	void showHomePage();
	void showSectionPage(int page_id);

public slots:
	void centralWidgetChanged(int index);

private:
	PageContainer *central_widget;
	HeaderWidget *header_widget;
	FavoritesWidget *favorites_widget;
};


#endif // HOMEWINDOW_H
