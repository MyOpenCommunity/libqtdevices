#include "iconsettings.h"
#include "navigation_bar.h"
#include "xml_functions.h"
#include "skinmanager.h"
#include "btmain.h" // version page
#include "version.h"
#include "screensaverpage.h"
#include "cleanscreen.h"
#include "btbutton.h"
#include "main.h" // pagSecLiv
#include "hardware_functions.h" // setBeep/getBeep/beep
#include "bannerfactory.h"
#include "bannercontent.h"
#include "lansettings.h" // LanSettings, Text2Column
#include "banner.h"
#include "bann_settings.h" // impPassword
#include "items.h" // ItemTuning
#include "devices_cache.h" // bt_global::add_device_to_cache
#include "platform_device.h" // PlatformDevice

#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

enum
{
	PAGE_LANSETTINGS = 72,  // TODO: this is not unique in touchx configuration file
	PAGE_DATE_TIME = 2902,
	PAGE_BRIGHTNESS = 2903,
	PAGE_PASSWORD = 2907,
	PAGE_VOLUME = 2908,
	PAGE_RINGTONES = 2909,
	PAGE_CALIBRATION = 2910,
	PAGE_VERSION = 2911,
	PAGE_SCREENSAVER = 2912,
	PAGE_CLEANSCREEN = 2913,
	PAGE_DISPLAY = 2920,
	PAGE_ALARMCLOCK = 2950
};


ToggleBeep::ToggleBeep(bool status, QString label, QString icon_on, QString icon_off)
{
	QLabel *lbl = new QLabel(label);
	lbl->setText(label);
	lbl->setAlignment(Qt::AlignHCenter);

	button = new BtButton;
	button->setStatus(status);
	button->setCheckable(true);
	button->setImage(icon_off);
	button->setPressedImage(icon_on);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->addWidget(button);
	l->addWidget(lbl);

	setBeep(status, false);
	connect(button, SIGNAL(clicked()), SLOT(toggleBeep()));
}

void ToggleBeep::toggleBeep()
{
	if (getBeep())
	{
		setBeep(false, true);
		button->setStatus(false);
	}
	else
	{
		setBeep(true, true);
		button->setStatus(true);
		beep();
	}
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
	QObject::connect(this, SIGNAL(Closed()), SLOT(stopRingtones()));
}

void RingtonesPage::stopRingtones()
{
	bt_global::ringtones->stopRingtone();
}


VolumePage::VolumePage(const QDomNode &config_node)
{
	// TODO: is this text ok for the banner? Should it be read from conf?
	ItemTuning *volume = new ItemTuning(tr("Volume"), bt_global::skin->getImage("volume"));
	NavigationBar *nav_bar = new NavigationBar;
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	buildPage(volume, nav_bar, getTextChild(config_node, "descr"));
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
	text_area->addRow(tr("Model"), bt_global::config[MODEL]);
	text_area->addRow(tr("Firmware version"), "");
	text_area->addRow(tr("Kernel version"), "");
	text_area->addRow(tr("IP address"), "");
	text_area->addRow(tr("Netmask"), "");

	buildPage(text_area, nav_bar, getTextChild(config_node, "descr"));
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
			qDebug() << "LUCA Kernel version: " << it.value().toString();
			break;
		case PlatformDevice::DIM_FW_VERS:
			text_area->setText(FW_ROW, it.value().toString());
			qDebug() << "LUCA Firmware version: " << it.value().toString();
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
	PageSetDateTime(ok_button_icon)
{
	connect(this, SIGNAL(dateTimeSelected(QDate,BtTime)), SLOT(dateTimeChanged(QDate, BtTime)));
	connect(this, SIGNAL(dateTimeSelected(QDate,BtTime)), SIGNAL(Closed()));
	dev = bt_global::add_device_to_cache(new PlatformDevice);
}

void ChangeDateTime::dateTimeChanged(QDate date, BtTime time)
{
	dev->setTime(time);
	dev->setDate(date);
}




IconSettings::IconSettings(const QDomNode &config_node)
{
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	loadItems(config_node);
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
	case RINGTONE:
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
			p = new ScreenSaverPage;
			break;
		case PAGE_CLEANSCREEN:
			// TODO config file does not contain the clean screen value
			w = new CleanScreen(bt_global::skin->getImage("cleanscreen"), 10);
			connect(w, SIGNAL(Closed()), bt_global::btmain->homeWindow(), SLOT(showWindow()));
			break;
		case PAGE_LANSETTINGS:
			p = new LanSettings(item);
			break;
		case BEEP_ICON:
		{
			QWidget *t = new ToggleBeep(false, descr,
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
	buildPage(descr);

	banner *b = new impPassword(0, bt_global::skin->getImage("state_on"),
		bt_global::skin->getImage("state_off"), bt_global::skin->getImage("edit"), descr,
		getTextChild(config_node, "password"), getTextChild(config_node, "activated").toInt());
	connect(b, SIGNAL(pageClosed()), SLOT(showPage()));
	page_content->appendBanner(b);
}
