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


#include "antintrusion.h"
#include "main.h" // ANTIINTRUSION
#include "xml_functions.h" // getTextChild, getChildren
#include "bann_antintrusion.h"
#include "btbutton.h"
#include "antintrusion_device.h"
#include "keypad.h" // KeypadWithState
#include "devices_cache.h"
#include "alarmpage.h"
#include "btmain.h" // showHomePage
#include "displaycontrol.h" // makeActive
#include "pagestack.h"
#include "icondispatcher.h" // bt_global::icons_cache
#include "navigation_bar.h"
#include "audiostatemachine.h" // bt_global::audio_states
#include "ringtonesmanager.h" // bt_global::ringtones


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QVariant>
#include <QDateTime>
#include <QLabel>
#include <QButtonGroup>


int AlarmManager::alarm_serial_id = 0;


enum
{
	ITEM_ICON = 0,
	BUTTON_ICON = 1
};


namespace
{

	QString getAlarmDescription(int alarm_type)
	{
		switch (alarm_type)
		{
		case AntintrusionDevice::DIM_ANTIPANIC_ALARM:
			return QT_TRANSLATE_NOOP("Antintrusion", "anti-panic");
		case AntintrusionDevice::DIM_TAMPER_ALARM:
			return QT_TRANSLATE_NOOP("Antintrusion", "tamper");
		case AntintrusionDevice::DIM_INTRUSION_ALARM:
			return QT_TRANSLATE_NOOP("Antintrusion", "intrusion");
		case AntintrusionDevice::DIM_TECHNICAL_ALARM:
			return QT_TRANSLATE_NOOP("Antintrusion", "technical");
		default:
			return QString();
		}
	}

	QString getAlarmTagname(int alarm_type)
	{
		switch (alarm_type)
		{
		case AntintrusionDevice::DIM_ANTIPANIC_ALARM:
			return "panic_alarm";
		case AntintrusionDevice::DIM_TAMPER_ALARM:
			return "tamper_alarm";
		case AntintrusionDevice::DIM_INTRUSION_ALARM:
			return "intrusion_alarm";
		case AntintrusionDevice::DIM_TECHNICAL_ALARM:
			return "technic_alarm";
		default:
			return QString();
		}
	}
}

void AlarmList::addHorizontalBox(QBoxLayout *layout, const ItemInfo &item, int id_btn)
{
	QWidget *box_widget = new QWidget;
	QHBoxLayout *box = new QHBoxLayout(box_widget);

	const QDateTime &date = item.data.toList().at(0).toDateTime();

	QLabel *label_icon = new QLabel;
	label_icon->setPixmap(*bt_global::icons_cache.getIcon(item.icons[ITEM_ICON]));

	QLabel *label_desc = new QLabel(item.description);
	label_desc->setAlignment(Qt::AlignTop|Qt::AlignLeft);
	label_desc->setFont(bt_global::font->get(FontManager::TEXT));

	QLabel *label_zone = new QLabel(item.name);
	label_zone->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
	label_zone->setFont(bt_global::font->get(FontManager::TEXT));

	QLabel *label_date = new QLabel(date.toString("dd/MM/yyyy\nhh:mm:ss"));
	label_date->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
	label_date->setFont(bt_global::font->get(FontManager::TEXT));

	// delete button
	BtButton *trash = new BtButton(item.icons[BUTTON_ICON]);
	buttons_group->addButton(trash, id_btn);

	box->addWidget(label_icon);
	box->addWidget(label_desc, 1);
	box->addWidget(label_zone, 1);
	box->addWidget(label_date, 1);
	box->addWidget(trash);

	layout->addWidget(box_widget);
}


AlarmListPage::AlarmListPage()
{
	QWidget *header = new QWidget;
	QHBoxLayout *header_layout = new QHBoxLayout(header);

	QLabel *label_type = new QLabel(tr("Alarm type"));
	label_type->setAlignment(Qt::AlignHCenter);
	header_layout->addWidget(label_type, 1);

	QLabel *label_zone = new QLabel(tr("Zone"));
	label_zone->setAlignment(Qt::AlignHCenter);
	header_layout->addWidget(label_zone, 1);

	QLabel *label_date = new QLabel(tr("Date & Hour"));
	label_date->setAlignment(Qt::AlignLeft);
	header_layout->addWidget(label_date, 1);

	AlarmList *item_list = new AlarmList;

	QWidget *main_widget = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(main_widget);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);
	main_layout->addWidget(header);
	main_layout->addWidget(item_list, 1);

	connect(item_list, SIGNAL(itemIsClicked(int)), SLOT(removeAlarmItem(int)));

	PageTitleWidget *title = 0;

#ifdef LAYOUT_TS_10
	title = new PageTitleWidget(tr("Alarms"), SMALL_TITLE_HEIGHT);
#endif

	buildPage(main_widget, item_list, new NavigationBar, title);
	need_update = false;
}

void AlarmListPage::removeAlarmItem(int index)
{
	page_content->removeItem(index);
	page_content->showList();
}

void AlarmListPage::removeAll()
{
	for (int i = 0; i < page_content->itemCount(); ++i)
		page_content->removeItem(i);

	page_content->showList();
}

void AlarmListPage::showPage()
{
	if (need_update)
		page_content->showList();
	need_update = false;
	ScrollablePage::showPage();
}

int AlarmListPage::alarmId(int alarm_type, int zone)
{
	for (int i = 0; i < page_content->itemCount(); ++i)
	{
		const QList<QVariant> &data = page_content->item(i).data.toList();
		if (data.at(2).toInt() == alarm_type && data.at(3).toInt() == zone)
			return data.at(1).toInt();
	}
	return -1;
}

void AlarmListPage::newAlarm(const QString &zone_description, int alarm_id, int alarm_type, int zone)
{
#ifdef LAYOUT_TS_10
	QString icon = bt_global::skin->getImage(getAlarmTagname(alarm_type));
#else
	// Because on ts 3.5 the AlarmListPage is never shown we use an image always present
	// only to silent warnings from SkinManager.
	QString icon = bt_global::skin->getImage("ok");
#endif
	QString description = getAlarmDescription(alarm_type);
	QList<QVariant> data;
	data.append(QDateTime::currentDateTime());
	data.append(alarm_id);
	data.append(alarm_type);
	data.append(zone);
	ItemList::ItemInfo info(zone_description, description, QStringList() << icon << bt_global::skin->getImage("alarm_del"), data);
	page_content->insertItem(page_content->itemCount(), info);
	need_update = true;
}

int AlarmListPage::alarmCount()
{
	return page_content->itemCount();
}

void AlarmListPage::removeAlarm(int alarm_id)
{
	for (int i = 0; i < page_content->itemCount(); ++i)
	{
		const ItemList::ItemInfo &item = page_content->item(i);
		if (item.data.toList().at(1).toInt() == alarm_id)
		{
			page_content->removeItem(i);
			return;
		}
	}
}


AlarmManager::AlarmManager(SkinManager::CidState cid, QObject *parent) : QObject(parent)
{
	skin_cid = cid;
	current_alarm = -1;
	alarm_list = new AlarmListPage;
	connect(alarm_list, SIGNAL(Closed()), SIGNAL(alarmListClosed()));
}

int AlarmManager::alarmCount()
{
	return alarm_list->alarmCount();
}

void AlarmManager::newAlarm(int alarm_type, int zone, const QString &zone_description)
{
	if (alarm_list->alarmId(alarm_type, zone) != -1)
	{
		qDebug() << "Skip duplicated alarm for zone:" << zone << "type:" << alarm_type;
		return;
	}
	qDebug() << "New alarm for zone:" << zone << "type:" << alarm_type;

	bt_global::skin->setCidState(skin_cid);
	QString page_icon = bt_global::skin->getImage(getAlarmTagname(alarm_type) + "_page");

	int alarm_id = ++alarm_serial_id;
	AlarmPage *alarm = new AlarmPage(page_icon, getAlarmDescription(alarm_type), zone_description, alarm_id);

	connect(alarm, SIGNAL(Next()), SLOT(nextAlarm()));
	connect(alarm, SIGNAL(Prev()), SLOT(prevAlarm()));
	connect(alarm, SIGNAL(Delete()), SLOT(deleteAlarm()));
	connect(alarm, SIGNAL(showHomePage()), SLOT(showHomePage()));
	connect(alarm, SIGNAL(destroyed(QObject*)), SLOT(alarmDestroyed(QObject*)));
	connect(alarm, SIGNAL(showAlarmList()), SLOT(showAlarmList()));

	alarm_pages.append(alarm);

	alarm_list->newAlarm(zone_description, alarm_id, alarm_type, zone);

	// The current alarm is the last alarm inserted
	current_alarm = alarm_pages.size() - 1;

	bt_global::display->makeActive();
	alarm->showPage();
}

void AlarmManager::removeAlarm(int alarm_type, int zone)
{
	int alarm_id = alarm_list->alarmId(alarm_type, zone);
	foreach (AlarmPage *page, alarm_pages)
	{
		if (page->alarmId() == alarm_id)
		{
			page->deleteLater();
			break;
		}
	}
	alarm_list->removeAlarm(alarm_id);
}

void AlarmManager::nextAlarm()
{
	Q_ASSERT_X(current_alarm >= 0 && current_alarm < alarm_pages.size(), "AlarmManager::nextAlarm",
		qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(current_alarm).arg(alarm_pages.size())));

	if (++current_alarm >= alarm_pages.size())
		current_alarm = 0;

	alarm_pages.at(current_alarm)->showPage();
}

void AlarmManager::prevAlarm()
{
	Q_ASSERT_X(current_alarm >= 0 && current_alarm < alarm_pages.size(), "AlarmManager::prevAlarm",
		qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(current_alarm).arg(alarm_pages.size())));

	if (--current_alarm < 0)
		current_alarm = alarm_pages.size() - 1;

	alarm_pages.at(current_alarm)->showPage();
}

void AlarmManager::deleteAlarm()
{
	Q_ASSERT_X(current_alarm >= 0 && current_alarm < alarm_pages.size(), "AlarmManager::deleteAlarm",
		qPrintable(QString("Current alarm index (%1) out of range! [0, %2]").arg(current_alarm).arg(alarm_pages.size())));

	// the page is removed from the alarm_pages in the alarmDestroyed slot
	AlarmPage *to_die = alarm_pages.at(current_alarm);

	// In this case the user has seen the alarm and delete it directly from the AlarmPage,
	// so we want to delete also the entry from the AlarmList. We do that using a global
	// static alarm id.
	alarm_list->removeAlarm(to_die->alarmId());
	to_die->deleteLater();
}

void AlarmManager::showHomePage()
{
	bt_global::page_stack.clear();
	bt_global::btmain->showHomePage();
}

#ifdef LAYOUT_TS_3_5

void AlarmManager::showAlarmList()
{
	if (alarm_pages.isEmpty())
		return;

	current_alarm = alarm_pages.size() - 1;
	alarm_pages.at(current_alarm)->showPage();
}

#else

void AlarmManager::showAlarmList()
{
	bt_global::page_stack.clear();
	alarm_list->showPage();
}

#endif

void AlarmManager::alarmDestroyed(QObject *page)
{
	alarm_pages.removeOne(static_cast<AlarmPage*>(page));
	--current_alarm;
}


Antintrusion::Antintrusion(const QDomNode &config_node)
{
	SkinContext cxt(getTextChild(config_node, "cid").toInt());

	dev = bt_global::add_device_to_cache(new AntintrusionDevice);
	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));

	alarm_manager = new AlarmManager(bt_global::skin->getCidState(), this);
	connect(alarm_manager, SIGNAL(alarmListClosed()), SLOT(showPage()));

	antintrusion_system = new BannAntintrusion;
	antintrusion_system->showAlarmsButton(false);
	connect(antintrusion_system, SIGNAL(toggleActivation()), SLOT(toggleActivation()));
	connect(antintrusion_system, SIGNAL(showAlarms()), alarm_manager, SLOT(showAlarmList()));

	QWidget *top_widget = new QWidget;

	QHBoxLayout *layout = new QHBoxLayout(top_widget);

	layout->setSpacing(5);
	layout->addWidget(antintrusion_system);
	layout->addStretch(1);

	action = NONE;

#ifdef LAYOUT_TS_10
	layout->setContentsMargins(10, 0, 20, 10);
	partial_button = new BtButton(bt_global::skin->getImage("partial"));
	layout->addWidget(partial_button);

#else
	layout->setContentsMargins(0, 0, 0, 25);
#endif

	BannerContent *content = new BannerContent;
	QWidget *main_widget = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(main_widget);
	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(5);
	main_layout->addWidget(top_widget);
	main_layout->addWidget(content, 1);

#ifdef LAYOUT_TS_10
	buildPage(main_widget, content, new NavigationBar, getTextChild(config_node, "descr"), SMALL_TITLE_HEIGHT);
#else
	NavigationBar *nav_bar = new NavigationBar(bt_global::skin->getImage("partial"));
	partial_button = nav_bar->forward_button;
	buildPage(main_widget, content, nav_bar);
#endif

	connect(partial_button, SIGNAL(clicked()), SLOT(partialize()));

	loadZones(config_node);

	QList<int> states;
	for (int i = 0; i < NUM_ZONES; ++i)
	{
		int state = -1;
		if (zones[i] != 0)
			state = (zones[i]->isPartialized() ? 0 : 1);

		states.append(state);
	}

	keypad = new KeypadWithState(QList<int>());
	updateKeypadStates();
	keypad->setMode(Keypad::HIDDEN);
	connect(keypad, SIGNAL(Closed()), SLOT(showPage()));
	connect(keypad, SIGNAL(accept()), SLOT(doAction()));
	connect(keypad, SIGNAL(accept()), SLOT(showPage()));

}

void Antintrusion::showPage()
{
	antintrusion_system->showAlarmsButton(alarm_manager->alarmCount() > 0);
	BannerPage::showPage();
}

void Antintrusion::updateKeypadStates()
{
	QList<int> states;
	for (int i = 0; i < NUM_ZONES; ++i)
	{
		int state = -1;
		if (zones[i] != 0)
			state = (zones[i]->isPartialized() ? 0 : 1);

		states.append(state);
	}
	keypad->setStates(states);
}

int Antintrusion::sectionId() const

{
	return ANTIINTRUSION;
}

void Antintrusion::loadZones(const QDomNode &config_node)
{
	for (int i = 0; i < NUM_ZONES; ++i)
		zones[i] = 0;

	foreach (const QDomNode &item, getChildren(config_node, "item"))
	{
		SkinContext cxt(getTextChild(item, "cid").toInt());
		int zone_number = getTextChild(item, "where").mid(1).toInt();
		AntintrusionZone *b = new AntintrusionZone(zone_number, getTextChild(item, "descr"));
		// Sync the graphical aspect with the device status
		dev->partializeZone(zone_number, b->isPartialized());
		connect(b, SIGNAL(requestPartialization(int,bool)), dev, SLOT(partializeZone(int,bool)));
		page_content->appendBanner(b);
		zones[zone_number - 1] = b;
	}
}

void Antintrusion::toggleActivation()
{
	action = ACTIVE;
	updateKeypadStates();
	keypad->showPage();
}

void Antintrusion::partialize()
{
	action = PARTIALIZE;
	updateKeypadStates();
	keypad->showPage();
}

void Antintrusion::doAction()
{
	QString password = keypad->getText();
	if (password.isEmpty())
		return;

	switch (action)
	{
	case ACTIVE:
		dev->toggleActivation(password);
		break;
	case PARTIALIZE:
		dev->setPartialization(password);
		break;
	default:
		qWarning() << "Antintrusion::doAction() Unknown action " << action;
	}
	keypad->resetText();
}

void Antintrusion::playRingtone()
{
	disconnect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), this, SLOT(playRingtone()));
	connect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(ringtoneFinished()));
	bt_global::ringtones->playRingtone(Ringtones::ALARM);
}

void Antintrusion::ringtoneFinished()
{
	bt_global::audio_states->removeState(AudioStates::PLAY_RINGTONE);
	disconnect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(ringtoneFinished()));
}

void Antintrusion::valueReceived(const DeviceValues &values_list)
{
	DeviceValues::const_iterator it = values_list.constBegin();
	while (it != values_list.constEnd())
	{
		switch (it.key())
		{
		case AntintrusionDevice::DIM_SYSTEM_INSERTED:
		{
			bool inserted = it.value().toBool();
#ifdef LAYOUT_TS_3_5
			partial_button->setVisible(!inserted);
#endif
			antintrusion_system->setState(inserted);
			for (int i = 0; i < NUM_ZONES; ++i)
				if (zones[i])
					zones[i]->enablePartialization(!inserted);
			break;
		}
		case AntintrusionDevice::DIM_ZONE_INSERTED:
		case AntintrusionDevice::DIM_ZONE_PARTIALIZED:
		{
			int zone = it.value().toInt();
			if (zone > 0 && zone <= NUM_ZONES)
				zones[zone - 1]->setPartialization(it.key() == AntintrusionDevice::DIM_ZONE_PARTIALIZED);
			break;
		}
		case AntintrusionDevice::DIM_ANTIPANIC_ALARM:
		case AntintrusionDevice::DIM_INTRUSION_ALARM:
		case AntintrusionDevice::DIM_TAMPER_ALARM:
		case AntintrusionDevice::DIM_TECHNICAL_ALARM:
		{
			int zone = it.value().toInt();
			QString zone_description;
			if (it.key() == AntintrusionDevice::DIM_TECHNICAL_ALARM)
				zone_description = QString("AUX:%1").arg(zone);
			else if (zone > 0 && zone <= NUM_ZONES && zones[zone - 1])
				zone_description = zones[zone - 1]->zoneDescription();
			else
				zone_description = tr("Z%1").arg(zone);

			alarm_manager->newAlarm(it.key(), zone, zone_description);

			connect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), this, SLOT(playRingtone()));
			if (bt_global::audio_states->currentState() != AudioStates::PLAY_RINGTONE)
				bt_global::audio_states->toState(AudioStates::PLAY_RINGTONE);
			break;
		}
		case AntintrusionDevice::DIM_RESET_TECHNICAL_ALARM:
			alarm_manager->removeAlarm(AntintrusionDevice::DIM_TECHNICAL_ALARM, it.value().toInt());
			break;
		}
		++it;
	}
}


