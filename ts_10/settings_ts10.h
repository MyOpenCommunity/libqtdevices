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


#ifndef SETTINGS_TS10_H
#define SETTINGS_TS10_H

#include "bannerpage.h"
#include "iconpage.h"
#include "datetime.h" // PageSetDateTime
#include "state_button.h"

#include <QWidget>

class QDomNode;
class BtButton;
class Banner;
class PlatformDevice;
class Text2Column;
class ItemTuning;

/*!
	\defgroup Settings Settings

	This section is always present and allows the user to control various
	aspects of the interface.
*/


/*!
	\ingroup Settings
	\brief The main page of the \ref Settings for the touchscreen 10''.
*/
class IconSettings : public IconPage
{
Q_OBJECT
public:
	IconSettings(const QDomNode &config_node);

	virtual int sectionId() const;
	static Banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};


/*!
	\ingroup Settings
	\brief Allows the user to change the ringtone to play for each Ringtones::Type.
*/
class RingtonesPage : public ListPage
{
Q_OBJECT
public:
	RingtonesPage(const QDomNode &config_node);
	virtual void showPage();
	virtual void cleanUp();

private slots:
	void handleClose();
};


/*!
	\ingroup Settings
	\brief Set the volume level for the Ringtones::PE1 ringtone.
*/
class VolumePage : public Page
{
Q_OBJECT
public:
	VolumePage(const QDomNode &config_node);

	virtual void cleanUp();

private slots:
	void ringtoneFinished();
	void playRingtone();
	void startPlayRingtone();
	void handleClose();

private:
	ItemTuning *volume;
	bool is_playing;
};


/*!
	\ingroup Settings
	\brief Shows information about the software.
*/
class InfoPage : public Page
{
Q_OBJECT
public:
	InfoPage(const QDomNode &config_node);

private:
	Text2Column *text_area;
	PlatformDevice *dev;

private slots:
	void valueReceived(const DeviceValues &values_list);
};


/*!
	\ingroup Settings
	\brief Allows the user to change the date/time of the touchscreen.
*/
class ChangeDateTime : public PageSetDateTime
{
Q_OBJECT
public:
	ChangeDateTime(const QString &ok_button_icon);

	virtual void showPage();

private:
	PlatformDevice *dev;

private slots:
	void dateTimeChanged(QDate date, BtTime time);
};


/*!
	\ingroup Settings
	\brief Enable or disable the beep.
*/
class ToggleBeep : public IconPageButton
{
Q_OBJECT
public:
	ToggleBeep(int item_id, bool status, QString label, QString icon_on, QString icon_off);

private slots:
	void toggleBeep();

private:
	int item_id;
};


/*!
	\ingroup Settings
	\brief Allows the user to enable/disable the password, and to change it.
*/
class PasswordPage : public Page
{
Q_OBJECT
public:
	PasswordPage(const QDomNode &config_node);
};


/*!
	\ingroup Settings
	\brief Controls the brightness of the screen during the normal use.
*/
class OperativeBrightnessPage : public Page
{
Q_OBJECT
public:
	OperativeBrightnessPage();

private slots:
	void incBrightness();
	void decBrightness();
};

#endif // SETTINGS_TS10_H
