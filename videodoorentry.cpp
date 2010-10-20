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
#include "main.h" // bt_global::config, VIDEODOORENTRY
#include "skinmanager.h"
#include "btbutton.h"
#include "navigation_bar.h"
#include "videodoorentry_device.h"
#include "devices_cache.h" // bt_global::add_device_to_cache
#ifdef LAYOUT_TS_10
#include "vctcall.h"
#endif
#include "generic_functions.h" //getBostikName
#include "items.h" // ItemTuning
#include "displaycontrol.h" // bt_global::display
#include "hardware_functions.h" // setVolume
#include "btmain.h" // bt_global::btmain
#include "homewindow.h" // TrayBar
#include "pagestack.h" // bt_global::page_stack
#include "state_button.h"
#include "audiostatemachine.h" // bt_global::audio_states
#include "ringtonesmanager.h" // bt_global::ringtones

#include <QGridLayout>
#include <QSignalMapper>
#include <QDebug>
#include <QLabel>
#include <QApplication> // QApplication::processEvents


enum Pages
{
	VIDEO_CONTROL_MENU = 10001,  /*!< Video control menu */
	INTERCOM_MENU = 10002,       /*!< Intercom menu */
	INTERNAL_INTERCOM = 10101,
	EXTERNAL_INTERCOM = 10102,
	GUARD_UNIT = 19004,
};

#ifdef LAYOUT_TS_10
bool VideoDoorEntry::ring_exclusion = false;
#endif


#ifdef LAYOUT_TS_3_5
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
			qFatal("Type of device not handled by VideoDoorEntry page!");
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


VideoDoorEntry::VideoDoorEntry()
{
	dev = bt_global::add_device_to_cache(new VideoDoorEntryDevice((*bt_global::config)[PI_ADDRESS],
		(*bt_global::config)[PI_MODE]));

	// These pages are showed only after the receiving of a call frame, so we
	// don't store any pointer to these. The destruction is provided by the PageContainer.
	(void) new IntercomCallPage(dev);
	(void) new VCTCallPage(dev);
}

VideoDoorEntry::VideoDoorEntry(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));

	dev = bt_global::add_device_to_cache(new VideoDoorEntryDevice((*bt_global::config)[PI_ADDRESS],
		(*bt_global::config)[PI_MODE]));

	loadItems(config_node);
}

int VideoDoorEntry::sectionId() const
{
	return VIDEODOORENTRY;
}

void VideoDoorEntry::loadItems(const QDomNode &config_node)
{
	VCTCallPage *call_page = 0;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		int id = getTextChild(item, "id").toInt();
		SkinContext cxt(getTextChild(item, "cid").toInt());
		QDomNode page_node = getPageNodeFromChildNode(item, "lnk_pageID");
		Page *p = 0;

		switch (id)
		{
		case INTERCOM_MENU:
			p = new IntercomMenu(page_node, dev);
			break;
		case VIDEO_CONTROL_MENU:
			if (!call_page)
				call_page = new VCTCallPage(dev);
			p = new VideoControl(page_node, dev);
			break;
		case GUARD_UNIT:
		{
			BtButton *btn = addButton(getTextChild(item, "descr"), bt_global::skin->getImage("link_icon"));
			connect(btn, SIGNAL(clicked()), SLOT(callGuardUnit()));
			break;
		}
		default:
			qFatal("Unhandled page id %d in VideoDoorEntry::loadItems", id);
		}

		if (p)
		{
			QString icon = bt_global::skin->getImage("link_icon");
			QString descr = getTextChild(item, "descr");
			addPage(p, descr, icon);
		}

	}
}

void VideoDoorEntry::callGuardUnit()
{
	dev->cameraOn((*bt_global::config)[GUARD_UNIT_ADDRESS]);
}

VideoControl::VideoControl(const QDomNode &config_node, VideoDoorEntryDevice *d)
{
	// we must have only one videodoor entry device since we need to remember some state
	dev = d;

	mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(QString)), SLOT(cameraOn(QString)));

	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext ctx(getTextChild(item, "cid").toInt());
		BtButton *btn = addButton(getTextChild(item, "descr"), bt_global::skin->getImage("link_icon"));
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


IntercomCallPage::IntercomCallPage(VideoDoorEntryDevice *d)
{
	dev = d;
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	already_closed = false;
	SkinContext ctx(666);

	QGridLayout *layout = new QGridLayout(this);
	layout->setContentsMargins(10, 0, 0, 15);
	layout->setSpacing(5);
	layout->setColumnStretch(1, 1);

	BtButton *back = new BtButton;
	back->setImage(bt_global::skin->getImage("back"));
	connect(back, SIGNAL(clicked()), dev, SLOT(endCall()));
	if (dev->vctMode() == VideoDoorEntryDevice::SCS_MODE)
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
	volume->disable();
	connect(volume, SIGNAL(valueChanged(int)), SLOT(changeVolume(int)));
	buttons_layout->addWidget(volume, 3, 0, 1, 2, Qt::AlignHCenter);

	layout->addLayout(buttons_layout, 0, 1, Qt::AlignHCenter);
	call_active = false;
}

int IntercomCallPage::sectionId() const
{
	return VIDEODOORENTRY;
}

void IntercomCallPage::cleanUp()
{
	// the cleanUp is performed when we exit from the page using an external
	// button. In this case, we have to send the end of call (even if is an
	// autoswitch call).
	dev->endCall();

	already_closed = true;
	if (dev->vctMode() == VideoDoorEntryDevice::IP_MODE) // See the comment on VctCallPage::cleanUp
	{
		while (call_active) // We wait for the END_OF_CALL
			QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
	}
	else
		handleClose();
}

void IntercomCallPage::showPageAfterCall()
{
	// The only difference between this method and the following is that
	// when the touch call an internal place the "accept" button should be
	// in the active status.
	bt_global::page_stack.showUserPage(this);
	bt_global::btmain->makeActive();
	bt_global::btmain->vde_call_active = true;
	call_accept->setStatus(true);
	mute_button->setStatus(StateButton::DISABLED);
	showPage();
}

void IntercomCallPage::showPageIncomingCall()
{
	disconnect(bt_global::display, SIGNAL(directScreenAccessStopped()), this, SLOT(showPageIncomingCall()));
	if (bt_global::display->isDirectScreenAccess())
	{
		connect(bt_global::display, SIGNAL(directScreenAccessStopped()), SLOT(showPageIncomingCall()));
		return;
	}

	bt_global::page_stack.showUserPage(this);
	bt_global::btmain->makeActive();
	bt_global::btmain->vde_call_active = true;
	call_accept->setStatus(false);
	mute_button->setStatus(StateButton::DISABLED);
	showPage();
}

void IntercomCallPage::handleClose()
{
	bt_global::btmain->vde_call_active = false;
	volume->disable();

	disconnect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), this, SLOT(playRingtone()));
	disconnect(bt_global::display, SIGNAL(directScreenAccessStopped()), this, SLOT(showPage()));

	if (bt_global::audio_states->contains(AudioStates::MUTE))
		bt_global::audio_states->removeState(AudioStates::MUTE);

	if (bt_global::audio_states->contains(AudioStates::SCS_INTERCOM_CALL))
		bt_global::audio_states->removeState(AudioStates::SCS_INTERCOM_CALL);
	else if (bt_global::audio_states->contains(AudioStates::IP_INTERCOM_CALL))
		bt_global::audio_states->removeState(AudioStates::IP_INTERCOM_CALL);

	if (bt_global::audio_states->contains(AudioStates::PLAY_VDE_RINGTONE))
	{
		bt_global::audio_states->removeState(AudioStates::PLAY_VDE_RINGTONE);
		bt_global::ringtones->stopRingtone();
	}

	if (!already_closed)
		emit Closed();

	already_closed = false;
}

void IntercomCallPage::toggleCall()
{
	bool connected = call_accept->getStatus();

	if (connected)
	{
		dev->endCall();
		if (dev->vctMode() == VideoDoorEntryDevice::SCS_MODE)
			handleClose();
	}
	else
	{
		call_accept->setStatus(!connected);
		dev->answerCall();
		if (dev->ipCall())
			bt_global::audio_states->toState(AudioStates::IP_INTERCOM_CALL);
		else
			bt_global::audio_states->toState(AudioStates::SCS_INTERCOM_CALL);
		mute_button->setStatus(StateButton::OFF);
		volume->enable();
	}
}

void IntercomCallPage::toggleMute()
{
	StateButton::Status st = mute_button->getStatus();

	if (st == StateButton::ON)
	{
		mute_button->setStatus(StateButton::OFF);
		bt_global::audio_states->removeState(AudioStates::MUTE);
		volume->enable();
	}
	else
	{
		mute_button->setStatus(StateButton::ON);
		bt_global::audio_states->toState(AudioStates::MUTE);
		volume->disable();
	}
}

void IntercomCallPage::changeVolume(int value)
{
	bt_global::audio_states->setVolume(value);
}

void IntercomCallPage::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case VideoDoorEntryDevice::INTERCOM_CALL:
			call_active = true;
			showPageIncomingCall();
			break;
		case VideoDoorEntryDevice::RINGTONE:
		{
			bool ring_exclusion = VideoDoorEntry::ring_exclusion;

			ringtone = static_cast<Ringtones::Type>(values_list[VideoDoorEntryDevice::RINGTONE].toInt());
			if (ringtone == Ringtones::PI_INTERCOM || ringtone == Ringtones::PE_INTERCOM)
			{
				if (!ring_exclusion)
					connect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), SLOT(playRingtone()));
				if (bt_global::audio_states->currentState() != AudioStates::PLAY_VDE_RINGTONE)
					bt_global::audio_states->toState(AudioStates::PLAY_VDE_RINGTONE);
				else if (!ring_exclusion)
					playRingtone();
			}
			else if (ringtone == Ringtones::FLOORCALL && !ring_exclusion)
			{
				if (bt_global::audio_states->currentState() != AudioStates::PLAY_FLOORCALL)
				{
					connect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), SLOT(playRingtone()));
					bt_global::audio_states->toState(AudioStates::PLAY_FLOORCALL);
				}
				else
					playRingtone();
			}
			break;
		}
		case VideoDoorEntryDevice::ANSWER_CALL:
			if (!call_active)
			{
				call_active = true;
				if (dev->ipCall())
					bt_global::audio_states->toState(AudioStates::IP_INTERCOM_CALL);
				else
					bt_global::audio_states->toState(AudioStates::SCS_INTERCOM_CALL);
				mute_button->setStatus(StateButton::OFF);
				volume->enable();
			}
			break;
		case VideoDoorEntryDevice::END_OF_CALL:
			if (call_active)
			{
				call_active = false;
				handleClose();
				// Reset the timers for the freeze/screensaver.
				bt_global::btmain->makeActive();
			}
			break;
		}
		++it;
	}
}

void IntercomCallPage::playRingtone()
{
	disconnect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), this, SLOT(playRingtone()));
	Ringtones::Type ring = static_cast<Ringtones::Type>(ringtone);
	if (ring == Ringtones::FLOORCALL)
		connect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(floorCallFinished()));

	bt_global::ringtones->playRingtone(ring);
}

void IntercomCallPage::floorCallFinished()
{
	bt_global::audio_states->removeState(AudioStates::PLAY_FLOORCALL);
	disconnect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(floorCallFinished()));
}


IntercomMenu::IntercomMenu(const QDomNode &config_node, VideoDoorEntryDevice *dev)
{
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
		QDomNode addresses = getElement(item, "addresses");
		QString where = getTextChild(addresses, "dev") + getTextChild(addresses, "where");

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
		connect(btn, SIGNAL(clicked()), call_page, SLOT(showPageAfterCall()));
	}
}


HandsFree::HandsFree(bool status, int item_id) : IconButtonOnTray(tr("Hands Free"),
	"handsfree_on", "handsfree_off", "tray_handsfree", TrayBar::HANDS_FREE, status, item_id)
{
	updateStatus();
}

void HandsFree::updateStatus()
{
	IconButtonOnTray::updateStatus();
	VCTCallPage::setHandsFree(button->getStatus() == StateButton::ON);
}


ProfessionalStudio::ProfessionalStudio(bool status, int item_id) : IconButtonOnTray(tr("Professional studio"),
	"profstudio_on", "profstudio_off", "tray_profstudio", TrayBar::PROF_STUDIO, status, item_id)
{
	updateStatus();
}

void ProfessionalStudio::updateStatus()
{
	IconButtonOnTray::updateStatus();
	VCTCallPage::setProfStudio(button->getStatus() == StateButton::ON);
}


RingtoneExclusion::RingtoneExclusion(bool status, int item_id) : IconButtonOnTray(tr("Ringtone Exclusion"),
	"ringexclusion_on", "ringexclusion_off", "tray_ringexclusion", TrayBar::RING_EXCLUSION, status, item_id)
{
	updateStatus();
}

void RingtoneExclusion::updateStatus()
{
	IconButtonOnTray::updateStatus();
	VideoDoorEntry::ring_exclusion = button->getStatus() == StateButton::ON;
}

#endif
