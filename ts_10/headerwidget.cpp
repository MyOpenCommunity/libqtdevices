/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "headerwidget.h"
#include "fontmanager.h"
#include "skinmanager.h"
#include "main.h" // bt_global::config
#include "xml_functions.h"
#include "btbutton.h"
#include "settings_ts10.h"
#include "generic_functions.h" // getBostikName
#include "probe_device.h" // NonControlledProbeDevice
#include "scaleconversion.h"
#include "devices_cache.h"
#include "homewindow.h"
#include "feedmanager.h"
#include "audioplayer_ts10.h"
#include "webcam.h"

#include <QSignalMapper>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QTime>
#include <QtDebug>
#include <QUrl> // for webcam


enum
{
	ITEM_SETTINGS_LINK = 14,
	ITEM_RSS_LINK = 16151,
	ITEM_WEB_CAM_LINK = 16201,
	ITEM_WEB_RADIO_LINK = 16101,
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


InnerPageTimeDisplay::InnerPageTimeDisplay()
{
	setFont(bt_global::font->get(FontManager::TEXT));
	setPixmap(bt_global::skin->getImage("clock_background"));
}

void InnerPageTimeDisplay::paintLabel(QPainter &painter)
{
	painter.drawText(rect().adjusted(0, 0, -3, -3), Qt::AlignRight|Qt::AlignVCenter,
			 QTime::currentTime().toString("hh:mm"));
}


InnerPageDateDisplay::InnerPageDateDisplay()
{
	setFont(bt_global::font->get(FontManager::TEXT));
	setPixmap(bt_global::skin->getImage("date_background"));
}

void InnerPageDateDisplay::paintLabel(QPainter &painter)
{
	painter.drawText(rect().adjusted(0, 0, -3, -3), Qt::AlignRight|Qt::AlignVCenter,
			 DateConversions::formatDateConfig(QDate::currentDate(), '/'));
}


HomepageTimeDisplay::HomepageTimeDisplay()
{
	setFont(bt_global::font->get(FontManager::HOMEPAGEWIDGET));
	setPixmap(bt_global::skin->getImage("background"));
}

void HomepageTimeDisplay::paintLabel(QPainter &painter)
{
	painter.drawText(rect().adjusted(15, 0, 0, 0), Qt::AlignCenter,
			 QTime::currentTime().toString("hh:mm"));
}


HomepageDateDisplay::HomepageDateDisplay()
{
	setFont(bt_global::font->get(FontManager::HOMEPAGEWIDGET));
	setPixmap(bt_global::skin->getImage("background"));
}

void HomepageDateDisplay::paintLabel(QPainter &painter)
{
	painter.drawText(rect().adjusted(35, 0, 0, 0), Qt::AlignCenter,
			 DateConversions::formatDateConfig(QDate::currentDate(), '/'));
}


TemperatureDisplay::TemperatureDisplay(device *probe)
{
	temp_scale = static_cast<TemperatureScale>((*bt_global::config)[TEMPERATURE_SCALE].toInt());
	label = "-";

	connect(probe, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void TemperatureDisplay::valueReceived(const DeviceValues &values_list)
{
	if (!values_list.contains(NonControlledProbeDevice::DIM_TEMPERATURE))
		return;

	int temperature = values_list[NonControlledProbeDevice::DIM_TEMPERATURE].toInt();
	switch (temp_scale)
	{
		case CELSIUS:
			label = celsiusString(bt2Celsius(temperature));
			break;
		case FAHRENHEIT:
			label = fahrenheitString(bt2Fahrenheit(temperature));
			break;
		default:
			qWarning("TemperatureDisplay: unknown scale");
	}

	update();
}


HomepageTemperatureDisplay::HomepageTemperatureDisplay(device *probe)
	: TemperatureDisplay(probe)
{
	setFont(bt_global::font->get(FontManager::HOMEPAGEWIDGET));
	setPixmap(bt_global::skin->getImage("background"));
}

void HomepageTemperatureDisplay::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);
	QPainter p(this);

	p.drawText(rect().adjusted(35, 0, 0, 0),
		   Qt::AlignCenter, label);
}


InnerPageTemperatureDisplay::InnerPageTemperatureDisplay(device *probe)
	: TemperatureDisplay(probe)
{
	setFont(bt_global::font->get(FontManager::TEXT));
	setPixmap(bt_global::skin->getImage("temperature_background"));
}

void InnerPageTemperatureDisplay::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);
	QPainter p(this);

	p.drawText(rect().adjusted(0, 0, -3, -3), Qt::AlignRight|Qt::AlignVCenter, label);
}


HomepageLink::HomepageLink(const QString &description, const QString &icon)
{
	QBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(10);

	BtButton *button = new BtButton(icon);

	QLabel *label = new QLabel(description);
	label->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));
	label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

	l->addWidget(button);
	l->addWidget(label, 1);

	connect(button, SIGNAL(clicked()), SLOT(buttonClicked()));
}

void HomepageLink::buttonClicked()
{
	emit aboutToClick();
	emit clicked();
}

HomepageFeedLink::HomepageFeedLink(const QString &description, const QString &feed) :
	HomepageLink(description, bt_global::skin->getImage("link_icon"))
{
	url = feed;
	feed_items = new FeedItemList;
	parser = new FeedParser;

	connect(this, SIGNAL(clicked()), SLOT(displayFeed()));
	connect(parser, SIGNAL(feedReady()), SLOT(feedReady()));

	connect(feed_items, SIGNAL(Closed()), SIGNAL(pageClosed()));
}

void HomepageFeedLink::hideEvent(QHideEvent *e)
{
	// do not display the feed if the user moves away from the home page
	parser->abort();
}

void HomepageFeedLink::displayFeed()
{
	parser->parse(url);
}

void HomepageFeedLink::feedReady()
{
	feed_items->setFeedInfo(0, parser->getFeedData());
	feed_items->showPage();
}


HomepageIPRadioLink::HomepageIPRadioLink(const QString &description, const QString &radio_url) :
	HomepageLink(description, bt_global::skin->getImage("link_icon"))
{
	url = radio_url;
	player = AudioPlayerPage::getAudioPlayerPage(AudioPlayerPage::IP_RADIO);

	connect(this, SIGNAL(clicked()), SLOT(playRadio()));
}

void HomepageIPRadioLink::playRadio()
{
	player->showPrevButton(false);
	player->showNextButton(false);

	disconnect(player, SIGNAL(Closed()), this, SLOT(radioClosed())); // avoid multiple connections
	connect(player, SIGNAL(Closed()), this, SLOT(radioClosed()));
	player->playAudioFiles(QStringList() << url, 0);
}

void HomepageIPRadioLink::radioClosed()
{
	if (!player->isPlayerInstanceRunning())
		disconnect(player, SIGNAL(Closed()), this, SLOT(radioClosed()));
	emit pageClosed();
}


HomepageWebcamLink::HomepageWebcamLink(const QString &description, const QString &webcam_url) :
	HomepageLink(description, bt_global::skin->getImage("link_icon"))
{
	url = webcam_url;
	title = description;

	webcam = new WebcamPage;
	webcam->displayNavigationButtons(false);

	connect(this, SIGNAL(clicked()), SLOT(showWebcam()));

	connect(webcam, SIGNAL(Closed()), SIGNAL(pageClosed()));
	connect(webcam, SIGNAL(reloadImage()), SLOT(reloadWebcam()));
}

void HomepageWebcamLink::reloadWebcam()
{
	webcam->setImage(QUrl(url), title);
}

void HomepageWebcamLink::showWebcam()
{
	webcam->setImage(QUrl(url), title);
	webcam->showPage();
}


HeaderLogo::HeaderLogo(TrayBar *tray)
{
	setFixedSize(800, 40);
	tray_bar = tray;
}

void HeaderLogo::loadItems(const QDomNode &config_node)
{
	date_display = 0;
	time_display = 0;
	temperature_display = 0;

	QHBoxLayout *l = new QHBoxLayout(this);
	l->setContentsMargins(3, 0, 10, 0);
	l->setSpacing(10);

	l->addWidget(tray_bar);

	l->addStretch(1);

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();

		switch (id)
		{
		case ITEM_DATE:
			date_display = new InnerPageDateDisplay;

			l->addWidget(date_display);
			break;
		case ITEM_TIME:
			time_display = new InnerPageTimeDisplay;

			l->addWidget(time_display);
			break;
		case ITEM_TEMPERATURE_PROBE:
		case ITEM_TEMPERATURE_EXTPROBE:
			NonControlledProbeDevice::ProbeType type = id == ITEM_TEMPERATURE_PROBE ? NonControlledProbeDevice::INTERNAL : NonControlledProbeDevice::EXTERNAL;

			device *probe = bt_global::add_device_to_cache(new NonControlledProbeDevice(getTextChild(item, "where"),
												    type, getTextChild(item, "openserver_id").toInt()));

			temperature_display = new InnerPageTemperatureDisplay(probe);
			l->addWidget(temperature_display);
			break;
		}
	}
}

void HeaderLogo::setControlsVisible(bool visible)
{
	if (date_display)
		date_display->setVisible(visible);
	if (time_display)
		time_display->setVisible(visible);
	if (temperature_display)
		temperature_display->setVisible(visible);
}


HeaderInfo::HeaderInfo()
{
	setFixedSize(800, 105);
}

void HeaderInfo::loadItems(const QDomNode &config_node, Page *s)
{
	settings = s;
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

			connect(button, SIGNAL(clicked()), SLOT(showSettings()));

			break;
		}
		case ITEM_TEMPERATURE_PROBE:
		case ITEM_TEMPERATURE_EXTPROBE:
		{
			NonControlledProbeDevice::ProbeType type = id == ITEM_TEMPERATURE_PROBE ? NonControlledProbeDevice::INTERNAL : NonControlledProbeDevice::EXTERNAL;

			device *probe = bt_global::add_device_to_cache(new NonControlledProbeDevice(getTextChild(item, "where"),
												    type, getTextChild(item, "openserver_id").toInt()));
			QWidget *item = new HomepageTemperatureDisplay(probe);
			home_layout->addWidget(item);
			break;
		}
		case ITEM_RSS_LINK:
		{
			HomepageFeedLink *feed_display = new HomepageFeedLink(getTextChild(item, "descr"), getTextChild(item, "url"));
			info_layout->addWidget(feed_display);
			connect(feed_display, SIGNAL(pageClosed()), SIGNAL(showHomePage()));
			connect(feed_display, SIGNAL(aboutToClick()), SIGNAL(aboutToChangePage()));

			break;
		}
		case ITEM_WEB_RADIO_LINK:
		{
			HomepageIPRadioLink *radio_display = new HomepageIPRadioLink(getTextChild(item, "descr"), getTextChild(item, "url"));
			info_layout->addWidget(radio_display);
			connect(radio_display, SIGNAL(pageClosed()), SIGNAL(showHomePage()));
			connect(radio_display, SIGNAL(aboutToClick()), SIGNAL(aboutToChangePage()));

			break;
		}
		case ITEM_WEB_CAM_LINK:
		{
			HomepageWebcamLink *webcam_display = new HomepageWebcamLink(getTextChild(item, "descr"), getTextChild(item, "url"));
			info_layout->addWidget(webcam_display);
			connect(webcam_display, SIGNAL(pageClosed()), SIGNAL(showHomePage()));
			connect(webcam_display, SIGNAL(aboutToClick()), SIGNAL(aboutToChangePage()));

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

void HeaderInfo::showSettings()
{
	emit aboutToChangePage();
	settings->showPage();
}


HeaderNavigationBar::HeaderNavigationBar()
{
	setFixedSize(800, 85);
}

void HeaderNavigationBar::loadItems(const QDomNode &config_node, Page *s)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	settings = s;

	QHBoxLayout *main_layout = new QHBoxLayout(this);
	main_layout->setContentsMargins(10, 0, 0, 0);
	main_layout->setSpacing(0);

	BtButton *home = new BtButton;
	home->setImage(bt_global::skin->getImage("go_home"));
	main_layout->addWidget(home);
	connect(home, SIGNAL(clicked()), SLOT(goHome()));

	navigation = new HeaderNavigationWidget;
	main_layout->addWidget(navigation, 1);
	connect(navigation, SIGNAL(pageSelected(int)), SLOT(pageSelected(int)));

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		int page_id = getTextChild(item, "lnk_pageID").toInt();
		QDomNode page_node = getPageNodeFromPageId(page_id);
		int id = getTextChild(page_node, "id").toInt();

		QString icon = bt_global::skin->getImage("top_navigation_button");
		QString icon_on;
		if (bt_global::skin->exists("top_navigation_button_on"))
			icon_on = bt_global::skin->getImage("top_navigation_button_on");
		navigation->addButton(id, page_id, icon, icon_on);
	}

	// add link to settings
	StateButton *link = navigation->createButton(settings->sectionId(), bt_global::skin->getImage("settings_icon"));
	connect(link, SIGNAL(clicked()), SLOT(showSettings()));
}

void HeaderNavigationBar::pageSelected(int page_id)
{
	emit aboutToChangePage();
	emit showSectionPage(page_id);
}

void HeaderNavigationBar::goHome()
{
	emit aboutToChangePage();
	emit showHomePage();
}

void HeaderNavigationBar::showSettings()
{
	emit aboutToChangePage();
	settings->showPage();
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

	left = new BtButton(bt_global::skin->getImage("left"));
	left->setAutoRepeat(true);
	connect(left, SIGNAL(clicked()), SLOT(scrollLeft()));

	right = new BtButton(bt_global::skin->getImage("right"));
	right->setAutoRepeat(true);
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

StateButton *HeaderNavigationWidget::createButton(int section_id, const QString &icon, const QString &icon_on)
{
	StateButton *link = new StateButton;
	link->setOffImage(icon);
	if (!icon_on.isEmpty())
		link->setOnImage(icon_on);
	buttons.append(link);

	QLabel *active = new QLabel;
	active->setAlignment(Qt::AlignHCenter);
	active->setPixmap(getBostikName(icon, "s"));
	active->setFixedSize(active->pixmap()->size());
	selected.append(active);

	section_ids.append(section_id);
	need_update = true;

	return link;
}

void HeaderNavigationWidget::addButton(int section_id, int page_id, const QString &icon, const QString &icon_on)
{
	StateButton *link = createButton(section_id, icon, icon_on);

	mapper->setMapping(link, page_id);
	connect(link, SIGNAL(clicked()), mapper, SLOT(map()));
}

QWidget *HeaderNavigationWidget::itemForIndex(int i)
{
	int index = (current_index + i) % buttons.size();

	if (section_ids[index] == selected_section_id)
		return selected[index];
	else
		return buttons[index];
}

void HeaderNavigationWidget::drawContent()
{
	if (!need_update)
		return;

	need_update = false;

	while (QLayoutItem *child = button_layout->takeAt(0))
		if (QWidget *w = child->widget())
			w->hide();

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
		QWidget *item = itemForIndex(i);

		item->show();
		button_layout->addWidget(item, 1, Qt::AlignCenter);
	}
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

	int prev_index = section_ids.indexOf(selected_section_id);
	int index = section_ids.indexOf(section_id);

	// should only happen when there are pages instantiated for a section without
	// the related section; the only known case is a touch that works as a videocall
	// receiver without the videodoor section
	if (index == -1)
		return;

	Q_ASSERT_X(prev_index != -1 || selected_section_id == 0, "HeaderNavigationWidget::setCurrentSection",
		   qPrintable(QString("Invalid section value %1").arg(selected_section_id)));

	for (int item_index = 0; item_index < button_layout->count(); ++item_index)
	{
		// show the selected icon if the corresponding button is in the layout
		if (button_layout->itemAt(item_index)->widget() == buttons[index])
		{
			delete button_layout->takeAt(item_index);
			button_layout->insertWidget(item_index, selected[index], 1, Qt::AlignCenter);

			buttons[index]->hide();
			selected[index]->show();
		}

		// show the button if the corresponding selected icon is in the layout
		if (prev_index != -1 && button_layout->itemAt(item_index)->widget() == selected[prev_index])
		{
			delete button_layout->takeAt(item_index);
			button_layout->insertWidget(item_index, buttons[prev_index], 1, Qt::AlignCenter);

			buttons[prev_index]->show();
			selected[prev_index]->hide();
		}
	}

	selected_section_id = section_id;
}

void HeaderNavigationWidget::changeIconState(int page_id, StateButton::Status st)
{
	if (StateButton *b = qobject_cast<StateButton*>(mapper->mapping(page_id)))
		b->setStatus(st);
}


HeaderWidget::HeaderWidget(TrayBar *tray_bar)
{
	main_layout = new QVBoxLayout(this);
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);

	header_logo = new HeaderLogo(tray_bar);
	main_layout->addWidget(header_logo);

	top_nav_bar = new HeaderNavigationBar;
	top_nav_bar->hide();
	main_layout->addWidget(top_nav_bar);

	connect(top_nav_bar, SIGNAL(showSectionPage(int)), SIGNAL(showSectionPage(int)));
	connect(top_nav_bar, SIGNAL(showHomePage()), SIGNAL(showHomePage()));
	connect(top_nav_bar, SIGNAL(aboutToChangePage()), SIGNAL(aboutToChangePage()));

	header_info = new HeaderInfo;
	main_layout->addWidget(header_info);

	connect(header_info, SIGNAL(showHomePage()), SIGNAL(showHomePage()));
	connect(header_info, SIGNAL(aboutToChangePage()), SIGNAL(aboutToChangePage()));
}

void HeaderWidget::loadConfiguration(const QDomNode &homepage_node, const QDomNode &infobar_node)
{
	Page *settings = NULL;

	foreach (const QDomNode &item, getChildren(infobar_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();

		if (id == ITEM_SETTINGS_LINK)
		{
			settings = new IconSettings(getPageNodeFromChildNode(item, "lnk_pageID"));
			connect(settings, SIGNAL(Closed()), SIGNAL(showHomePage()));

			break;
		}
	}

	Q_ASSERT_X(settings, "HeaderWidget::loadConfiguration", "No settings item defined!");

	header_logo->loadItems(infobar_node);
	top_nav_bar->loadItems(homepage_node, settings);
	header_info->loadItems(infobar_node, settings);
}

void HeaderWidget::centralPageChanged(int section_id, bool is_homepage)
{
	qDebug() << "new central widget = " << " id " << section_id;

	header_logo->setControlsVisible(!is_homepage);
	top_nav_bar->setVisible(!is_homepage);
	header_logo->show();
	header_info->setVisible(is_homepage);

	if (!is_homepage)
		top_nav_bar->setCurrentSection(section_id);

	updateGeometry();
}

void HeaderWidget::sectionChanged(int section_id)
{
	qDebug() << "new section = " << section_id;
	top_nav_bar->setCurrentSection(section_id);
}

void HeaderWidget::iconStateChanged(int page_id, StateButton::Status st)
{
	top_nav_bar->navigation->changeIconState(page_id, st);
}

