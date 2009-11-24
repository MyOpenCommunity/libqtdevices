#include "headerwidget.h"
#include "fontmanager.h"
#include "skinmanager.h"
#include "main.h" // bt_global::config
#include "xml_functions.h"
#include "btbutton.h"
#include "iconsettings.h"
#include "generic_functions.h" // getBostikName

#include <QSignalMapper>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QTime>
#include <QtDebug>


enum
{
	ITEM_TIME = 987,
	ITEM_DATE = 988,
	ITEM_TEMPERATURE = 989,
	ITEM_SETTINGS_LINK = 29
};


// base class for date/time dsiplay widgets
class PollingDisplayWidget : public QLabel
{
public:
	PollingDisplayWidget();
	~PollingDisplayWidget();

protected:
	void timerEvent(QTimerEvent *e);
	void paintEvent(QPaintEvent *e);

	virtual void paintLabel(QPainter &painter) = 0;

private:
	int timer_id;
};

PollingDisplayWidget::PollingDisplayWidget()
{
	timer_id = startTimer(7000);
}

PollingDisplayWidget::~PollingDisplayWidget()
{
	killTimer(timer_id);
}

void PollingDisplayWidget::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);
	QPainter p(this);

	paintLabel(p);
}

void PollingDisplayWidget::timerEvent(QTimerEvent *e)
{
	update();
}


// helper widget, display time for internal pages
class TimeDisplay : public PollingDisplayWidget
{
public:
	TimeDisplay();

protected:
	virtual void paintLabel(QPainter &painter);
};

TimeDisplay::TimeDisplay()
{
	setFont(bt_global::font->get(FontManager::SMALLTEXT)); // TODO check this is the correct font
	setPixmap(bt_global::skin->getImage("clock_background"));
}

void TimeDisplay::paintLabel(QPainter &painter)
{
	painter.drawText(rect(), Qt::AlignCenter,
			 QTime::currentTime().toString("hh:mm"));
}


// helper widget, displays time in the homepage
class HomepageTimeDisplay : public PollingDisplayWidget
{
public:
	HomepageTimeDisplay();

protected:
	virtual void paintLabel(QPainter &painter);
};

HomepageTimeDisplay::HomepageTimeDisplay()
{
	setFont(bt_global::font->get(FontManager::SUBTITLE)); // TODO check this is the correct font
	setPixmap(bt_global::skin->getImage("background"));
}

void HomepageTimeDisplay::paintLabel(QPainter &painter)
{
	painter.drawText(rect().adjusted(15, 0, 0, 0), Qt::AlignCenter,
			 QTime::currentTime().toString("hh:mm"));
}


// helper widget, displays date in the homepage
class HomepageDateDisplay : public PollingDisplayWidget
{
public:
	HomepageDateDisplay();

protected:
	virtual void paintLabel(QPainter &painter);
};

HomepageDateDisplay::HomepageDateDisplay()
{
	setFont(bt_global::font->get(FontManager::SUBTITLE)); // TODO check this is the correct font
	setPixmap(bt_global::skin->getImage("background"));
}

void HomepageDateDisplay::paintLabel(QPainter &painter)
{
	QString format;
	if (bt_global::config[DATE_FORMAT].toInt() == USA_DATE)
		format = "MM/dd/yy";
	else
		format = "dd/MM/yy";

	painter.drawText(rect().adjusted(35, 0, 0, 0), Qt::AlignCenter,
			 QDate::currentDate().toString(format));
}


HeaderLogo::HeaderLogo()
{
	setFixedSize(800, 40);

	time_display = new TimeDisplay;

	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(3, 0, 3, 0);
	l->setSpacing(0);

	l->addWidget(time_display);
	l->addStretch(1);
}

void HeaderLogo::setControlsVisible(bool visible)
{
	time_display->setVisible(visible);
}


HeaderInfo::HeaderInfo(const QDomNode &config_node)
{
	setFixedSize(800, 105);

	loadItems(config_node);
}

void HeaderInfo::loadItems(const QDomNode &config_node)
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	main_layout->setContentsMargins(50, 0, 63, 0);
	main_layout->setSpacing(0);

	QHBoxLayout *home_layout = new QHBoxLayout;
	home_layout->setContentsMargins(0, 5, 0, 5);
	home_layout->setSpacing(10);

	QHBoxLayout *info_layout = new QHBoxLayout;
	info_layout->setContentsMargins(0, 7, 0, 8);
	info_layout->setSpacing(0);

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		int id = getTextChild(item, "id").toInt();

		switch (id)
		{
		case ITEM_TIME:
		{
			QWidget *item = new HomepageTimeDisplay();
			home_layout->addWidget(item);

			break;
		}
		case ITEM_DATE:
		{
			QWidget *item = new HomepageDateDisplay();
			home_layout->addWidget(item);

			break;
		}
		case ITEM_SETTINGS_LINK:
		{
			BtButton *button = new BtButton(this);
			button->setImage(bt_global::skin->getImage("link_icon"));
			home_layout->addStretch(1);
			home_layout->addWidget(button);

			int page_id = getTextChild(item, "lnk_pageID").toInt();

			Page *settings = new IconSettings(getPageNodeFromPageId(page_id));
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


HeaderNavigationBar::HeaderNavigationBar(const QDomNode &config_node)
{
	SkinContext cxt(0);
	setFixedSize(800, 85);

	loadItems(config_node);
}

void HeaderNavigationBar::loadItems(const QDomNode &config_node)
{
	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->setContentsMargins(10, 0, 0, 0);
	main_layout->setSpacing(0);

	BtButton *home = new BtButton;
	home->setImage(bt_global::skin->getImage("go_home"));
	main_layout->addWidget(home);
	connect(home, SIGNAL(clicked()), SIGNAL(showHomePage()));

	navigation = new HeaderNavigationWidget;
	main_layout->addWidget(navigation, 1);
	connect(navigation, SIGNAL(pageSelected(int)), SIGNAL(showSectionPage(int)));

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		int page_id = getTextChild(item, "lnk_pageID").toInt();
		QDomNode page_node = getPageNodeFromPageId(page_id);
		int id = getTextChild(page_node, "id").toInt();

		navigation->addButton(id, page_id, bt_global::skin->getImage("top_navigation_button"));
	}
}

void HeaderNavigationBar::setCurrentSection(int section_id)
{
	navigation->setCurrentSection(section_id);
}


HeaderNavigationWidget::HeaderNavigationWidget()
	: current_index(0),
	selected_section_id(NO_SECTION),
	visible_buttons(0),
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
	connect(left, SIGNAL(clicked()), SLOT(scrollLeft()));

	right = new BtButton;
	right->setAutoRepeat(true);
	right->setImage(bt_global::skin->getImage("right"));
	connect(right, SIGNAL(clicked()), SLOT(scrollRight()));

	main_layout->addWidget(left, 0, Qt::AlignVCenter);
	main_layout->addLayout(button_layout, 1);
	main_layout->addWidget(right, 0, Qt::AlignVCenter);

	mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(int)), SIGNAL(pageSelected(int)));
}

void HeaderNavigationWidget::showEvent(QShowEvent *e)
{
	drawContent();
	QWidget::showEvent(e);
}

void HeaderNavigationWidget::addButton(int section_id, int page_id, const QString &icon)
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

void HeaderNavigationWidget::drawContent()
{
	if (!need_update)
		return;

	need_update = false;

	while (QLayoutItem *child = button_layout->takeAt(0))
		if (QWidget *w = child->widget())
			w->hide();

	button_layout->addStretch(1);

	// first time, compute if there is need for scroll arrows
	if (visible_buttons == 0)
	{
		int button_width = buttons[0]->sizeHint().width();
		int button_spacing = button_layout->spacing();
		// total size of all the buttons plus the spacing (does not take into account contents margins
		int buttons_width = (buttons.size() - 1) * (button_width + button_spacing) + button_width;

		// if buttons_width > width(), show the scroll arrows
		left->setVisible(buttons_width > width());
		right->setVisible(buttons_width > width());

		if (buttons_width > width())
		{
			// width of the scroll buttons and contents margins
			int scroll_area_width = layout()->minimumSize().width();
			// number of visible buttons
			visible_buttons = (width() - scroll_area_width - button_width) / (button_width + button_spacing) + 1;
		}
		else
			visible_buttons = buttons.size();
	}

	for (int i = 0; i < visible_buttons; ++i)
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

void HeaderNavigationWidget::scrollRight()
{
	current_index = (current_index + 1) % buttons.size();
	need_update = true;
	drawContent();
}

void HeaderNavigationWidget::scrollLeft()
{
	current_index = (current_index - 1 + buttons.size()) % buttons.size();
	need_update = true;
	drawContent();
}

void HeaderNavigationWidget::setCurrentSection(int section_id)
{
	if (section_id == NO_SECTION || section_id == selected_section_id)
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

	header_logo = new HeaderLogo;
	main_layout->addWidget(header_logo);

	top_nav_bar = new HeaderNavigationBar(homepage_node);
	top_nav_bar->hide();
	main_layout->addWidget(top_nav_bar);

	connect(top_nav_bar, SIGNAL(showSectionPage(int)), SIGNAL(showSectionPage(int)));
	connect(top_nav_bar, SIGNAL(showHomePage()), SIGNAL(showHomePage()));

	header_info = new HeaderInfo(infobar_node);
	main_layout->addWidget(header_info);

	connect(header_info, SIGNAL(showSectionPage(int)), SIGNAL(showSectionPage(int)));
	connect(header_info, SIGNAL(showHomePage()), SIGNAL(showHomePage()));
}

void HeaderWidget::centralPageChanged(int section_id, Page::PageType type)
{
	qDebug() << "new central widget = " << type << " id " << section_id;
	switch (type)
	{
	case Page::HOMEPAGE:
		header_logo->setControlsVisible(false);
		top_nav_bar->hide();
		header_logo->show();
		header_info->show();
		break;
	default:
		header_logo->setControlsVisible(true);
		top_nav_bar->setCurrentSection(section_id);
		top_nav_bar->show();
		header_logo->show();
		header_info->hide();
		break;
	}
}
