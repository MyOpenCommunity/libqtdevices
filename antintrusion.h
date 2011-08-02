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


#ifndef ANTINTRUSION_H
#define ANTINTRUSION_H

#include "bannerpage.h"
#include "skinmanager.h" // SkinManager::CidState
#include "antintrusion_device.h" // NUM_ZONES, AntintrusionDevice
#include "itemlist.h"

class BtButton;
class BannAntintrusion;
class KeypadWithState;
class AntintrusionZone;
class QDomNode;
class QBoxLayout;
class AlarmPage;



/*!
	\defgroup Antintrusion Antintrusion

	The Antintrusion class shows the standard zones (from 1 to 8) and allows
	the user to control the plant partializing (that means excluding) or
	inserting a zone from the plant.
	Every time that an action is performed on the plant, the KeypadWithState class
	is used to ask the password from the user.

	When a new alarm is triggered, the AlarmPage class is shown as popup and the
	alarm is added to a list, displayed by the AlarmListPage class.
*/


class AlarmList : public ItemList
{
Q_OBJECT
public:
	AlarmList(int rows_per_page = ITEM_PER_PAGE, QWidget *parent  = 0) : ItemList(rows_per_page, parent) {}

protected:
	virtual void addHorizontalBox(QGridLayout *layout, const ItemInfo &item, int id_btn);
};


/*!
	\ingroup Antintrusion
	\brief Shows the list of the alarms triggered.
*/
class AlarmListPage : public ScrollablePage
{
Q_OBJECT
public:
	typedef AlarmList ContentType;
	AlarmListPage();
	virtual void showPage();

	void newAlarm(const QString &zone_description, int alarm_id, int alarm_type, int zone);
	void removeAlarm(int alarm_id);
	void removeAll();
	int alarmCount();
	int alarmId(int alarm_type, int zone);

private slots:
	void removeAlarmItem(int index);

private:
	bool need_update;
};


/*!
	\ingroup Antintrusion
	\brief Manages the list of the alarm triggered and the popup pages.
*/
class AlarmManager : public QObject
{
Q_OBJECT
public:
	AlarmManager(SkinManager::CidState cid, QObject *parent = 0);

	// Create a new alarm page and show it
	void newAlarm(int alarm_type, int zone, const QString &zone_description);
	// Remove a single alarm (without showing the remaining alarms)
	void removeAlarm(int alarm_type, int zone);
	// Remove all the alarms
	void removeAll();
	// The number of alarms
	int alarmCount();

public slots:
	void showAlarmList();

signals:
	void alarmListClosed();

private slots:
	void nextAlarm();
	void prevAlarm();
	void deleteAlarm();
	void showHomePage();
	void alarmDestroyed(QObject *);
#ifdef LAYOUT_TS_3_5
	void closeAlarms();
#endif

private:
	int current_alarm;
	static int alarm_serial_id;
	SkinManager::CidState skin_cid;
	QList<AlarmPage*> alarm_pages;
	AlarmListPage *alarm_list;
};



/*!
	\ingroup Antintrusion
	\brief The main page of the burglar alarms system.

	It shows the list of the standard zones, allows the user to activate / disactivate
	the plant, partialize / insert a zone and manages the list of the alarms
	triggered.
*/
class Antintrusion : public BannerPage
{
Q_OBJECT
public:
	Antintrusion(const QDomNode &config_node);
	virtual int sectionId() const;
	virtual void showPage();

private slots:
	void partialize();
	void toggleActivation();
	void doAction();
	void valueReceived(const DeviceValues &values_list);
	void playRingtone();
	void ringtoneFinished();

private:
	enum Action
	{
		NONE = 0,
		ACTIVE,
		PARTIALIZE
	};

	BtButton *partial_button;
	AntintrusionDevice *dev;
	BannAntintrusion *antintrusion_system;
	KeypadWithState *keypad;
	Action action;
	AntintrusionZone *zones[NUM_ZONES];
	AlarmManager *alarm_manager;

	void loadZones(const QDomNode &config_node);
	void updateKeypadStates();
};


#endif

