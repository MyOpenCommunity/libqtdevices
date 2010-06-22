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
#include "entryphone_device.h"
#include "devices_cache.h" // bt_global::add_device_to_cache
#ifdef LAYOUT_TOUCHX
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
	EntryphoneDevice *dev = bt_global::add_device_to_cache(new EntryphoneDevice((*bt_global::config)[PI_ADDRESS],
		(*bt_global::config)[PI_MODE]));

	// These pages are showed only after the receiving of a call frame, so we
	// don't store any pointer to these. The destruction is provided by the PageContainer.
	(void) new IntercomCallPage(dev);
	(void) new VCTCallPage(dev);

	ring_exclusion = new StateButton;
	ring_exclusion->setOnOff();
	ring_exclusion->setOffImage(bt_global::skin->getImage("tray_ring_ex_off"));
	ring_exclusion->setOnImage(bt_global::skin->getImage("tray_ring_ex_on"));
	connect(ring_exclusion, SIGNAL(clicked()), SLOT(toggleRingExclusion()));
	bt_global::btmain->trayBar()->addButton(ring_exclusion, TrayBar::RING_EXCLUSION);
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
	bt_global::btmain->trayBar()->addButton(ring_exclusion, TrayBar::RING_EXCLUSION);

	loadItems(config_node);
}

void VideoDoorEntry::toggleRingExclusion()
{
	ring_exclusion->setStatus(!ring_exclusion->getStatus());
}

int VideoDoorEntry::sectionId() const
{
	return VIDEODOORENTRY;
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
	dev = bt_global::add_device_to_cache(new EntryphoneDevice((*bt_global::config)[PI_ADDRESS],
		(*bt_global::config)[PI_MODE]));

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
	bt_global::btmain->vde_call_active = false;

	disconnect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), this, SLOT(playRingtone()));
	disconnect(bt_global::display, SIGNAL(directScreenAccessStopped()), this, SLOT(showPage()));

	if (bt_global::audio_states->contains(AudioStates::MUTE))
		bt_global::audio_states->removeState(AudioStates::MUTE);

	if (bt_global::audio_states->contains(AudioStates::SCS_INTERCOM_CALL))
	{
		bt_global::audio_states->removeState(AudioStates::SCS_INTERCOM_CALL);
		volume->disable();
	}

	if (bt_global::audio_states->contains(AudioStates::PLAY_VDE_RINGTONE))
	{
		bt_global::audio_states->removeState(AudioStates::PLAY_VDE_RINGTONE);
		bt_global::ringtones->stopRingtone();
	}

}

void IntercomCallPage::showPageAfterCall()
{
	// The only difference between this method and the following is that
	// when the touch call an internal place the "accept" button should be
	// in the active status.
	bt_global::page_stack.showVCTPage(this);
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

	bt_global::page_stack.showVCTPage(this);
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
	{
		bt_global::audio_states->removeState(AudioStates::SCS_INTERCOM_CALL);
		volume->disable();
	}

	if (bt_global::audio_states->contains(AudioStates::PLAY_VDE_RINGTONE))
	{
		bt_global::audio_states->removeState(AudioStates::PLAY_VDE_RINGTONE);
		bt_global::ringtones->stopRingtone();
	}
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
		case EntryphoneDevice::INTERCOM_CALL:
			call_active = true;
			showPageIncomingCall();
			break;
		case EntryphoneDevice::RINGTONE:
		{
			StateButton *ring_exclusion = qobject_cast<StateButton*>(bt_global::btmain->trayBar()->getButton(TrayBar::RING_EXCLUSION));

			ringtone = static_cast<Ringtones::Type>(values_list[EntryphoneDevice::RINGTONE].toInt());
			if (ringtone == Ringtones::PI_INTERCOM || ringtone == Ringtones::PE_INTERCOM)
			{
				if (!ring_exclusion || !ring_exclusion->getStatus())
					connect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), SLOT(playRingtone()));
				bt_global::audio_states->toState(AudioStates::PLAY_VDE_RINGTONE);
			}
			break;
		}
		case EntryphoneDevice::ANSWER_CALL:
			if (!call_active)
			{
				call_active = true;
				bt_global::audio_states->toState(AudioStates::SCS_INTERCOM_CALL);
				mute_button->setStatus(StateButton::OFF);
				volume->enable();
			}
			break;
		case EntryphoneDevice::END_OF_CALL:
			if (call_active)
			{
				call_active = false;
				handleClose();
			}
			break;
		}
		++it;
	}
}

void IntercomCallPage::playRingtone()
{
	disconnect(bt_global::audio_states, SIGNAL(directAudioAccessStopped()),	this, SLOT(playRingtone()));
	bt_global::ringtones->playRingtone(static_cast<Ringtones::Type>(ringtone));
}


Intercom::Intercom(const QDomNode &config_node)
{
	EntryphoneDevice *dev = bt_global::add_device_to_cache(new EntryphoneDevice((*bt_global::config)[PI_ADDRESS],
		(*bt_global::config)[PI_MODE]));

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


HandsFree::HandsFree() : IconButtonOnTray(tr("Hands Free"),
	"handsfree_on", "handsfree_off", "tray_handsfree", TrayBar::HANDS_FREE)
{
}

void HandsFree::updateStatus()
{
	IconButtonOnTray::updateStatus();
	VCTCallPage::setHandsFree(button->getStatus() == StateButton::ON);
}


ProfessionalStudio::ProfessionalStudio() : IconButtonOnTray(tr("Professional studio"),
	"profstudio_on", "profstudio_off", "tray_profstudio", TrayBar::PROF_STUDIO)
{
}

void ProfessionalStudio::updateStatus()
{
	IconButtonOnTray::updateStatus();
	VCTCallPage::setProfStudio(button->getStatus() == StateButton::ON);
}


#endif
