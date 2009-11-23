#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "page.h" // Page, PageType, StyledWidget

#include <QStackedWidget>
#include <QWidget>
#include <QList>

class QLabel;
class QVBoxLayout;
class QDomNode;
class QHBoxLayout;
class BtButton;
class TopNavigationWidget;
class QSignalMapper;


// topmost header, contains the BTicino logo and (on internal pages) the
// clock and temperature display
class HeaderBar : public StyledWidget
{
Q_OBJECT
public:
	HeaderBar();

	void setControlsVisible(bool visible);

private:
	QWidget *time_display;
};


// only displayed on the home page, contains date, time and temperature
// display, a link to the settings page and links to multimedia contents
// (RSS feeds, web radio, web tv, web cams)
class HomeBar : public StyledWidget
{
Q_OBJECT
public:
	HomeBar(const QDomNode &config_node);

signals:
	void showHomePage();

private:
	void loadItems(const QDomNode &config_node);
};


// navigation bar shown in internal pages; contains a link to the home page
// and a scrollable list of links to section pages
class TopNavigationBar : public StyledWidget
{
Q_OBJECT
public:
	TopNavigationBar(const QDomNode &config_node);

	void setCurrentSection(int section_id);

signals:
	void showHomePage();
	void showSectionPage(int page_id);

private:
	void loadItems(const QDomNode &config_node);

private:
	TopNavigationWidget *navigation;
};


// helper class for TopNavigationBar, contains the scrollable list of links to
// internal pages
class TopNavigationWidget : public QWidget
{
Q_OBJECT
public:
	TopNavigationWidget();

	void addButton(int section_id, int page_id, const QString &icon);
	void setCurrentSection(int section_id);

public slots:
	void scrollLeft();
	void scrollRight();

signals:
	void pageSelected(int page_id);

protected:
	void showEvent(QShowEvent *e);

private:
	void drawContent();

private:
	int current_index, selected_section_id, visible_buttons;
	bool need_update;
	QList<int> section_ids;
	QList<QWidget *> buttons, selected;
	QHBoxLayout *button_layout;
	BtButton *left, *right;
	QSignalMapper *mapper;
};


// favorites widges, shown on the right, contains a list of banners
class FavoritesWidget : public QWidget
{
Q_OBJECT
public:
	FavoritesWidget(const QDomNode &config_node);

protected:
	virtual QSize minimumSizeHint() const;
};


// contains all the header widgets and shows/hides them as the central page changes
class HeaderWidget : public QWidget
{
Q_OBJECT
public:
	HeaderWidget(const QDomNode &homepage_node, const QDomNode &infobar_node);
	void centralPageChanged(int section_id, Page::PageType);

signals:
	void showHomePage();
	void showSectionPage(int page_id);

private:
	QVBoxLayout *main_layout;
	HeaderBar *header_bar;
	TopNavigationBar *top_nav_bar;
	QWidget *home_bar;
};


// controls the layout of "normal" pages; contains the stack of pages and th
// header and favorites widgets
class MainWidget : public QWidget
{
Q_OBJECT
public:
	MainWidget();
	MainWindow *centralLayout();

signals:
	void showHomePage();
	void showSectionPage(int page_id);

public slots:
	void centralWidgetChanged(int index);

private:
	MainWindow *central_widget;
	HeaderWidget *header_widget;
	FavoritesWidget *favorites_widget;
};


// top level widget, contains the MainWidget and other special widgets that
// need to be shown full screen (for example the screen saver and transition widgets)
class RootWidget : public QStackedWidget
{
Q_OBJECT
public:
	RootWidget(int width, int height);
	MainWindow *centralLayout();
	MainWidget *mainWidget();

private:
	MainWidget *main;
};

#endif // MAINWIDGET_H
