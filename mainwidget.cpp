#include "mainwidget.h"
#include "main_window.h"
#include "page.h"
#include "hardware_functions.h" // hardwareType()

#include <QDebug>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedLayout>

HeaderWidget::HeaderWidget()
{
	setStyleSheet("QWidget {background-color:gray; }");
	main_layout = new QVBoxLayout(this);
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);

	header_bar = new QLabel;
	header_bar->setFixedSize(800, 40);
	header_bar->setText("Header bar");
	main_layout->addWidget(header_bar);

	top_nav_bar = new QLabel;
	top_nav_bar->setFixedSize(800, 85);
	top_nav_bar->setText("Top nav bar");
	top_nav_bar->hide();
	main_layout->addWidget(top_nav_bar);

	home_bar = new QLabel;
	home_bar->setFixedSize(800, 50);
	home_bar->setText("Home bar");
	main_layout->addWidget(home_bar);

	info_bar = new QLabel;
	info_bar->setFixedSize(800, 55);
	info_bar->setText("Info bar");
	main_layout->addWidget(info_bar);
}

void HeaderWidget::centralPageChanged(Page::PageType type)
{
	qDebug() << "new central widget = " << type;
	switch (type)
	{
	case Page::HOMEPAGE:
		top_nav_bar->hide();
		header_bar->show();
		home_bar->show();
		info_bar->show();
		break;
	default:
		top_nav_bar->show();
		header_bar->show();
		home_bar->hide();
		info_bar->hide();
		break;
	}
}


FavoritesWidget::FavoritesWidget()
{
	setStyleSheet("QWidget {background-color:blue; }");
	QVBoxLayout *lay = new QVBoxLayout(this);
	QLabel *l = new QLabel("Favorites widget", this);
	lay->addWidget(l);
}

QSize FavoritesWidget::sizeHint() const
{
	return QSize(237, 335);
}

QSize FavoritesWidget::minimumSizeHint() const
{
	return QSize(237, 335);
}


MainWidget::MainWidget()
{
	header_widget = 0;
	favorites_widget = 0;
	central_widget = 0;

	if (hardwareType() == TOUCH_X)
	{
		QGridLayout *main_layout = new QGridLayout(this);
		main_layout->setSpacing(0);
		main_layout->setContentsMargins(0, 0, 0, 0);
		qDebug() << "adding top and right widgets";

		central_widget = new MainWindow(this);
		main_layout->addWidget(central_widget, 1, 0, 1, 1);
		connect(central_widget, SIGNAL(currentChanged(int)), SLOT(centralWidgetChanged(int)));
		Page::setMainWindow(central_widget);

		header_widget = new HeaderWidget(getPageNodeFromPageId(info_bar_pageid));
		main_layout->addWidget(header_widget, 0, 0, 1, 2);

		favorites_widget = new FavoritesWidget;
		main_layout->addWidget(favorites_widget, 1, 1, 1, 1);
	}
	else
	{
		central_widget = new MainWindow(this);
		Page::setMainWindow(central_widget);
		central_widget->setFixedSize(maxWidth(), maxHeight());
	}
}

void MainWidget::centralWidgetChanged(int index)
{
	// the check on header_widget shouldn't fail, but I don't want to rely on this...
	if (header_widget && qobject_cast<Page *>(central_widget->widget(index)))
	{
		Page *p = static_cast<Page *>(central_widget->widget(index));
		header_widget->centralPageChanged(p->pageType());
	}
}


MainWindow *MainWidget::centralLayout()
{
	return central_widget;
}


RootWidget::RootWidget(int width, int height)
{
	main = new MainWidget;
	addWidget(main);
	QStackedLayout *root_layout = static_cast<QStackedLayout *>(layout());
	root_layout->setSpacing(0);
	root_layout->setContentsMargins(0, 0, 0, 0);

	showFullScreen();
	setFixedSize(width, height);

	Page::setMainWindow(main->centralLayout());
}

MainWindow *RootWidget::centralLayout()
{
	return main->centralLayout();
}
