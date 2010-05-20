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


#include "bann_automation.h"
#include "state_button.h"
#include "devices_cache.h" // bt_global::add_device_to_cache
#include "skinmanager.h" // SkinContext, bt_global::skin
#include "lighting_device.h" // LightingDevice
#include "xml_functions.h" // getTextChild
#include "automation_device.h"
#include "entryphone_device.h"

#include <QTimer>
#include <QDebug>

#define BUT_DIM     60


InterblockedActuator::InterblockedActuator(const QString &descr, const QString &where, int openserver_id)
	: BannOpenClose(0)
{
	dev = bt_global::add_device_to_cache(new AutomationDevice(where, PULL_UNKNOWN, openserver_id));
	initBanner(bt_global::skin->getImage("close"), bt_global::skin->getImage("actuator_state"),
		bt_global::skin->getImage("open"), bt_global::skin->getImage("stop"), STOP, descr);

	connect(right_button, SIGNAL(clicked()), SLOT(sendGoUp()));
	connect(left_button, SIGNAL(clicked()), SLOT(sendGoDown()));
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	setOpenserverConnection(dev);
}

void InterblockedActuator::sendGoUp()
{
	dev->goUp();
}

void InterblockedActuator::sendGoDown()
{
	dev->goDown();
}

void InterblockedActuator::sendStop()
{
	dev->stop();
}

void InterblockedActuator::connectButton(BtButton *btn, const char *slot)
{
	btn->disconnect();
	connect(btn, SIGNAL(clicked()), slot);
}

void InterblockedActuator::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case AutomationDevice::DIM_UP:
			setState(OPENING);
			right_button->enable();
			connectButton(right_button, SLOT(sendStop()));
			left_button->disable();
			break;
		case AutomationDevice::DIM_DOWN:
			setState(CLOSING);
			right_button->disable();
			left_button->enable();
			connectButton(left_button, SLOT(sendStop()));
			break;
		case AutomationDevice::DIM_STOP:
			setState(STOP);
			right_button->enable();
			connectButton(right_button, SLOT(sendGoUp()));
			left_button->enable();
			connectButton(left_button, SLOT(sendGoDown()));
			break;
		}
		++it;
	}
}

SecureInterblockedActuator::SecureInterblockedActuator(const QString &descr, const QString &where, int openserver_id) :
	BannOpenClose(0)
{
	dev = bt_global::add_device_to_cache(new AutomationDevice(where, PULL_UNKNOWN, openserver_id));

	initBanner(bt_global::skin->getImage("close"), bt_global::skin->getImage("actuator_state"),
		bt_global::skin->getImage("open"), bt_global::skin->getImage("stop"), STOP, descr);

	is_any_button_pressed = false;
	connectButtons();
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	setOpenserverConnection(dev);
}

void SecureInterblockedActuator::sendOpen()
{
	dev->goUp();
	is_any_button_pressed = true;
}

void SecureInterblockedActuator::sendClose()
{
	dev->goDown();
	is_any_button_pressed = true;
}

void SecureInterblockedActuator::buttonReleased()
{
	QTimer::singleShot(500, this, SLOT(sendStop()));
	is_any_button_pressed = false;
}

void SecureInterblockedActuator::sendStop()
{
	dev->stop();
}

void SecureInterblockedActuator::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case AutomationDevice::DIM_UP:
			setState(OPENING);
			left_button->disable();
			right_button->enable();
			changeButtonStatus(right_button);
			break;
		case AutomationDevice::DIM_DOWN:
			setState(CLOSING);
			right_button->disable();
			left_button->enable();
			changeButtonStatus(left_button);
			break;
		case AutomationDevice::DIM_STOP:
			setState(STOP);
			right_button->enable();
			left_button->enable();
			if (!is_any_button_pressed)
			{
				right_button->disconnect();
				left_button->disconnect();
				connectButtons();
			}
			break;
		}
		++it;
	}
}

void SecureInterblockedActuator::connectButtons()
{
	connect(right_button, SIGNAL(pressed()), SLOT(sendOpen()));
	connect(left_button, SIGNAL(pressed()), SLOT(sendClose()));
	connect(right_button, SIGNAL(released()), SLOT(buttonReleased()));
	connect(left_button, SIGNAL(released()), SLOT(buttonReleased()));
}

void SecureInterblockedActuator::changeButtonStatus(BtButton *btn)
{
	if (!is_any_button_pressed)
	{
		btn->disconnect();
		connect(btn, SIGNAL(clicked()), SLOT(sendStop()));
	}
}


GateEntryphoneActuator::GateEntryphoneActuator(const QString &descr, const QString &where, int openserver_id) :
	BannSinglePuls(0)
{
	dev = bt_global::add_device_to_cache(new EntryphoneDevice(where, QString(), openserver_id));
	setOpenserverConnection(dev);
	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("gate"), descr);
	connect(right_button, SIGNAL(pressed()), SLOT(activate()));
}

void GateEntryphoneActuator::activate()
{
	dev->openLock();
	dev->releaseLock();
}


GateLightingActuator::GateLightingActuator(const BtTime &t, const QString &descr, const QString &where, int openserver_id) :
	BannSinglePuls(0),
	time(t)
{
	// we don't need to init the device, as we don't care about its status
	dev = bt_global::add_device_to_cache(new LightingDevice(where, PULL_UNKNOWN, openserver_id), NO_INIT);
	setOpenserverConnection(dev);
	initBanner(bt_global::skin->getImage("on"), bt_global::skin->getImage("gate"), descr);
	connect(right_button, SIGNAL(clicked()), SLOT(activate()));
}

void GateLightingActuator::activate()
{
	dev->variableTiming(time.hour(), time.minute(), time.second());
}


InterblockedActuatorGroup::InterblockedActuatorGroup(const QStringList &addresses, const QString &descr, int openserver_id) :
	Bann3Buttons(0)
{
	foreach (const QString &where, addresses)
		actuators.append(bt_global::add_device_to_cache(new AutomationDevice(where, PULL, openserver_id), NO_INIT));
	// since we have just one openserver_id, we just need to connect to one device
	Q_ASSERT_X(!actuators.isEmpty(), "InterblockedActuatorGroup::InterblockedActuatorGroup", "No device found!");
	setOpenserverConnection(actuators[0]);


	initBanner(bt_global::skin->getImage("scroll_down"), bt_global::skin->getImage("stop"),
		bt_global::skin->getImage("scroll_up"), descr);

	connect(right_button, SIGNAL(clicked()), SLOT(sendOpen()));
	connect(center_button, SIGNAL(clicked()), SLOT(sendStop()));
	connect(left_button, SIGNAL(clicked()), SLOT(sendClose()));
}

void InterblockedActuatorGroup::sendClose()
{
	foreach (AutomationDevice *dev, actuators)
		dev->goDown();
}

void InterblockedActuatorGroup::sendOpen()
{
	foreach (AutomationDevice *dev, actuators)
		dev->goUp();
}

void InterblockedActuatorGroup::sendStop()
{
	foreach (AutomationDevice *dev, actuators)
		dev->stop();
}


PPTStat::PPTStat(QString where, int openserver_id) : BannerOld(0)
{
	dev = bt_global::add_device_to_cache(new PPTStatDevice(where, openserver_id));
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	setOpenserverConnection(dev);

	// This banner shows only an icon in central position and a text below.
	addItem(ICON, (banner_width - BUT_DIM * 2) / 2, 0, BUT_DIM*2 ,BUT_DIM);
	addItem(TEXT, 0, BUT_DIM, banner_width, banner_height - BUT_DIM);

	img_open = bt_global::skin->getImage("pptstat_open");
	img_close = bt_global::skin->getImage("pptstat_close");

	if (!img_open.isEmpty())
		SetIcons(2, img_open);
	Draw();
}

void PPTStat::valueReceived(const DeviceValues &status_list)
{
	bool st = status_list[PPTStatDevice::DIM_STATUS].toBool();
	SetIcons(2, st ? img_close : img_open);
	Draw();
}

