#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QWidget>

class PageContainer;
class HeaderWidget;
class FavoritesWidget;

// controls the layout of "normal" pages; contains the stack of pages and th
// header and favorites widgets
class HomeWindow : public QWidget
{
Q_OBJECT
public:
	HomeWindow();
	PageContainer *centralLayout();

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
