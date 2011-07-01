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


#include "settings_ts10.h"
#include "navigation_bar.h"
#include "xml_functions.h"
#include "skinmanager.h"
#include "version.h"
#include "screensaverpage.h"
#include "cleanscreen.h"
#include "btbutton.h"
#include "hardware_functions.h" // setBeep/getBeep/beep
#include "bannerfactory.h"
#include "lansettings.h" // LanSettings, Text2Column
#include "banner.h"
#include "bann_settings.h" // impPassword
#include "items.h" // ItemTuning
#include "devices_cache.h" // bt_global::add_device_to_cache
#include "platform_device.h" // PlatformDevice
#include "generic_functions.h" // setCfgValue
#include "displaycontrol.h" // bt_global::display
#include "videodoorentry.h" // HandsFree, ProfessionalStudio
#include "main.h" // bt_global::config, SETTINGS
#include "audiostatemachine.h" // bt_global::audio_states
#include "fontmanager.h"

#if !defined(BT_HARDWARE_X11)
#include "calibration.h"
#endif

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

enum
{
	ICON_BEEP = 14001,
	PAGE_DATE_TIME = 14002,
	PAGE_BRIGHTNESS = 14151,
	PAGE_PASSWORD = 14003,
	PAGE_VOLUME = 14004,
	PAGE_RINGTONES = 14005,
	PAGE_CALIBRATION = 14153,
	PAGE_INFO = 14006,
	PAGE_SCREENSAVER = 14154,
	PAGE_CLEANSCREEN = 14152,
	PAGE_DISPLAY = 14007,
	PAGE_ALARMCLOCK = 14009,
	RINGTONE_0 = 14101,
	RINGTONE_1 = 14102,
	RINGTONE_2 = 14103,
	RINGTONE_3 = 14104,
	RINGTONE_PI_INTERCOM = 14105,
	RINGTONE_PE_INTERCOM = 14106,
	RINGTONE_FLOORCALL = 14107,
	RINGTONE_ALARM = 14109,
	RINGTONE_MESSAGE = 14110,
	PAGE_LANSETTINGS = 14008,
	PAGE_VCTSETTINGS = 14010,
	// TODO ids?
	PAGE_CALIBRATION_TEST = 1777777,
};

namespace
{
	Ringtones::Type idToRingtoneType(int id)
	{
		switch (id)
		{
		case RINGTONE_0:
			return Ringtones::PE1;
		case RINGTONE_1:
			return Ringtones::PE2;
		case RINGTONE_2:
			return Ringtones::PE3;
		case RINGTONE_3:
			return Ringtones::PE4;
		case RINGTONE_PI_INTERCOM:
			return Ringtones::PI_INTERCOM;
		case RINGTONE_PE_INTERCOM:
			return Ringtones::PE_INTERCOM;
		case RINGTONE_FLOORCALL:
			return Ringtones::FLOORCALL;
		case RINGTONE_ALARM:
			return Ringtones::ALARM;
		case RINGTONE_MESSAGE:
		default:
			return Ringtones::MESSAGE;
		}
	}
}


ToggleBeep::ToggleBeep(int _item_id, bool status, QString label, QString icon_on, QString icon_off) :
	IconPageButton(label)
{
	item_id = _item_id;

	button->setCheckable(true);
	button->setOffImage(icon_off);
	button->setOnImage(icon_on);
	button->setStatus(status);

	setBeep(status);
	connect(button, SIGNAL(clicked()), SLOT(toggleBeep()));
}

void ToggleBeep::toggleBeep()
{
	bool beep_on = !getBeep();

	setBeep(beep_on);
	button->setStatus(beep_on);

#ifdef CONFIG_TS_3_5
	setCfgValue("value", beep_on, SUONO);
#else
	setCfgValue("enabled", beep_on, item_id);
#endif

	if (beep_on)
		beep();
}


ListPage::ListPage(const QDomNode &config_node)
{
	buildPage(getTextChild(config_node, "descr"));
	loadItems(config_node);
}

void ListPage::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode& item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();

		if (Banner *b = getBanner(item))
		{
			page_content->appendBanner(b);
			connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
		}
		else
			qFatal("Type of item %d not handled on settings page!", id);
	}
}


RingtonesPage::RingtonesPage(const QDomNode &config_node) : ListPage(config_node)
{
	connect(this, SIGNAL(Closed()), SLOT(handleClose()));
}

void RingtonesPage::handleClose()
{
	bt_global::ringtones->stopRingtone();
}

void RingtonesPage::cleanUp()
{
	handleClose();
}

void RingtonesPage::showPage()
{
	Page::showPage();
}


VolumePage::VolumePage(const QDomNode &config_node)
{
	QWidget *content = new QWidget;
	QHBoxLayout *layout = new QHBoxLayout(content);
	layout->setContentsMargins(0, 0, 0, TITLE_HEIGHT);

	volume = new ItemTuning(tr("Volume"), bt_global::skin->getImage("volume"));
	volume->enableBeep(false);
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(this, SIGNAL(Closed()), SLOT(handleClose()));

	layout->addWidget(volume, 0, Qt::AlignCenter);
	buildPage(content, nav_bar, getTextChild(config_node, "descr"));

	connect(volume, SIGNAL(valueChanged(int)), SLOT(playRingtone()));

	is_playing = false;
}

void VolumePage::handleClose()
{
	bt_global::ringtones->stopRingtone();
}

void VolumePage::cleanUp()
{
	handleClose();
}

void VolumePage::ringtoneFinished()
{
	if (is_playing)
	{
		is_playing = false;
		bt_global::audio_states->removeState(AudioStates::PLAY_RINGTONE);
		disconnect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(ringtoneFinished()));
	}
}

void VolumePage::playRingtone()
{
	if (!is_playing)
	{
		connect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), SLOT(startPlayRingtone()));
		bt_global::audio_states->toState(AudioStates::PLAY_RINGTONE);
	}
	else
		startPlayRingtone();
}

void VolumePage::startPlayRingtone()
{
	disconnect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), this, SLOT(startPlayRingtone()));
	if (!is_playing)
		connect(bt_global::ringtones, SIGNAL(ringtoneFinished()), SLOT(ringtoneFinished()));
	is_playing = true;

	bt_global::audio_states->setVolume(volume->level());
	bt_global::ringtones->playRingtone(Ringtones::PE1);
}



InfoPage::InfoPage(const QDomNode &config_node)
{
	// TODO for touch 3.5 we need to send status requests as early as possible,
	// because the version page is shown during boot

	dev = bt_global::add_device_to_cache(new PlatformDevice);
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	QWidget *content = new QWidget;
	QVBoxLayout *content_layout = new QVBoxLayout(content);
	content_layout->setContentsMargins(0, 10, 0, 0);

	text_area = new Text2Column;
	text_area->setSpacing(20);
	text_area->setFont(bt_global::font->get(FontManager::TEXT));
	text_area->addRow(tr("Model"), (*bt_global::config)[MODEL]);
	text_area->addRow(tr("Firmware version"), "");
	text_area->addRow(tr("Kernel version"), "");
	text_area->addRow(tr("Address N."), (*bt_global::config)[PI_ADDRESS].mid(1));
	text_area->addRow(tr("T.S. N."), (*bt_global::config)[TS_NUMBER]);

	content_layout->addWidget(text_area);
	content_layout->addStretch(1);
	buildPage(content, nav_bar, getTextChild(config_node, "descr"), TINY_TITLE_HEIGHT);
}

void InfoPage::valueReceived(const DeviceValues &values_list)
{
	const int FW_ROW = 2;
	const int KERN_ROW = 3;

	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case PlatformDevice::DIM_KERN_VERS:
			text_area->setText(KERN_ROW, it.value().toString());
			break;
		case PlatformDevice::DIM_FW_VERS:
			text_area->setText(FW_ROW, it.value().toString());
			break;
		}

		++it;
	}
}


ChangeDateTime::ChangeDateTime(const QString &ok_button_icon) :
	PageSetDateTime(ok_button_icon, true)
{
	connect(this, SIGNAL(dateTimeSelected(QDate,BtTime)), SLOT(dateTimeChanged(QDate, BtTime)));
	connect(this, SIGNAL(dateTimeSelected(QDate,BtTime)), SIGNAL(Closed()));
	dev = bt_global::add_device_to_cache(new PlatformDevice);
}

void ChangeDateTime::showPage()
{
	setDateTime(QDateTime::currentDateTime());
	PageSetDateTime::showPage();
}

void ChangeDateTime::dateTimeChanged(QDate date, BtTime time)
{
	dev->setTime(time);
	dev->setDate(date);
}


IconSettings::IconSettings(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	loadItems(config_node);
}

int IconSettings::sectionId() const
{
	return SETTINGS;
}

Banner *IconSettings::getBanner(const QDomNode &item_node)
{
	SkinContext ctx(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	int item_id = getTextChild(item_node, "itemID").toInt();

	Banner *b = 0;
	QString descr = getTextChild(item_node, "descr");
	switch (id)
	{
	case SET_SVEGLIA_SINGLEPAGE:
	{
		int type = getTextChild(item_node, "type").toInt();
		int enabled = getTextChild(item_node, "enabled").toInt();
		int hour = getTextChild(item_node, "hour").toInt();
		int minute = getTextChild(item_node, "minute").toInt();
		int days = getTextChild(item_node, "days").toInt();

		b = new BannAlarmClockIcon(item_id, hour, minute,
					bt_global::skin->getImage("on"),
					bt_global::skin->getImage("off"),
					bt_global::skin->getImage("state_icon_on"),
					bt_global::skin->getImage("state_icon_off"),
					bt_global::skin->getImage("edit"),
					getTextChild(item_node, "descr"),
					enabled, type, days);

		break;
	}

	case RINGTONE_0:
	case RINGTONE_1:
	case RINGTONE_2:
	case RINGTONE_3:
	case RINGTONE_PI_INTERCOM:
	case RINGTONE_PE_INTERCOM:
	case RINGTONE_FLOORCALL:
	case RINGTONE_ALARM:
	case RINGTONE_MESSAGE:
	{

		b = new BannRingtone(descr, item_id, idToRingtoneType(id),
				getTextChild(item_node, "id_ringtone").toInt());
		break;
	}
	}

	return b;
}


void IconSettings::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QString icon = bt_global::skin->getImage("link_icon");
		QString descr = getTextChild(item, "descr");
		Page *p = 0;
		Window *w = 0;

		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		int link_id = getTextChild(item, "id").toInt();

		switch (link_id)
		{
		case PAGE_DATE_TIME:
		{
			ChangeDateTime *page = new ChangeDateTime(bt_global::skin->getImage("ok"));
			page->setTitle(descr);
			p = page;
		}
			break;
		case PAGE_ALARMCLOCK:
			p = new ListPage(page_node);
			break;
		case PAGE_DISPLAY:
			p = new IconSettings(page_node);
			break;
		case PAGE_SCREENSAVER:
		{
			int timeout = getTextChild(item, "timeout").toInt() / 1000;
			int blank = getTextChild(item, "blankscreen").toInt() / 1000;
			bt_global::display->setScreenSaverTimeouts(timeout, blank == 0 ? 0 : timeout + blank);
			// Screensaver data is not correctly initialized (in BtMain) when using TS 10'' conf file
			ScreenSaver::initData(item);
			p = new ScreenSaverPage(item);
			break;
		}
		case PAGE_CLEANSCREEN:
		{
			int countdown = getTextChild(item, "countdown").toInt() / 1000;
			w = new CleanScreen(bt_global::skin->getImage("cleanscreen"), countdown);
			break;
		}
#if !defined(BT_HARDWARE_X11)
		case PAGE_CALIBRATION:
			w = new Calibration;
			break;
		case PAGE_CALIBRATION_TEST:
			w = new CalibrationTest;
			break;
#else
		case PAGE_CALIBRATION:
		case PAGE_CALIBRATION_TEST:
			break;
#endif
		case PAGE_LANSETTINGS:
			p = new LanSettings(item);
			break;
		case ICON_BEEP:
		{
			QWidget *t = new ToggleBeep(getTextChild(item, "itemID").toInt(),
						    getTextChild(item, "enabled").toInt(), descr,
						    bt_global::skin->getImage("state_on"),
						    bt_global::skin->getImage("state_off"));
			page_content->addWidget(t);
			break;
		}
		case PAGE_PASSWORD:
			p = new PasswordPage(item);
			break;
		case PAGE_RINGTONES:
			p = new RingtonesPage(page_node);
			break;
		case PAGE_VOLUME:
			p = new VolumePage(item);
			break;
		case PAGE_INFO:
			p = new InfoPage(item);
			break;
		case PAGE_BRIGHTNESS:
			p = new BrightnessPage;
			break;
		case PAGE_VCTSETTINGS:
			p = new VctSettings(page_node);
			break;
		default:
			qFatal("Unhandled page id %d in IconSettings::loadItems", link_id);
		};

		if (p)
		{
			p->inizializza();
			addPage(p, descr, icon);
		}
		else if (w)
		{
			BtButton *b = addButton(descr, icon);
			connect(b, SIGNAL(clicked()), w, SLOT(showWindow()));
		}
	}
}


// TODO: password item in conf file hasn't its own page; this has two consequences:
// 1. I need to parse here the conf file
// 2. I can't use Settings::getBanner() because I don't have the correct item_node to pass to it (and xml tags
//    are different from the ones used in TS 3.5'' conf anyway).
PasswordPage::PasswordPage(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	QString descr = getTextChild(config_node, "descr");
	QWidget *content = new QWidget;
	QGridLayout *layout = new QGridLayout(content);
	layout->setContentsMargins(0, 0, 0, TITLE_HEIGHT);

	int item_id = getTextChild(config_node, "itemID").toInt();
	bool active = getTextChild(config_node, "actived").toInt();
	PasswordChanger *changer = new PasswordChanger(item_id, getTextChild(config_node, "password"), active);
	StateButton *left_button = new StateButton;
	BtButton *right_button = new BtButton(bt_global::skin->getImage("edit"));

	left_button->setOffImage(bt_global::skin->getImage("state_off"));
	left_button->setOnImage(bt_global::skin->getImage("state_on"));
	left_button->setStatus(active);

	connect(right_button, SIGNAL(clicked()), changer, SLOT(changePassword()));
	connect(left_button, SIGNAL(clicked()), changer, SLOT(requestPasswdOn()));
	connect(changer, SIGNAL(finished()), SLOT(showPage()));
	connect(changer, SIGNAL(passwordActive(bool)), left_button, SLOT(setStatus(bool)));

	QGridLayout *b = new QGridLayout;

	QLabel *left_label = new QLabel(tr("Activate"));
	QLabel *right_label = new QLabel(tr("Change"));

	left_label->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));
	right_label->setFont(bt_global::font->get(FontManager::BANNERDESCRIPTION));

	b->addWidget(left_button, 0, 0, Qt::AlignHCenter);
	b->addWidget(right_button, 0, 2, Qt::AlignHCenter);
	b->addWidget(left_label, 1, 0, Qt::AlignHCenter);
	b->addWidget(right_label, 1, 2, Qt::AlignHCenter);
	b->setColumnStretch(1, 1);

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	layout->addLayout(b, 1, 1);
	layout->setRowStretch(0, 1);
	layout->setRowStretch(2, 1);
	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, 2);
	layout->setColumnStretch(2, 1);

	buildPage(content, nav_bar, descr);
}


BrightnessPage::BrightnessPage()
{
	QWidget *content = new QWidget;
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);

	buildPage(content, nav_bar, tr("Brightness"), SMALL_TITLE_HEIGHT);

	// create buttons
	BtButton *decBut = new BtButton(bt_global::skin->getImage("minus"));
	BtButton *incBut = new BtButton(bt_global::skin->getImage("plus"));

	incBut->setAutoRepeat(true);
	decBut->setAutoRepeat(true);

	connect(decBut, SIGNAL(clicked()), SLOT(decBrightness()));
	connect(incBut, SIGNAL(clicked()), SLOT(incBrightness()));

	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	// create logo
	QLabel *logo = new QLabel;
	logo->setPixmap(QPixmap(bt_global::skin->getImage("logo_brightness")));

	// layout
	QHBoxLayout *b = new QHBoxLayout;
	b->setContentsMargins(0, 0, 0, 0);
	b->setSpacing(20);

	b->addWidget(decBut, 0, Qt::AlignRight);
	b->addWidget(incBut, 0 , Qt::AlignLeft);

	QVBoxLayout *l = new QVBoxLayout(content);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(10);

	l->addWidget(logo, 0, Qt::AlignCenter);
	l->addLayout(b, Qt::AlignCenter);
}

void BrightnessPage::incBrightness()
{
	int b = bt_global::display->operativeBrightness();
	if (b < 10)
		bt_global::display->setOperativeBrightness(b + 1);
}

void BrightnessPage::decBrightness()
{
	int b = bt_global::display->operativeBrightness();
	if (b > 1)
		bt_global::display->setOperativeBrightness(b - 1);
}
