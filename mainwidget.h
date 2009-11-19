#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "page.h" // Page, PageType

#include <QStackedWidget>
#include <QWidget>

class QLabel;
class QVBoxLayout;
class QDomNode;


class HomeBar : public QWidget
{
Q_OBJECT
public:
	HomeBar(const QDomNode &config_node);

signals:
	void showHomePage();

protected:
	void paintEvent(QPaintEvent *);

private:
	void loadItems(const QDomNode &config_node);
};


class FavoritesWidget : public QWidget
{
Q_OBJECT
public:
	FavoritesWidget();
protected:
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
};


class HeaderWidget : public QWidget
{
Q_OBJECT
public:
	HeaderWidget(const QDomNode &config_node);
	void centralPageChanged(Page::PageType);

signals:
	void showHomePage();
	void showSectionPage(int page_id);

private:
	QVBoxLayout *main_layout;
	QLabel *header_bar;
	QLabel *top_nav_bar;
	QWidget *home_bar;
};


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
