#include "mainwidget.h"
#include "main_window.h"
#include "page.h"
#include "hardware_functions.h" // hardwareType()
#include "xml_functions.h"
#include "skinmanager.h"
#include "main.h" // getHomepageNode
#include "btbutton.h"
#include "settings_touchx.h"

#include <QDebug>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedLayout>
#include <QStyleOption>
#include <QPainter>


enum
{
	ITEM_TIME = 987,
	ITEM_DATE = 988,
	ITEM_TEMPERATURE = 989,
	ITEM_SETTINGS_LINK = 29
};


HomeBar::HomeBar(const QDomNode &config_node)
{
	SkinContext cxt(99);
	setFixedSize(800, 105);

	loadItems(config_node);
}

void HomeBar::paintEvent(QPaintEvent *)
{
	// required for Style Sheets on a QWidget subclass
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void HomeBar::loadItems(const QDomNode &config_node)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(50, 0, 63, 0);
	main_layout->setSpacing(0);

	QHBoxLayout *home_layout = new QHBoxLayout;
	home_layout->setContentsMargins(0, 5, 0, 5);
	home_layout->setSpacing(0);

	QHBoxLayout *info_layout = new QHBoxLayout;
	info_layout->setContentsMargins(0, 7, 0, 8);
	info_layout->setSpacing(0);

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		int id = getTextChild(item, "id").toInt();

		switch (id)
		{
		case ITEM_SETTINGS_LINK:
		{
			BtButton *button = new BtButton(this);
			button->setImage(bt_global::skin->getImage("link_icon"));
			home_layout->addStretch(1);
			home_layout->addWidget(button);

			int page_id = getTextChild(item, "lnk_pageID").toInt();

			Page *settings = new SettingsTouchX(getPageNodeFromPageId(page_id));
			connect(button, SIGNAL(clicked()), settings, SLOT(showPage()));
			connect(settings, SIGNAL(Closed()), SIGNAL(showHomePage()));

			break;
		}
		default:
			qWarning("Ignoring item in info bar");
		}
	}

	main_layout->addLayout(home_layout);
	if (info_layout->count() > 0)
		main_layout->addLayout(info_layout);
	else
	{
		// use a spacer if there are no items in the infobar layout
		delete info_layout;
		main_layout->addSpacing(55);
	}
}


HeaderWidget::HeaderWidget(const QDomNode &config_node)
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

	home_bar = new HomeBar(config_node);
	main_layout->addWidget(home_bar);

	connect(home_bar, SIGNAL(showSectionPage(int)), SIGNAL(showSectionPage(int)));
	connect(home_bar, SIGNAL(showHomePage()), SIGNAL(showHomePage()));
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
		break;
	default:
		top_nav_bar->show();
		header_bar->show();
		home_bar->hide();
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
		QDomNode pagemenu_home = getHomepageNode();
		int favourites_pageid = getTextChild(pagemenu_home, "h_lnk_pageID").toInt();
		QDomNode favourites_node = getPageNodeFromPageId(favourites_pageid);
		int info_bar_pageid = getTextChild(favourites_node, "h_lnk_pageID").toInt();

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

		connect(header_widget, SIGNAL(showSectionPage(int)), SIGNAL(showSectionPage(int)));
		connect(header_widget, SIGNAL(showHomePage()), SIGNAL(showHomePage()));
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

MainWidget *RootWidget::mainWidget()
{
	return main;
}
