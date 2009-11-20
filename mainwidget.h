#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "page.h" // Page, PageType

#include <QStackedWidget>
#include <QWidget>
#include <QList>

class QLabel;
class QVBoxLayout;
class QDomNode;
class QHBoxLayout;
class BtButton;
class TopNavigationWidget;


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


class TopNavigationBar : public QWidget
{
Q_OBJECT
public:
	TopNavigationBar(const QDomNode &config_node);

	void setCurrentSection(int page_id);

signals:
	void showHomePage();
	void showSectionPage(int page_id);

protected:
	void paintEvent(QPaintEvent *);

private:
	void loadItems(const QDomNode &config_node);

private:
	TopNavigationWidget *navigation;
};


class TopNavigationWidget : public QWidget
{
Q_OBJECT
public:
	TopNavigationWidget();

	void addButton(int page_id, const QString &icon);
	void setCurrentSection(int page_id);

signals:
	void pageSelected(int page_id);

protected:
	void showEvent(QShowEvent *e);

private:
	void drawContent();

private:
	int current_index, selected_page_id;
	bool need_update;
	QList<int> page_ids;
	QList<QWidget *> buttons, selected;
	QHBoxLayout *button_layout;
	BtButton *left, *right;
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
	HeaderWidget(const QDomNode &homepage_node, const QDomNode &infobar_node);
	void centralPageChanged(int page_id, Page::PageType);

signals:
	void showHomePage();
	void showSectionPage(int page_id);

private:
	QVBoxLayout *main_layout;
	QLabel *header_bar;
	TopNavigationBar *top_nav_bar;
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
