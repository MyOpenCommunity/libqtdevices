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


#ifndef ICON_SETTINGS_H
#define ICON_SETTINGS_H

#include "bannerpage.h"
#include "iconpage.h"
#include "datetime.h" // PageSetDateTime
#include "state_button.h"

#include <QWidget>

class QDomNode;
class BtButton;
class banner;
class PlatformDevice;
class Text2Column;


class IconSettings : public IconPage
{
Q_OBJECT
public:
	IconSettings(const QDomNode &config_node, bool load_vct_items = false);
	void loadVctItems();

	virtual int sectionId() const;
	static banner *getBanner(const QDomNode &item_node);

private:
	void loadItems(const QDomNode &config_node);
};

// this can be a generic class
class ListPage : public BannerPage
{
public:
	ListPage(const QDomNode &config_node);

private:
	void loadItems(const QDomNode &config_node);
};


class RingtonesPage : public ListPage
{
Q_OBJECT
public:
	RingtonesPage(const QDomNode &config_node);

protected:
	virtual void hideEvent(QHideEvent *e);
};



class VolumePage : public Page
{
Q_OBJECT
public:
	VolumePage(const QDomNode &config_node);

public slots:
	void changeVolume(int new_vol);

protected:
	virtual void hideEvent(QHideEvent *e);
};


class VersionPage : public Page
{
Q_OBJECT
public:
	VersionPage(const QDomNode &config_node);

private:
	Text2Column *text_area;
	PlatformDevice *dev;

private slots:
	void valueReceived(const DeviceValues &values_list);
};


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


// button to toggle on/off the beep sound
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


class PasswordPage : public Page
{
Q_OBJECT
public:
	PasswordPage(const QDomNode &config_node);
};


class BrightnessPage : public Page
{
Q_OBJECT
public:
	BrightnessPage();

private slots:
	void incBrightness();
	void decBrightness();
};

#endif // ICON_SETTINGS_H
