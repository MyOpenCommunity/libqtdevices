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


#include "iconsettings.h"
#include "navigation_bar.h"
#include "xml_functions.h"
#include "skinmanager.h"
#include "btmain.h" // version page
#include "version.h"
#include "screensaverpage.h"
#include "cleanscreen.h"
#include "btbutton.h"
#include "hardware_functions.h" // setBeep/getBeep/beep
#include "bannerfactory.h"
#include "bannercontent.h"
#include "lansettings.h" // LanSettings, Text2Column
#include "banner.h"
#include "bann_settings.h" // impPassword
#include "items.h" // ItemTuning
#include "devices_cache.h" // bt_global::add_device_to_cache
#include "platform_device.h" // PlatformDevice
#include "generic_functions.h" // setCfgValue
#include "displaycontrol.h"
#include "videodoorentry.h"
#include "main.h" // bt_global::config

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
	PAGE_VERSION = 14006,
	PAGE_SCREENSAVER = 14154,
	PAGE_CLEANSCREEN = 14152,
	PAGE_DISPLAY = 14007,
	PAGE_ALARMCLOCK = 14009,

	// TODO should be the same ID
	RINGTONE_0 = 14101,
	RINGTONE_1 = 14102,
	RINGTONE_2 = 14103,
	RINGTONE_3 = 14104,
	RINGTONE_INTERCOM = 14105,
	RINGTONE_VCT = 14106,
	RINGTONE_DOOR = 14107,
	RINGTONE_ALARM = 14109,

	// TODO ids?
	PAGE_LANSETTINGS = 1700000,
	PAGE_CALIBRATION_TEST = 1777777,
};


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

#ifdef CONFIG_BTOUCH
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

		if (banner *b = getBanner(item))
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
}

void RingtonesPage::hideEvent(QHideEvent *e)
{
	bt_global::ringtones->stopRingtone();
}


VolumePage::VolumePage(const QDomNode &config_node)
{
	QWidget *content = new QWidget;
	QHBoxLayout *layout = new QHBoxLayout(content);
	layout->setContentsMargins(0, 0, 0, TITLE_HEIGHT);

	// TODO: is this text ok for the banner? Should it be read from conf?
	ItemTuning *volume = new ItemTuning(tr("Volume"), bt_global::skin->getImage("volume"));
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	layout->addWidget(volume, 0, Qt::AlignCenter);
	buildPage(content, nav_bar, getTextChild(config_node, "descr"));

	connect(volume, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));
}

void VolumePage::hideEvent(QHideEvent *e)
{
	bt_global::ringtones->stopRingtone();
}

void VolumePage::changeVolume(int new_vol)
{
	setVolume(VOLUME_RING, new_vol);
	bt_global::ringtones->playRingtone(RINGTONE_PE1);
}


VersionPage::VersionPage(const QDomNode &config_node)
{
	dev = bt_global::add_device_to_cache(new PlatformDevice);
	connect(dev, SIGNAL(status_changed(const StatusList &)), SLOT(status_changed(const StatusList &)));
	dev->requestFirmwareVersion();
	dev->requestKernelVersion();
	dev->requestIp();
	dev->requestNetmask();

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	text_area = new Text2Column;
	text_area->addRow(tr("Model"), (*bt_global::config)[MODEL]);
	text_area->addRow(tr("Firmware version"), "");
	text_area->addRow(tr("Kernel version"), "");
	text_area->addRow(tr("IP address"), "");
	text_area->addRow(tr("Netmask"), "");

	buildPage(text_area, nav_bar, getTextChild(config_node, "descr"), TINY_TITLE_HEIGHT);
}

void VersionPage::status_changed(const StatusList &sl)
{
	const int FW_ROW = 2;
	const int KERN_ROW = 3;
	const int IP_ROW = 4;
	const int NETMASK_ROW = 5;

	StatusList::const_iterator it = sl.constBegin();
	while (it != sl.constEnd())
	{
		switch (it.key())
		{
		case PlatformDevice::DIM_KERN_VERS:
			text_area->setText(KERN_ROW, it.value().toString());
			break;
		case PlatformDevice::DIM_FW_VERS:
			text_area->setText(FW_ROW, it.value().toString());
			break;
		case PlatformDevice::DIM_IP:
			text_area->setText(IP_ROW, it.value().toString());
			break;
		case PlatformDevice::DIM_NETMASK:
			text_area->setText(NETMASK_ROW, it.value().toString());
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


IconSettings::IconSettings(const QDomNode &config_node, bool load_vct_items)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	loadItems(config_node);


	if (load_vct_items && !(*bt_global::config)[PI_ADDRESS].isEmpty())
		addPage(new SettingsVideoDoorEntry, tr("Video door entry"),
			bt_global::skin->getImage("videodoorentry"));
}

int IconSettings::sectionId()
{
	return IMPOSTAZIONI;
}

banner *IconSettings::getBanner(const QDomNode &item_node)
{
	SkinContext ctx(getTextChild(item_node, "cid").toInt());
	int id = getTextChild(item_node, "id").toInt();
	banner *b = 0;
	QString descr = getTextChild(item_node, "descr");
	switch (id)
	{
	case SET_SVEGLIA_SINGLEPAGE:
	{
		int item_id = getTextChild(item_node, "itemID").toInt();
		int type = getTextChild(item_node, "type").toInt();
		int enabled = getTextChild(item_node, "enabled").toInt();
		int hour = getTextChild(item_node, "hour").toInt();
		int minute = getTextChild(item_node, "minute").toInt();
		int days = getTextChild(item_node, "days").toInt();

		QList<bool> alarms;
		for (int i = 0; i < 7; ++i)
		{
			int mask = 1 << (6 - i);
			alarms.append((days & mask) ? true : false);
		}

		b = new bannAlarmClockIcon(item_id, hour, minute,
					   bt_global::skin->getImage("on"),
					   bt_global::skin->getImage("off"),
					   bt_global::skin->getImage("state_icon"),
					   bt_global::skin->getImage("edit"),
					   getTextChild(item_node, "descr"),
					   enabled, type, alarms);

		break;
	}
	case RINGTONE_0:
	case RINGTONE_1:
	case RINGTONE_2:
	case RINGTONE_3:
	case RINGTONE_INTERCOM:
	case RINGTONE_VCT:
	case RINGTONE_DOOR:
	case RINGTONE_ALARM:
		// TODO: type should be read from config file
		b = new BannRingtone(descr, RINGTONE_PE1);
		break;
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
			bt_global::btmain->setScreenSaverTimeouts(timeout, timeout + blank);
			p = new ScreenSaverPage;
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
		case PAGE_VERSION:
			p = new VersionPage(item);
			break;
		case PAGE_BRIGHTNESS:
			p = new BrightnessPage;
			break;
		default:
			;// qFatal("Unhandled page id in SettingsTouchX::loadItems");
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
//    are different from the ones used in btouch conf anyway).
PasswordPage::PasswordPage(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	QString descr = getTextChild(config_node, "descr");
	QWidget *content = new QWidget;
	QGridLayout *layout = new QGridLayout(content);
	layout->setContentsMargins(0, 0, 0, TITLE_HEIGHT);

	int item_id = getTextChild(config_node, "itemID").toInt();
	banner *b = new impPassword(bt_global::skin->getImage("state_on"),
		bt_global::skin->getImage("state_off"), bt_global::skin->getImage("edit"), descr, item_id,
		getTextChild(config_node, "password"), getTextChild(config_node, "actived").toInt());
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));

	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));

	layout->addWidget(b, 1, 1);
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
	int b = (*bt_global::display).operativeBrightness();
	if (b > 10)
		(*bt_global::display).setOperativeBrightness(b - 10);
}

void BrightnessPage::decBrightness()
{
	int b = (*bt_global::display).operativeBrightness();
	if (b < 250)
		(*bt_global::display).setOperativeBrightness(b + 10);
}
