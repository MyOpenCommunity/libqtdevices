#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "page.h" // Page, PageType

#include <QStackedWidget>
#include <QWidget>

class QLabel;
class QVBoxLayout;


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
	HeaderWidget();
	void centralPageChanged(Page::PageType);

private:
	QVBoxLayout *main_layout;
	QLabel *header_bar;
	QLabel *top_nav_bar;
	QLabel *info_bar;
	QLabel *home_bar;
};

class MainWidget : public QWidget
{
Q_OBJECT
public:
	MainWidget();
	MainWindow *centralLayout();

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

private:
	MainWidget *main;
};

#endif // MAINWIDGET_H
