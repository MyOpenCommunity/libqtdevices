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
#include "device.h" // DeviceValues
#include "bttime.h" // BtTime

/// Forward Declarations
class device;
class device_status;
class PPTStatDevice;
class AutomationDevice;
class LightingDevice;
class QDomNode;


class InterblockedActuator : public BannOpenClose
{
Q_OBJECT
public:
	InterblockedActuator(const QString &descr, const QString &where, int openserver_id);

private slots:
	void sendGoUp();
	void sendGoDown();
	void sendStop();
	void valueReceived(const DeviceValues &values_list);

private:
	void connectButton(BtButton *btn, const char *slot);
	AutomationDevice *dev;
};

/**
 * Send open/close frame on button press and send stop on button release.
 *
 * Banner behaviour must change (both graphically and logically) depending on who was the origin of the action.
 * In case of update from bus, the icon must be STOP not pressed and a stop frame must be sent when the button
 * is clicked.
 * In case of command sent after the user pressed the touch screen, the icon must be STOP pressed and a stop
 * frame must be sent on button release.
 */
class SecureInterblockedActuator : public BannOpenClose
{
Q_OBJECT
public:
	SecureInterblockedActuator(const QString &descr, const QString &where, int openserver_id);

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

class GateEntryphoneActuator : public BannSinglePuls
{
Q_OBJECT
public:
	GateEntryphoneActuator(const QString &descr, const QString &where, int openserver_id);

private slots:
	void activate();

private:
	QString where;
	device *dev;
};

class GateLightingActuator : public BannSinglePuls
{
Q_OBJECT
public:
	GateLightingActuator(const BtTime &t, const QString &descr, const QString &where, int openserver_id);

private slots:
	void activate();

private:
	BtTime time;
	LightingDevice *dev;
};


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


class PPTStat : public BannerOld
{
Q_OBJECT
public:
	PPTStat(QString where, int openserver_id);

private:
	PPTStatDevice *dev;
	QString img_open, img_close;
private slots:
	void valueReceived(const DeviceValues &values_list);
};

#endif
