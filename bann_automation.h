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


#ifndef BANN_AUTOMATION_H
#define BANN_AUTOMATION_H

#include "bann1_button.h" // BannSinglePuls
#include "bann3_buttons.h" // Bann3Buttons
#include "bann2_buttons.h" // BannOpenClose
#include "pulldevice.h" // DeviceValues, PullMode
#include "bttime.h" // BtTime

/// Forward Declarations
class device;
class device_status;
class PPTStatDevice;
class AutomationDevice;
class LightingDevice;
class VideoDoorEntryDevice;
class QDomNode;


/*!
	\ingroup Automation
	\brief Banner to control a generic automation device
 */
class InterblockedActuator : public BannOpenClose
{
Q_OBJECT
public:
	InterblockedActuator(const QString &descr, const QString &where, int openserver_id, PullMode pull_mode);

private slots:
	void sendGoUp();
	void sendGoDown();
	void sendStop();
	void valueReceived(const DeviceValues &values_list);

private:
	void connectButton(BtButton *btn, const char *slot);
	AutomationDevice *dev;
};

/*!
	\ingroup Automation
	\brief Banner to control a secure automation device (stops automatically when releasing the button)

	Banner behaviour changes (both graphically and logically) depending on who was the origin of the action.

	In case of update from bus, the icon is STOP (not pressed) and a stop frame must be sent when the button
	is clicked.

	In case of command sent after the user pressed the touch screen, the icon must be STOP (pressed) and a stop
	frame must be sent after button release.
 */
class SecureInterblockedActuator : public BannOpenClose
{
Q_OBJECT
public:
	SecureInterblockedActuator(const QString &descr, const QString &where, int openserver_id, PullMode pull_mode);

private slots:
	void sendOpen();
	void sendClose();
	void buttonReleased();
	void sendStop();
	void valueReceived(const DeviceValues &values_list);

private:
	void connectButtons();
	void changeButtonStatus(BtButton *btn);
	AutomationDevice *dev;
	bool is_any_button_pressed;
};


/*!
	\ingroup Automation
	\brief Banner to control the entry phone actuator from video door-entry system.
 */
class GateEntryphoneActuator : public BannSinglePuls
{
Q_OBJECT
public:
	GateEntryphoneActuator(const QString &descr, const QString &where, int openserver_id);

private slots:
	void activate();

private:
	QString where;
	VideoDoorEntryDevice *dev;
};


/*!
	\ingroup Automation
	\brief Banner to control the entry phone actuator from lighting system.
 */
class GateLightingActuator : public BannSinglePuls
{
Q_OBJECT
public:
	GateLightingActuator(const BtTime &t, const QString &descr, const QString &where, int openserver_id, PullMode pull_mode);

private slots:
	void activate();

private:
	BtTime time;
	LightingDevice *dev;
};


/*!
	\ingroup Automation
	\brief Banner to control a group of automation devices.
 */
class InterblockedActuatorGroup : public Bann3Buttons
{
Q_OBJECT
public:
	InterblockedActuatorGroup(const QStringList &addresses, const QString &descr, int openserver_id);

private slots:
	void sendOpen();
	void sendClose();
	void sendStop();

private:
	QList<AutomationDevice *> actuators;
};


/*!
	\ingroup Automation
	\brief Banner display the state of dry contacts.
 */
class PPTStat : public Bann2Buttons
{
Q_OBJECT
public:
	PPTStat(QString description, QString where, int openserver_id);

private:
	QString img_open, img_close;

private slots:
	void valueReceived(const DeviceValues &values_list);
};

#endif
