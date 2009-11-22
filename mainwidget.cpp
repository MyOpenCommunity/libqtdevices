#include "mainwidget.h"
#include "main_window.h"
#include "page.h"
#include "hardware_functions.h" // hardwareType()
#include "xml_functions.h"
#include "skinmanager.h"
#include "main.h" // getHomepageNode
#include "btbutton.h"
#include "settings_touchx.h"
#include "generic_functions.h" // getBostikName
#include "content_widget.h"

#include <QDebug>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedLayout>
#include <QSignalMapper>
#include <QVariant>
#include <QPainter>
#include <QTime>


enum
{
	ITEM_TIME = 987,
	ITEM_DATE = 988,
	ITEM_TEMPERATURE = 989,
	ITEM_SETTINGS_LINK = 29
};


class TimeDisplay : public QLabel
{
public:
	TimeDisplay();
	~TimeDisplay();

protected:
	void timerEvent(QTimerEvent *e);
	void paintEvent(QPaintEvent *e);

private:
	int timer_id;
};

TimeDisplay::TimeDisplay()
{
	setPixmap(bt_global::skin->getImage("clock_background"));
	timer_id = startTimer(7000);
}

TimeDisplay::~TimeDisplay()
{
	killTimer(timer_id);
}

void TimeDisplay::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);
	QPainter p(this);

	p.drawText(rect(), Qt::AlignCenter, QTime::currentTime().toString("hh:mm"));
}

void TimeDisplay::timerEvent(QTimerEvent *e)
{
	update();
}


HeaderBar::HeaderBar()
{
	setFixedSize(800, 40);

	time_display = new TimeDisplay;

	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(3, 0, 3, 0);
	l->setSpacing(0);

	l->addWidget(time_display);
	l->addStretch(1);
}

void HeaderBar::setControlsVisible(bool visible)
{
	time_display->setVisible(visible);
}


HomeBar::HomeBar(const QDomNode &config_node)
{
	setFixedSize(800, 105);

	loadItems(config_node);
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


TopNavigationBar::TopNavigationBar(const QDomNode &config_node)
{
	SkinContext cxt(0);
	setFixedSize(800, 85);

	loadItems(config_node);
}

void TopNavigationBar::loadItems(const QDomNode &config_node)
{
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->setContentsMargins(10, 0, 0, 0);
	main_layout->setSpacing(0);

	BtButton *home = new BtButton;
	home->setImage(bt_global::skin->getImage("go_home"));
	main_layout->addWidget(home);
	connect(home, SIGNAL(clicked()), SIGNAL(showHomePage()));

	navigation = new TopNavigationWidget;
	main_layout->addWidget(navigation, 1);
	connect(navigation, SIGNAL(pageSelected(int)), SIGNAL(showSectionPage(int)));

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		int page_id = getTextChild(item, "lnk_pageID").toInt();
		QDomNode page_node = getPageNodeFromPageId(page_id);
		int id = getTextChild(page_node, "id").toInt();

		navigation->addButton(Page::SectionId(id), page_id, bt_global::skin->getImage("top_navigation_button"));
	}
}

void TopNavigationBar::setCurrentSection(Page::SectionId section_id)
{
	navigation->setCurrentSection(section_id);
}


TopNavigationWidget::TopNavigationWidget()
	:
	current_index(0),
	selected_section_id(Page::NO_SECTION),
	need_update(true)
{
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->setContentsMargins(15, 0, 10, 0);
	main_layout->setSpacing(5);

	button_layout = new QHBoxLayout;
	button_layout->setContentsMargins(0, 13, 0, 12);

	left = new BtButton;
	left->setAutoRepeat(true);
	left->setImage(bt_global::skin->getImage("left"));

	right = new BtButton;
	right->setAutoRepeat(true);
	right->setImage(bt_global::skin->getImage("right"));

	main_layout->addWidget(left, 0, Qt::AlignVCenter);
	main_layout->addLayout(button_layout, 1);
	main_layout->addWidget(right, 0, Qt::AlignVCenter);

	mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(int)), SIGNAL(pageSelected(int)));
}

void TopNavigationWidget::showEvent(QShowEvent *e)
{
	drawContent();
	QWidget::showEvent(e);
}

void TopNavigationWidget::addButton(Page::SectionId section_id, int page_id, const QString &icon)
{
	BtButton *link = new BtButton;
	link->setImage(icon);
	buttons.append(link);

	mapper->setMapping(link, page_id);
	connect(link, SIGNAL(clicked()), mapper, SLOT(map()));

	QLabel *active = new QLabel;
	active->setPixmap(getBostikName(icon, "s"));
	selected.append(active);

	section_ids.append(section_id);

	need_update = true;
}

void TopNavigationWidget::drawContent()
{
	if (!need_update)
		return;

	need_update = false;

	while (QLayoutItem *child = button_layout->takeAt(0))
		if (QWidget *w = child->widget())
			w->hide();

	button_layout->addStretch(1);

	left->setVisible(buttons.size() > 10);
	right->setVisible(buttons.size() > 10);

	int max = buttons.size() > 10 ? 9 : buttons.size();
	for (int i = 0; i < max; ++i)
	{
		int index = (current_index + i) % buttons.size();
		QWidget *item;
		if (section_ids[index] == selected_section_id)
			item = selected[index];
		else
			 item = buttons[index];
		item->show();
		button_layout->addWidget(item);
	}

	button_layout->addStretch(1);
}

void TopNavigationWidget::setCurrentSection(Page::SectionId section_id)
{
	if (section_id == Page::NO_SECTION || section_id == selected_section_id)
		return;

	selected_section_id = section_id;
	need_update = true;
	drawContent();
}


HeaderWidget::HeaderWidget(const QDomNode &homepage_node, const QDomNode &infobar_node)
{
	main_layout = new QVBoxLayout(this);
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);

	header_bar = new HeaderBar;
	main_layout->addWidget(header_bar);

	top_nav_bar = new TopNavigationBar(homepage_node);
	top_nav_bar->hide();
	main_layout->addWidget(top_nav_bar);

	connect(top_nav_bar, SIGNAL(showSectionPage(int)), SIGNAL(showSectionPage(int)));
	connect(top_nav_bar, SIGNAL(showHomePage()), SIGNAL(showHomePage()));

	home_bar = new HomeBar(infobar_node);
	main_layout->addWidget(home_bar);

	connect(home_bar, SIGNAL(showSectionPage(int)), SIGNAL(showSectionPage(int)));
	connect(home_bar, SIGNAL(showHomePage()), SIGNAL(showHomePage()));
}

void HeaderWidget::centralPageChanged(Page::SectionId section_id, Page::PageType type)
{
	qDebug() << "new central widget = " << type << " id " << section_id;
	switch (type)
	{
	case Page::HOMEPAGE:
		header_bar->setControlsVisible(false);
		top_nav_bar->hide();
		header_bar->show();
		home_bar->show();
		break;
	default:
		header_bar->setControlsVisible(true);
		top_nav_bar->setCurrentSection(section_id);
		top_nav_bar->show();
		header_bar->show();
		home_bar->hide();
		break;
	}
}


FavoritesWidget::FavoritesWidget(const QDomNode &config_node)
{
	QVBoxLayout *l = new QVBoxLayout(this);
	QLabel *title = new QLabel(getTextChild(config_node, "descr"));

	title->setAlignment(Qt::AlignHCenter);

	QWidget *p = new QWidget;

	// TODO add banners from configuration

	l->addWidget(title);
	l->addWidget(p, 1);
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
		SkinContext cxt(99);

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

		header_widget = new HeaderWidget(pagemenu_home, getPageNodeFromPageId(info_bar_pageid));
		main_layout->addWidget(header_widget, 0, 0, 1, 2);

		favorites_widget = new FavoritesWidget(favourites_node);

		// container widget for favorites
		QWidget *f = new QWidget;
		f->setProperty("Favorites", true); // for stylesheet
		QVBoxLayout *l = new QVBoxLayout(f);
		l->setContentsMargins(0, 0, 0, 0);
		l->setSpacing(0);

		l->addStretch(1);
		l->addWidget(favorites_widget, 1);

		main_layout->addWidget(f, 1, 1, 1, 1);

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
		header_widget->centralPageChanged(p->sectionId(), p->pageType());
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
