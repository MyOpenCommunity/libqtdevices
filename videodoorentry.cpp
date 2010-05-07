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


#include "videodoorentry.h"
#include "bann_videodoorentry.h"
#include "bann2_buttons.h"
#include "xml_functions.h" // getTextChild, getChildren
#include "main.h" // bt_global::config
#include "skinmanager.h"
#include "btbutton.h"
#include "navigation_bar.h"
#include "entryphone_device.h"
#include "devices_cache.h" // bt_global::add_device_to_cache
#ifdef LAYOUT_TOUCHX
#include "vctcall.h"
#endif
#include "generic_functions.h" //getBostikName
#include "items.h" // ItemTuning
#include "displaycontrol.h" // (*bt_global::display)
#include "hardware_functions.h" // setVolume
#include "ringtonesmanager.h" // bt_global::ringtones
#include "btmain.h" // bt_global::btmain
#include "homewindow.h" // TrayBar
#include "pagestack.h" // bt_global::page_stack
#include "state_button.h"

#include <QGridLayout>
#include <QSignalMapper>
#include <QDebug>
#include <QLabel>


enum Pages
{
	VIDEO_CONTROL_MENU = 10001,  /*!< Video control menu */
	INTERCOM_MENU = 10002,       /*!< Intercom menu */
	INTERNAL_INTERCOM = 10101,
	EXTERNAL_INTERCOM = 10102
};


#ifdef LAYOUT_BTOUCH
VideoDoorEntry::VideoDoorEntry(const QDomNode &config_node)
{
	buildPage();
	loadDevices(config_node);
}

void VideoDoorEntry::loadDevices(const QDomNode &config_node)
{
	QString unknown = getTextChild(config_node, "unknown");

	foreach (const QDomNode &device, getChildren(config_node, "device"))
	{
		int id = getTextChild(device, "id").toInt();
		if (id != POSTO_ESTERNO)
			qFatal("Type of device not handled by Video EntryPhone page!");
		QString img1 = IMG_PATH + getTextChild(device, "cimg1");
		QString img2 = IMG_PATH + getTextChild(device, "cimg2");
		QString img3 = IMG_PATH + getTextChild(device, "cimg3");
		QString img4 = IMG_PATH + getTextChild(device, "cimg4");
		QString descr = getTextChild(device, "descr");
		QString light = getTextChild(device, "light");
		QString key = getTextChild(device, "key");
		QString where = getTextChild(device, "where");

		banner *b = new postoExt(this, descr, img1, img2, img3, img4, where, light, key, unknown);
		b->setText(descr);
		b->setId(id);
		b->Draw();
		page_content->appendBanner(b);
	}
}
#else


void VideoDoorEntry::loadHiddenPages()
{
	EntryphoneDevice *dev = bt_global::add_device_to_cache(new EntryphoneDevice((*bt_global::config)[PI_ADDRESS]));

	// These pages are showed only after the receiving of a call frame, so we
	// don't store any pointer to these. The destruction is provided by the PageContainer.
	(void) new IntercomCallPage(dev);
	(void) new VCTCallPage(dev);
}

VideoDoorEntry::VideoDoorEntry(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));

	ring_exclusion = new StateButton;
	ring_exclusion->setOnOff();
	ring_exclusion->setOffImage(bt_global::skin->getImage("tray_ring_ex_off"));
	ring_exclusion->setOnImage(bt_global::skin->getImage("tray_ring_ex_on"));
	connect(ring_exclusion, SIGNAL(clicked()), SLOT(toggleRingExclusion()));
	bt_global::btmain->trayBar()->addButton(ring_exclusion);

	loadItems(config_node);
	dev = bt_global::add_device_to_cache(new EntryphoneDevice((*bt_global::config)[PI_ADDRESS]));
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void VideoDoorEntry::toggleRingExclusion()
{
	ring_exclusion->setStatus(!ring_exclusion->getStatus());
}

void VideoDoorEntry::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case EntryphoneDevice::RINGTONE:
			Ringtones::Type ringtone = static_cast<Ringtones::Type>(it.value().toInt());
			if (!ring_exclusion->getStatus())
				bt_global::ringtones->playRingtone(ringtone);

			break;
		}
		++it;
	}
}

int VideoDoorEntry::sectionId() const
{
	return VIDEOCITOFONIA;
}

void VideoDoorEntry::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		Page *p = 0;

		switch (id)
		{
		case INTERCOM_MENU:
			p = new Intercom(page_node);
			break;
		case VIDEO_CONTROL_MENU:
			p = new VideoControl(page_node);
			break;
		default:
			qFatal("Unhandled page id %d in VideoDoorEntry::loadItems", id);
		};

		if (p)
		{
			QString icon = bt_global::skin->getImage("link_icon");
			QString descr = getTextChild(item, "descr");
			addPage(p, descr, icon);
		}

	}
}


VideoControl::VideoControl(const QDomNode &config_node)
{
	// we must have only one entryphone device since we need to remember some state
	dev = bt_global::add_device_to_cache(new EntryphoneDevice((*bt_global::config)[PI_ADDRESS]));

	mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(QString)), SLOT(cameraOn(QString)));

	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	call_page = new VCTCallPage(dev);
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext ctx(getTextChild(item, "cid").toInt());
		BtButton *btn = addButton(getTextChild(item, "descr"), bt_global::skin->getImage("link_icon"), 0, 0);
		connect(btn, SIGNAL(clicked()), mapper, SLOT(map()));

		QDomNode addresses = getElement(item, "addresses");
		QString where = getTextChild(addresses, "dev") + getTextChild(addresses, "where");
		Q_ASSERT_X(!where.isEmpty(), "VideoControl::VideoControl", "empty where in configuration");
		mapper->setMapping(btn, where);
	}
}

void VideoControl::cameraOn(QString where)
{
	dev->cameraOn(where);
}


IntercomCallPage::IntercomCallPage(EntryphoneDevice *d)
{
	dev = d;
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	SkinContext ctx(666);

	QGridLayout *layout = new QGridLayout(this);
	layout->setContentsMargins(10, 0, 0, 15);
	layout->setSpacing(5);
	layout->setColumnStretch(1, 1);

	BtButton *back = new BtButton;
	back->setImage(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), dev, SLOT(endCall()));
	connect(back, SIGNAL(clicked()), SLOT(handleClose()));
	layout->addWidget(back, 0, 0, 1, 1, Qt::AlignBottom);

	QGridLayout *buttons_layout = new QGridLayout;
	buttons_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding), 0, 0, 1, 1);
	buttons_layout->setContentsMargins(0, 0, 0, 0);
	buttons_layout->setSpacing(15);

	QLabel *call_image = new QLabel;
	call_image->setPixmap(bt_global::skin->getImage("connection"));
	buttons_layout->addWidget(call_image, 1, 0, 1, 2);

	QString call_icon = bt_global::skin->getImage("call");
	call_accept = new StateButton;
	call_accept->setOnOff();
	call_accept->setOffImage(getBostikName(call_icon, "off"));
	call_accept->setOnImage(getBostikName(call_icon, "on"));
	connect(call_accept, SIGNAL(clicked()), SLOT(toggleCall()));
	buttons_layout->addWidget(call_accept, 2, 0);

	QString mute_icon = bt_global::skin->getImage("mute");
	mute_button = new StateButton;
	mute_button->setOffImage(getBostikName(mute_icon, "off"));
	mute_button->setDisabledImage(getBostikName(getBostikName(mute_icon, "off"), "dis"));
	mute_button->setOnImage(getBostikName(mute_icon, "on"));
	mute_button->setStatus(StateButton::DISABLED);
	connect(mute_button, SIGNAL(clicked()), SLOT(toggleMute()));
	buttons_layout->addWidget(mute_button, 2, 1);

	volume = new ItemTuning("", bt_global::skin->getImage("volume"));
	connect(volume, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));
	buttons_layout->addWidget(volume, 3, 0, 1, 2, Qt::AlignHCenter);

	layout->addLayout(buttons_layout, 0, 1, Qt::AlignHCenter);
}

int IntercomCallPage::sectionId() const
{
	return VIDEOCITOFONIA;
}

void IntercomCallPage::showPage()
{
	bt_global::page_stack.showVCTPage(this);
	(*bt_global::display).forceOperativeMode(true);
	call_accept->setStatus(true);
	mute_button->setStatus(StateButton::DISABLED);
	Page::showPage();
}

void IntercomCallPage::showPageIncomingCall()
{
	bt_global::page_stack.showVCTPage(this);

	if (!BtMain::isCalibrating())
	{
		if ((*bt_global::display).currentState() != DISPLAY_FREEZED)
			(*bt_global::display).forceOperativeMode(true);
		call_accept->setStatus(false);
		mute_button->setStatus(StateButton::OFF);
	}

	Page::showPage();
}

void IntercomCallPage::handleClose()
{
	(*bt_global::display).forceOperativeMode(false);
	emit Closed();
}

void IntercomCallPage::toggleCall()
{
	bool connected = call_accept->getStatus();
	call_accept->setStatus(!connected);

	if (connected)
	{
		mute_button->setStatus(StateButton::DISABLED);
		dev->endCall();
		handleClose();
	}
	else
	{
		dev->answerCall();
		mute_button->setStatus(StateButton::OFF);
	}
}

void IntercomCallPage::toggleMute()
{
	StateButton::Status st = mute_button->getStatus();
	setVolume(VOLUME_MIC, st == StateButton::ON ? 0 : 1);

	if (st == StateButton::ON)
		mute_button->setStatus(StateButton::OFF);
	else
		mute_button->setStatus(StateButton::ON);
}

void IntercomCallPage::changeVolume(int value)
{
	setVolume(VOLUME_VIDEOCONTROL, value);
}

void IntercomCallPage::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case EntryphoneDevice::INTERCOM_CALL:
			showPageIncomingCall();
			break;
		case EntryphoneDevice::END_OF_CALL:
			handleClose();
			break;
		}
		++it;
	}
}


Intercom::Intercom(const QDomNode &config_node)
{
	EntryphoneDevice *dev = bt_global::add_device_to_cache(new EntryphoneDevice((*bt_global::config)[PI_ADDRESS]));

	mapper_int_intercom = new QSignalMapper(this);
	mapper_ext_intercom = new QSignalMapper(this);
	connect(mapper_int_intercom, SIGNAL(mapped(QString)), dev, SLOT(internalIntercomCall(QString)));
	connect(mapper_ext_intercom, SIGNAL(mapped(QString)), dev, SLOT(externalIntercomCall(QString)));

	IntercomCallPage *call_page = new IntercomCallPage(dev);

	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext ctx(getTextChild(item, "cid").toInt());
		BtButton *btn = addButton(getTextChild(item, "descr"), bt_global::skin->getImage("link_icon"), 0, 0);

		int id = getTextChild(item, "id").toInt();
		QString where = getTextChild(item, "dev") + getTextChild(item, "where");

		if (id == INTERNAL_INTERCOM)
		{
			mapper_int_intercom->setMapping(btn, where);
			connect(btn, SIGNAL(clicked()), mapper_int_intercom, SLOT(map()));
		}
		else
		{
			mapper_ext_intercom->setMapping(btn, where);
			connect(btn, SIGNAL(clicked()), mapper_ext_intercom, SLOT(map()));
		}
		connect(btn, SIGNAL(clicked()), call_page, SLOT(showPage()));
	}
}


HandsFree::HandsFree() : IconButtonOnTray(tr("Hands Free"),
	"handsfree_on", "handsfree_off", "tray_handsfree")
{
}

void HandsFree::updateStatus()
{
	IconButtonOnTray::updateStatus();
	VCTCallPage::setHandsFree(button->getStatus() == StateButton::ON);
}


ProfessionalStudio::ProfessionalStudio() : IconButtonOnTray(tr("Professional studio"),
	"profstudio_on", "profstudio_off", "tray_profstudio")
{
}

void ProfessionalStudio::updateStatus()
{
	IconButtonOnTray::updateStatus();
	VCTCallPage::setProfStudio(button->getStatus() == StateButton::ON);
}


SettingsVideoDoorEntry::SettingsVideoDoorEntry()
{
	buildPage(new IconContent, new NavigationBar, "Video Door Entry");

	page_content->addWidget(new HandsFree);
	page_content->addWidget(new ProfessionalStudio);
}

#endif
