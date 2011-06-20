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
#include "generic_functions.h" //getBostikName, silentExecute
#include "items.h" // ItemTuning
#include "displaycontrol.h" // bt_global::display
#include "hardware_functions.h" // setVolume
#include "btmain.h" // bt_global::status
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
#include <QtConcurrentRun>

#include <fcntl.h> // open


// The address used to save information in the e2 memory.
#define E2_BASE_CONF_ZARLINK 11694

// The keys used to update the e2, the eeprom memory where is stored the zarlink
// configuration.
#define ZARLINK_KEY_SCS 0x72
#define ZARLINK_KEY_IP 0x82

// The executable of zarlink (the echo canceller)
#define ZARLINK_BIN "/home/bticino/bin/zarlink"

// Another executable of zarlink, used in a pipeline to adjust the volume of the call.
#define ZARLINK_VOLUME  "/home/bticino/bin/zarlink2"


#define CALL_NOTIFIER_TIMEOUT 30000


enum Items
{
	ENTRANCE_PANEL = 49, // to be changed
};

enum Pages
{
	VIDEO_CONTROL_MENU = 10001,  /* Video control menu */
	INTERCOM_MENU = 10002,       /* Intercom menu */
	INTERNAL_INTERCOM = 10101,
	EXTERNAL_INTERCOM = 10102,
	GUARD_UNIT = 19004,
};

enum ItemsSettings
{
	ITEM_HANDSFREE = 14251,
	ITEM_PROF_STUDIO = 14252,
	ITEM_RING_EXCLUSION = 14253,
};

#ifdef LAYOUT_TS_10
bool VideoDoorEntry::ring_exclusion = false;
#endif


#ifdef LAYOUT_TS_3_5

VideoDoorEntry::VideoDoorEntry(const QDomNode &config_node)
{
	dev = bt_global::add_device_to_cache(new BasicVideoDoorEntryDevice((*bt_global::config)[PI_ADDRESS],
		(*bt_global::config)[PI_MODE]));
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	buildPage();
	loadItems(config_node);
}

void VideoDoorEntry::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item_node, getChildren(config_node, "item"))
	{
		SkinContext ctx(getTextChild(item_node, "cid").toInt());
		int id = getTextChild(item_node, "id").toInt();
		if (id != ENTRANCE_PANEL)
		{
			qWarning() << "Unknown item id" << id << "for VideoDoorEntry";
			continue;
		}

		QString descr = getTextChild(item_node, "descr");
		bool light = getTextChild(item_node, "light").toInt() == 1;
		bool key = getTextChild(item_node, "key").toInt() == 1;
		QString where = getTextChild(item_node, "where");

		EntrancePanel *b = new EntrancePanel(descr, where, light, key);
		CallNotifierPage *p = new CallNotifierPage(descr, where, light, key);

		foreach (QObject *obj, QList<QObject*>() << b << p)
		{
			connect(obj, SIGNAL(stairLightActivate(QString)), dev, SLOT(stairLightActivate(QString)));
			connect(obj, SIGNAL(stairLightRelease(QString)), dev, SLOT(stairLightRelease(QString)));
			connect(obj, SIGNAL(openLock(QString)), dev, SLOT(openLock(QString)));
			connect(obj, SIGNAL(releaseLock(QString)), dev, SLOT(releaseLock(QString)));
		}

		popup_pages[where] = p;
		page_content->appendBanner(b);
	}

	// The popup-page for unknown callers.
	popup_pages[QString()] = new CallNotifierPage(tr("Unknown"), QString(), false, false);
}

void VideoDoorEntry::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		if (it.key() == VideoDoorEntryDevice::CALLER_ADDRESS)
		{
			QString where = it.value().toString();
			if (!popup_pages.contains(where))
				where = QString();

			bt_global::display->makeActive();
			bt_global::page_stack.showUserPage(popup_pages[where]);
			popup_pages[where]->showPage();
		}
		++it;
	}
}


CallNotifierPage::CallNotifierPage(QString descr, QString _where, bool light, bool key)
{
	where = _where;
	timer = new QTimer(this);
	timer->setSingleShot(true);
	timer->setInterval(CALL_NOTIFIER_TIMEOUT);
	connect(timer, SIGNAL(timeout()), SIGNAL(Closed()));

	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);
	main_layout->setSpacing(20);
	main_layout->setContentsMargins(0, 35, 0, 0);

	QLabel *label = new QLabel(descr);
	label->setFont(bt_global::font->get(FontManager::TEXT));
	label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	main_layout->addWidget(label);

	if (light)
	{
		Bann2Buttons *b = new Bann2Buttons;
		b->initBanner(bt_global::skin->getImage("entrance_panel_light"), QString(), tr("Staircase light"));
		connect(b, SIGNAL(leftReleased()), timer, SLOT(start()));
		connect(b, SIGNAL(leftPressed()), SLOT(lightPressed()));
		connect(b, SIGNAL(leftReleased()), SLOT(lightReleased()));
		main_layout->addWidget(b);
	}

	if (key)
	{
		Bann2Buttons *b = new Bann2Buttons;
		b->initBanner(bt_global::skin->getImage("entrance_panel_key"), QString(), tr("Door lock"));
		connect(b, SIGNAL(leftReleased()), timer, SLOT(start()));
		connect(b, SIGNAL(leftPressed()), SLOT(lockPressed()));
		connect(b, SIGNAL(leftReleased()), SLOT(lockReleased()));
		main_layout->addWidget(b);
	}

	main_layout->addStretch();

	NavigationBar *nav_bar = new NavigationBar;
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	nav_bar->displayScrollButtons(false);
	buildPage(content, nav_bar);
}

void CallNotifierPage::showPage()
{
	timer->start();
	Page::showPage();
}

void CallNotifierPage::lightPressed()
{
	emit stairLightActivate(where);
}

void CallNotifierPage::lightReleased()
{
	emit stairLightRelease(where);
}

void CallNotifierPage::lockPressed()
{
	emit openLock(where);
}

void CallNotifierPage::lockReleased()
{
	emit releaseLock(where);
}

#else

VideoDoorEntry::VideoDoorEntry()
{
	createVctElements();
}

VideoDoorEntry::VideoDoorEntry(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	buildPage(new IconContent, new NavigationBar, getTextChild(config_node, "descr"));
	createVctElements();

	loadItems(config_node);
}

void VideoDoorEntry::createVctElements()
{
	// We create all the stuff required to answer the incoming video/intercom calls.
	dev = bt_global::add_device_to_cache(new VideoDoorEntryDevice((*bt_global::config)[PI_ADDRESS],
		(*bt_global::config)[PI_MODE]));

	(void)new EchoCanceller(dev->vctMode());

	intercom_page = new IntercomCallPage(dev);
	vct_page = new VCTCallPage(dev);
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
			p = new IntercomMenu(page_node, dev, intercom_page);
			break;
		case VIDEO_CONTROL_MENU:
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


EchoCanceller::EchoCanceller(VideoDoorEntryDevice::VctMode mode)
{
#ifdef BT_HARDWARE_TS_10
	if (mode == VideoDoorEntryDevice::SCS_MODE)
		QtConcurrent::run(EchoCanceller::initScs);
	else
		QtConcurrent::run(EchoCanceller::initIp);
#endif
}

// Init the echo canceller, updating (if needed) the configuration. We call in a separate
// thread, in order to avoid the freeze of the ui.
void EchoCanceller::initScs()
{
	qDebug() << "EchoCanceller::initScs";
	unsigned char init = 0;
	bool need_reset = false;

	int eeprom = open(DEV_E2, O_RDWR | O_SYNC, 0666);
	if (eeprom == -1)
	{
		qWarning() << "Unable to open E2 device";
		return;
	}
	char read_value = '\0';
	while (1)
	{
		lseek(eeprom, E2_BASE_CONF_ZARLINK, SEEK_SET);
		read(eeprom, &init, 1);

		if (read_value == init)
			break;
		read_value = init;
	}

	if (init != ZARLINK_KEY_SCS) // different versions, update the zarlink configuration
	{
		while (1)
		{
			int dummy = system(ZARLINK_BIN " 0400 0001 CONF /home/bticino/cfg/zle38004_scs.cr > /dev/null 2>&1");
			if (dummy == 0)
			{
				init = ZARLINK_KEY_SCS;
				lseek(eeprom, E2_BASE_CONF_ZARLINK, SEEK_SET);
				write(eeprom, &init, 1);
				need_reset = true;
				break;
			}
			// We don't care about blocking the UI, because we are in a separate thread.
			usleep(1500000);
		}
	}

	silentExecute(QString("echo %1 > /home/bticino/cfg/vers_conf_zarlink").arg(init));
	if (need_reset)
	{
		silentExecute("echo 0 > /proc/sys/dev/btweb/reset_ZL1");
		usleep(100000);
		silentExecute("echo 1 > /proc/sys/dev/btweb/reset_ZL1");
	}
}

void EchoCanceller::initIp()
{
	qDebug() << "EchoCanceller::initIp";
	unsigned char init = 0;
	bool need_reset = false;

	system("echo 1 > /proc/sys/dev/btweb/reset_ZL2");
	usleep(200000);

	int eeprom = open(DEV_E2, O_RDWR | O_SYNC, 0666);
	if (eeprom == -1)
	{
		qWarning() << "Unable to open E2 device";
		return;
	}
	char read_value = '\0';
	while (1)
	{
		lseek(eeprom, E2_BASE_CONF_ZARLINK, SEEK_SET);
		read(eeprom, &init, 1);

		if (read_value == init)
			break;
		read_value = init;
	}

	if (init != ZARLINK_KEY_IP) // different versions, update the zarlink configuration
	{
		while (1)
		{
			int dummy = system(ZARLINK_BIN " 0400 0001 CONF /home/bticino/cfg/zle38004_ip.cr > /dev/null 2>&1");
			if (dummy == 0)
			{
				init = ZARLINK_KEY_IP;
				lseek(eeprom, E2_BASE_CONF_ZARLINK, SEEK_SET);
				write(eeprom, &init, 1);
				need_reset = true;
				break;
			}
			// We don't care about blocking the UI, because we are in a separate thread.
			usleep(1500000);
		}
	}

	silentExecute(QString("echo %1 > /home/bticino/cfg/vers_conf_zarlink").arg(init));
	if (need_reset)
	{
		silentExecute("echo 0 > /proc/sys/dev/btweb/reset_ZL1");
		usleep(100000);
		silentExecute("echo 1 > /proc/sys/dev/btweb/reset_ZL1");
		usleep(100000);
		silentExecute("echo 0 > /proc/sys/dev/btweb/reset_ZL2");
		usleep(100000);
		silentExecute("echo 1 > /proc/sys/dev/btweb/reset_ZL2");
	}

	// In IP always do these initializations
	usleep(200000);

	QStringList volume_params;
	volume_params << "05E8 2d4b WR" << "044D 9C05 WR" << "0529 7fff WR" << "0472 7fff WR";
	volume_params << "0473 7fff WR" << "0451 1f40 WR" << "0453 7fff WR" << "0474 7fff WR";
	volume_params << "044a 7e00 WR" << "044b 1072 WR" << "047b 0072 WR" << "04d0 0003 WR";
	volume_params << "04d5 0004 WR" << "0420 210c WR" << "0401 4800 WR" << "0400 0206 WR";

	foreach (const QString &param, volume_params)
	{
		system(qPrintable(QString("%1 %2 >/dev/null 2>&1").arg(ZARLINK_VOLUME).arg(param)));
		usleep(100000);
	}

	QStringList echo_params;
	echo_params << "04D0 0004 WR" << "0420 210C WR" << "0401 4800 WR" << "0400 0206 WR";

	foreach (const QString &param, echo_params)
	{
		system(qPrintable(QString("%1 %2 >/dev/null 2>&1").arg(ZARLINK_BIN).arg(param)));
		usleep(100000);
	}

	system("/devmem2/devmem2 0x41000030 w 0x08 check > /dev/null 2>&1");
	usleep(100000);
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
	layout->setContentsMargins(13, 0, 0, 28);
	layout->setSpacing(5);
	layout->setColumnStretch(1, 1);

	BtButton *back = new BtButton(bt_global::skin->getImage("back"));
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

void IntercomCallPage::closeCall()
{
	cleanUp();
	bt_global::page_stack.closePage(this);
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
	bt_global::display->makeActive();
	call_accept->setStatus(true);
	mute_button->setStatus(StateButton::DISABLED);
	showPage();
	call_active = true;
	bt_global::status.vde_call_active = true;
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
	bt_global::display->makeActive();
	call_accept->setStatus(false);
	mute_button->setStatus(StateButton::DISABLED);
	showPage();
	call_active = true;
	bt_global::status.vde_call_active = true;
}

void IntercomCallPage::handleClose()
{
	bt_global::status.vde_call_active = false;
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
	}
}

void IntercomCallPage::callStarted()
{
	if (dev->ipCall())
		bt_global::audio_states->toState(AudioStates::IP_INTERCOM_CALL);
	else
		bt_global::audio_states->toState(AudioStates::SCS_INTERCOM_CALL);
	mute_button->setStatus(StateButton::OFF);
	volume->enable();
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
			if (call_active)
				callStarted();
			break;
		case VideoDoorEntryDevice::END_OF_CALL:
			if (call_active)
			{
				call_active = false;
				handleClose();
				// Reset the timers for the freeze/screensaver.
				bt_global::display->makeActive();
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


IntercomMenu::IntercomMenu(const QDomNode &config_node, VideoDoorEntryDevice *dev, IntercomCallPage *call_page)
{
	mapper_int_intercom = new QSignalMapper(this);
	mapper_ext_intercom = new QSignalMapper(this);
	connect(mapper_int_intercom, SIGNAL(mapped(QString)), dev, SLOT(internalIntercomCall(QString)));
	connect(mapper_ext_intercom, SIGNAL(mapped(QString)), dev, SLOT(externalIntercomCall(QString)));

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


VctSettings::VctSettings(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());
	buildPage(new BannerContent(1), new NavigationBar, getTextChild(config_node, "descr"));
	page_content->layout()->setSpacing(30);
	page_content->layout()->setContentsMargins(18, 0, 207, 0);
	loadItems(config_node);
}

void VctSettings::loadItems(const QDomNode &config_node)
{
	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		bool status = getTextChild(item, "enable").toInt();
		banner *b = 0;

		int id = getTextChild(item, "id").toInt();
		int item_id = getTextChild(item, "itemID").toInt();

		switch (id)
		{
		case ITEM_HANDSFREE:
			b = new HandsFree(status, item_id);
			break;
		case ITEM_PROF_STUDIO:
			b = new ProfessionalStudio(status, item_id);
			break;
		case ITEM_RING_EXCLUSION:
			b = new RingtoneExclusion(status, item_id);
			break;
		default:
			Q_ASSERT_X(false, "VctSettings::loadItems", qPrintable(QString("Unknown item %1").arg(id)));
		}

		if (b)
			page_content->appendBanner(b);
	}
}


HandsFree::HandsFree(bool status, int item_id) : BannOnTray(tr("Hands Free"),
	"handsfree_on", "handsfree_off", "tray_handsfree", TrayBar::HANDS_FREE, status, item_id)
{
	updateStatus();
}

void HandsFree::updateStatus()
{
	BannOnTray::updateStatus();
	VCTCallPage::setHandsFree(left_button->getStatus() == StateButton::ON);
}


ProfessionalStudio::ProfessionalStudio(bool status, int item_id) : BannOnTray(tr("Professional studio"),
	"profstudio_on", "profstudio_off", "tray_profstudio", TrayBar::PROF_STUDIO, status, item_id)
{
	updateStatus();
}

void ProfessionalStudio::updateStatus()
{
	BannOnTray::updateStatus();
	VCTCallPage::setProfStudio(left_button->getStatus() == StateButton::ON);
}


RingtoneExclusion::RingtoneExclusion(bool status, int item_id) : BannOnTray(tr("Ringtone Exclusion"),
	"ringexclusion_on", "ringexclusion_off", "tray_ringexclusion", TrayBar::RING_EXCLUSION, status, item_id)
{
	updateStatus();
}

void RingtoneExclusion::updateStatus()
{
	BannOnTray::updateStatus();
	VideoDoorEntry::ring_exclusion = left_button->getStatus() == StateButton::ON;
}

#endif
