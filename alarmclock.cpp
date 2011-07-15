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


#include "alarmclock.h"
#include "generic_functions.h" // setCfgValue
#include "hardware_functions.h" // getBeep, setBeep, beep
#include "state_button.h"
#include "datetime.h"
#include "fontmanager.h" // bt_global::font
#include "displaycontrol.h" // bt_global::display
#include "btmain.h" // bt_global::btmain
#include "singlechoicepage.h"
#include "skinmanager.h"
#include "media_device.h" // AlarmSoundDiffDevice
#include "navigation_bar.h"
#include "generic_functions.h" // getBostikName
#include "devices_cache.h" // bt_global::add_device_to_cache
#include "mediaplayer.h" // bt_global::sound
#include "sounddiffusionpage.h" // alarmClockPage
#ifdef LAYOUT_TS_10
#include "audiostatemachine.h"
#endif

#include <openmsg.h>

#include <QApplication>
#include <QDateTime>
#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


AlarmClock::AlarmClock(int _item_id, Type type, int days_active, int hour, int minute)
{
	item_id = _item_id;
	timer_increase_volume = NULL;
	alarm_time = QTime(hour, minute);
	check_alarm_timer = NULL;
	alarm_type = type;
	active = false;

	for (int i = 0; i < 7; ++i)
	{
		int mask = 1 << (6 - i);
		alarm_days.append((days_active & mask) ? true : false);
	}

	for (int i = 0; i < AMPLI_NUM; i++)
		alarm_volumes[i] = -1;

#ifdef LAYOUT_TS_3_5
	alarm_time_page = new AlarmClockTime(alarm_time);
	alarm_days_page = new AlarmClockDays(alarm_type, alarm_days);

	connect(alarm_time_page, SIGNAL(forwardClick()), alarm_days_page, SLOT(showPage()));
	connect(alarm_time_page, SIGNAL(Closed()), SIGNAL(Closed()));

	connect(alarm_days_page, SIGNAL(Closed()), alarm_time_page, SLOT(showPage()));
	if (alarm_type == SOUND_DIFF)
		connect(alarm_days_page, SIGNAL(forwardClick()), SLOT(showSoundDiffPage()));
	else
		connect(alarm_days_page, SIGNAL(forwardClick()), SLOT(saveAndActivate()));
#else
	alarm_time_page = alarm_days_page = new AlarmClockTimeDays(alarm_time, alarm_type, alarm_days);
	connect(alarm_time_page, SIGNAL(Closed()), SIGNAL(Closed()));
	connect(alarm_time_page, SIGNAL(okClicked()), SLOT(saveAndActivate()));
	connect(alarm_time_page, SIGNAL(showSoundDiffusion()), SLOT(showSoundDiffPage()));
#endif

	if (alarm_type == SOUND_DIFF)
	{
		alarm_sound_diff = new AlarmClockSoundDiff;
		connect(alarm_sound_diff, SIGNAL(Closed()), SLOT(resetVolumes()));
		connect(alarm_sound_diff, SIGNAL(Closed()), alarm_days_page, SLOT(showPage()));
		// when confirming the sound diffusion status, also save and activate the alarm
		connect(alarm_sound_diff, SIGNAL(saveVolumes()), SLOT(saveVolumes()));
		connect(alarm_sound_diff, SIGNAL(saveVolumes()), SLOT(saveAndActivate()));

		dev = bt_global::add_device_to_cache(new AlarmSoundDiffDevice());
		general = AmplifierDevice::createDevice("0");

		connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
	}
	else
		alarm_sound_diff = NULL;
}

void AlarmClock::showPage()
{
	alarm_time_page->resetAlarmTime();
	alarm_days_page->resetAlarmDays();
	alarm_time_page->showPage();
}

void AlarmClock::saveAndActivate()
{
	setActive(true);
	alarm_time = alarm_time_page->getAlarmTime();
	alarm_days = alarm_days_page->getAlarmDays();

	alarm_time_page->saveAlarmTime();
	alarm_days_page->saveAlarmDays();

	QMap<QString, QString> data;
	data["hour"] = alarm_time.toString("hh");
	data["minute"] = alarm_time.toString("mm");

	int active = 0;
	for (int i = 0; i < 7; ++i)
		if (alarm_days[i])
			active |= 1 << (6 - i);
	data["days"] = QString::number(active);

	// Because the old ts3 config file doesn't have the 'days' node, we cannot save
	// the configuration
#ifdef CONFIG_TS_10
	setCfgValue(data, item_id);
#endif

	emit Closed();
}

void AlarmClock::showSoundDiffPage()
{
	update_eeprom = true;
	source = 1;
	station = 0;
	dev->setReceiveFrames(true);
	for (unsigned idx = 0; idx < AMPLI_NUM; idx++)
		alarm_volumes[idx] = -1;

	alarm_sound_diff->showPage();
}

void AlarmClock::saveVolumes()
{
	dev->setReceiveFrames(false);
	general->turnOff();

	if (update_eeprom)
		setAlarmVolumes(serial_number-1, alarm_volumes, source, station);
	update_eeprom = false;
}

void AlarmClock::resetVolumes()
{
	dev->setReceiveFrames(false);
	general->turnOff();
	update_eeprom = false;
}

void AlarmClock::setActive(bool a)
{
	alarm_time_page->setActive(a);

	active = a;
	if (active)
	{
		if (!check_alarm_timer)
		{
			check_alarm_timer = new QTimer(this);
			check_alarm_timer->start(200);
			connect(check_alarm_timer, SIGNAL(timeout()), this, SLOT(checkAlarm()));
		}
	}
	else
	{
		if (check_alarm_timer)
		{
			check_alarm_timer->stop();
			disconnect(check_alarm_timer, SIGNAL(timeout()), this, SLOT(checkAlarm()));
			delete check_alarm_timer;
			check_alarm_timer = NULL;
		}
	}

#ifdef CONFIG_TS_3_5
	setCfgValue("enabled", active, SET_ALARMCLOCK, serial_number);
#else
	setCfgValue("enabled", active, item_id);
#endif
}

void AlarmClock::valueReceived(const DeviceValues &values_list)
{
	if (values_list.contains(AlarmSoundDiffDevice::DIM_STATUS))
	{
		bool status = values_list[AlarmSoundDiffDevice::DIM_STATUS].toBool();
		int amplifier = values_list[AlarmSoundDiffDevice::DIM_AMPLIFIER].toInt();

		if (status)
		{
			int volume = values_list[AlarmSoundDiffDevice::DIM_VOLUME].toInt();

			alarm_volumes[amplifier] = volume;
		}
		else
			alarm_volumes[amplifier] = -1;
	}

	if (values_list.contains(AlarmSoundDiffDevice::DIM_SOURCE))
	{
		source = values_list[AlarmSoundDiffDevice::DIM_SOURCE].toInt();
		dev->requestStation(source);
	}
	if (values_list.contains(AlarmSoundDiffDevice::DIM_RADIO_STATION))
		station = values_list[AlarmSoundDiffDevice::DIM_RADIO_STATION].toInt();
}

bool AlarmClock::eventFilter(QObject *obj, QEvent *ev)
{
	// Discard the mouse press and mouse double click
	if (ev->type() == QEvent::MouseButtonPress || ev->type() == QEvent::MouseButtonDblClick)
		return true;

	if (ev->type() != QEvent::MouseButtonRelease)
		return false;

	// We stop the alarm and restore the normal behaviour
	qApp->removeEventFilter(this);
	// must reset forceOperativeMode before stopAlarm(), otherwise video playback
	// will not restart correctly
	bt_global::display->forceOperativeMode(false);
	stopAlarm();
	return true;
}

void AlarmClock::checkAlarm()
{
	if (!active)
		return;

	QDateTime current = QDateTime::currentDateTime();

	bool ring_alarm = false;
	bool ring_once = false;

	if (alarm_days[current.date().dayOfWeek() - 1])
		ring_alarm = true;
	else
	{
		// If the user enter a time without a day, ring the alarm once the current
		// day (or the next, if the time entered is before the current time) at
		// the established time.
		ring_once = true;
		ring_alarm = true;

		for (int i = 0; i < alarm_days.length(); ++i)
			if (alarm_days[i])
			{
				ring_alarm = false;
				break;
			}
	}

	if (ring_alarm)
	{
		if ((current.time() >= alarm_time) && (alarm_time.secsTo(current.time()) < 60))
		{
			ringAlarm();
			if (ring_once)
				setActive(false);
		}
	}

	if (active)
		check_alarm_timer->start((60 - current.time().second()) * 1000);
}

void AlarmClock::ringAlarm()
{
	if (alarm_type == BUZZER)
	{
#ifdef LAYOUT_TS_10
		bt_global::audio_states->toState(AudioStates::ALARM_TO_SPEAKER);
#endif

#ifdef BT_HARDWARE_TS_10
		timer_increase_volume = new QTimer(this);
		timer_increase_volume->start(5000);
		connect(timer_increase_volume, SIGNAL(timeout()), SLOT(wavAlarm()));
#else
		timer_increase_volume = new QTimer(this);
		timer_increase_volume->start(100);
		connect(timer_increase_volume, SIGNAL(timeout()), SLOT(buzzerAlarm()));
#endif
		buzzer_counter = 0;
		conta2min = 0;
	}
	else if (alarm_type == SOUND_DIFF)
	{
		if (dev->isValid(source, station, alarm_volumes))
		{
			timer_increase_volume = new QTimer(this);
			timer_increase_volume->start(3000);
			connect(timer_increase_volume, SIGNAL(timeout()), SLOT(sounddiffusionAlarm()));
			conta2min = 0;
		}
		else
			qWarning() << "Invalid source data for alarm, failed to start";
	}
	else
		qFatal("Unknown alarm clock type!");

	// When the alarm ring we have to put the light on (like in the
	// operative mode) but with a screen "locked" (like in the freezed
	// mode). We do that using an event filter.
	bt_global::display->freeze(false); // To stop a screensaver, if running
	bt_global::display->forceOperativeMode(true); // Prevent the screeensaver start
	qApp->installEventFilter(this);
	bt_global::status.alarm_clock_on = true;

	qDebug("Starting alarm clock");
}

bool AlarmClock::isActive()
{
	return active;
}

void AlarmClock::sounddiffusionAlarm()
{
	if (conta2min == 0)
	{
		dev->startAlarm(SoundDiffusionPage::isMultichannel(), source, station, alarm_volumes);
		conta2min = 9;
	}

	conta2min++;
	if (conta2min < 32)
	{
		for (int idx = 0; idx < AMPLI_NUM; idx++)
		{
			if (alarm_volumes[idx] >= conta2min)
				dev->setVolume(idx, conta2min);
		}
	}
	else if (conta2min > 49)
	{
		dev->stopAlarm(source, alarm_volumes);

		// must reset forceOperativeMode before stopAlarm(), otherwise video playback
		// will not restart correctly
		bt_global::display->forceOperativeMode(false);
		alarmTimeout();
	}
}

void AlarmClock::buzzerAlarm()
{
	if (buzzer_counter == 0)
	{
		buzzer_enabled = getBeep();
		setBeep(true);
	}
	if  (buzzer_counter % 2 == 0)
	{
		if (((buzzer_counter + 2) % 12) && (buzzer_counter % 12))
			beep(10);
	}

	if (buzzer_counter % 8 == 0)
		bt_global::display->changeBrightness(DISPLAY_OPERATIVE);
	else
		bt_global::display->changeBrightness(DISPLAY_FREEZED);

	buzzer_counter++;
	if (buzzer_counter >= 10*60*2)
	{
		setBeep(buzzer_enabled);

		alarmTimeout();
	}
}

void AlarmClock::wavAlarm()
{
	bt_global::sound->play(SOUND_PATH "alarm.wav");

	buzzer_counter++;
	if (buzzer_counter >= 24)
		alarmTimeout();
}

void AlarmClock::alarmTimeout()
{
	qDebug("Alarm clock timeout");

	// stop alarm timer
	timer_increase_volume->stop();
	delete timer_increase_volume;
	timer_increase_volume = NULL;

#ifdef LAYOUT_TS_10
	if (alarm_type == BUZZER)
		bt_global::audio_states->removeState(AudioStates::ALARM_TO_SPEAKER);
#endif

	// restore display state
	bt_global::display->freeze(false);
	bt_global::status.alarm_clock_on = false;
	bt_global::display->forceOperativeMode(false);

	emit alarmClockFired();
}

void AlarmClock::stopAlarm()
{
	// should never happen
	if (!timer_increase_volume || !timer_increase_volume->isActive())
		return;

	qDebug("Stopping alarm clock");
	timer_increase_volume->stop();
#ifdef BT_HARDWARE_TS_3_5
	if (alarm_type == BUZZER)
		setBeep(buzzer_enabled);
#endif
#ifdef LAYOUT_TS_10
	if (alarm_type == BUZZER)
		bt_global::audio_states->removeState(AudioStates::ALARM_TO_SPEAKER);
#endif

	delete timer_increase_volume;
	timer_increase_volume = NULL;
	bt_global::status.alarm_clock_on = false;

	emit alarmClockFired();
}

void AlarmClock::setSerNum(int s)
{
	serial_number = s;
}

void AlarmClock::inizializza()
{
	getAlarmVolumes(serial_number-1, alarm_volumes, &source, &station);
}


AlarmClockTime::AlarmClockTime(QTime time)
{
	NavigationBar *nav_bar = new NavigationBar("forward");
	nav_bar->displayScrollButtons(false);
	connect(nav_bar, SIGNAL(backClick()), SIGNAL(Closed()));
	connect(nav_bar, SIGNAL(forwardClick()), SIGNAL(forwardClick()));

	QLabel *icon = new QLabel;
	icon->setPixmap(bt_global::skin->getImage("alarm_icon"));

	alarm_time = time;
	edit = new BtTimeEdit(this);
	edit->setTime(alarm_time);

	QWidget *content = new QWidget;
	QVBoxLayout *main_layout = new QVBoxLayout(content);

	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(5);

	QHBoxLayout *r = new QHBoxLayout;
	r->setContentsMargins(0, 0, 0, 0);
	r->setSpacing(0);

	r->addSpacing(40);
	r->addWidget(edit);
	r->addSpacing(40);

	// top level layout
	main_layout->addWidget(icon, 0, Qt::AlignHCenter);
	main_layout->addSpacing(10);
	main_layout->addLayout(r);

	buildPage(content, nav_bar);
}

QTime AlarmClockTime::getAlarmTime() const
{
	BtTime t = edit->time();

	return QTime(t.hour(), t.minute());
}

void AlarmClockTime::saveAlarmTime()
{
	alarm_time = getAlarmTime();
}

void AlarmClockTime::resetAlarmTime()
{
	edit->setTime(alarm_time);
}


BannAlarmDay::BannAlarmDay(QString img_off, QString img_on, QString descr)
{
	initBanner(img_off, QString(), descr);
	left_button->setOffImage(img_off);
	left_button->setOnImage(img_on);
	connect(left_button, SIGNAL(clicked()), SLOT(toggleStatus()));
	setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

void BannAlarmDay::toggleStatus()
{
	left_button->setStatus(left_button->getStatus() == StateButton::OFF);
}

void BannAlarmDay::setStatus(bool st)
{
	left_button->setStatus(st);
}

bool BannAlarmDay::getStatus() const
{
	return left_button->getStatus() == StateButton::ON;
}


AlarmClockDays::AlarmClockDays(AlarmClock::Type type, QList<bool> days)
{
	NavigationBar *nav_bar =  new NavigationBar(type == AlarmClock::SOUND_DIFF ? "forward" : "ok");
	nav_bar->displayScrollButtons(true);

	buildPage(new BannerContent, nav_bar);
	setSpacing(10);

	QStringList days_description;
	days_description << tr("Monday") << tr("Tuesday") << tr("Wednesday") << tr("Thursday")
		<< tr("Friday") << tr("Saturday") << tr("Sunday");

	alarm_days = days;
	for (int i = 0; i < 7; ++i)
	{
		BannAlarmDay *b = new BannAlarmDay(bt_global::skin->getImage("day_off"),
			bt_global::skin->getImage("day_on"), days_description[i]);

		b->setStatus(days[i]);
		page_content->appendBanner(b);
	}
}

QList<bool> AlarmClockDays::getAlarmDays() const
{
	QList<bool> days;

	for (int i = 0; i < 7; ++i)
		days.append(static_cast<BannAlarmDay*>(page_content->getBanner(i))->getStatus());

	return days;
}

void AlarmClockDays::saveAlarmDays()
{
	alarm_days = getAlarmDays();
}

void AlarmClockDays::resetAlarmDays()
{
	for (int i = 0; i < 7; ++i)
		static_cast<BannAlarmDay*>(page_content->getBanner(i))->setStatus(alarm_days[i]);
}


void AlarmClockSoundDiff::showPage()
{
	Page *difson = SoundDiffusionPage::alarmClockPage();
	disconnect(difson, SIGNAL(Closed()), 0, 0);
	disconnect(difson, SIGNAL(saveVolumes()), 0, 0);
	connect(difson, SIGNAL(Closed()), SIGNAL(Closed()));
	connect(difson, SIGNAL(saveVolumes()), SIGNAL(saveVolumes()));

	difson->showPage();
}


AlarmClockTimeDays::AlarmClockTimeDays(QTime time, AlarmClock::Type type, QList<bool> days)
{
	static const char *day_labels[] = {QT_TR_NOOP("Mon"),
					   QT_TR_NOOP("Tue"),
					   QT_TR_NOOP("Wed"),
					   QT_TR_NOOP("Thu"),
					   QT_TR_NOOP("Fri"),
					   QT_TR_NOOP("Sat"),
					   QT_TR_NOOP("Sun")};

	QWidget *content = new QWidget;
	QVBoxLayout *main = new QVBoxLayout(content);

	NavigationBar *nav = new NavigationBar;
	nav->displayScrollButtons(false);
	buildPage(content, nav, tr("Wake up"), SMALL_TITLE_HEIGHT);

	connect(nav, SIGNAL(backClick()), SIGNAL(Closed()));

	alarm_label = new QLabel;
	alarm_icon = bt_global::skin->getImage("alarm_icon");

	QLabel *descr = new QLabel;
	descr->setText(tr("Set Time-Hour & Minute-Day"));
	descr->setAlignment(Qt::AlignHCenter);
	descr->setFont(bt_global::font->get(FontManager::TEXT));

	alarm_time = time;
	edit = new BtTimeEdit(this);
	edit->setTime(alarm_time);

	QHBoxLayout *top_layout = new QHBoxLayout;
	QGridLayout *days_layout = new QGridLayout;
	QVBoxLayout *icon_label = new QVBoxLayout;

	main->setContentsMargins(25, 0, 25, 18);
	main->setSpacing(12);
	top_layout->setSpacing(5);
	days_layout->setSpacing(5);

	icon_label->addWidget(alarm_label, 0, Qt::AlignTop|Qt::AlignHCenter);
	icon_label->addWidget(descr, 0, Qt::AlignHCenter);

	top_layout->addLayout(icon_label, 1);
	top_layout->addWidget(edit, 1);
	if (type == AlarmClock::SOUND_DIFF)
	{
		BtButton *go_difson = new BtButton(bt_global::skin->getImage("goto_sounddiffusion"));
		connect(go_difson, SIGNAL(clicked()), SIGNAL(showSoundDiffusion()));

		top_layout->addWidget(go_difson, 1, Qt::AlignCenter);
	}
	else
		top_layout->addStretch(1);

	alarm_days = days;
	for (int i = 0; i < 7; ++i)
	{
		StateButton *toggle = new StateButton;
		toggle->setCheckable(true);
		toggle->setOffImage(bt_global::skin->getImage("day_off"));
		toggle->setOnImage(bt_global::skin->getImage("day_on"));
		toggle->setStatus(days[i]);
		toggle->setChecked(days[i]);

		buttons[i] = toggle;

		QLabel *day = new QLabel(tr(day_labels[i]));
		day->setAlignment(Qt::AlignHCenter);

		days_layout->addWidget(toggle, 0, i);
		days_layout->addWidget(day, 1, i);
	}

	BtButton *ok = new BtButton(bt_global::skin->getImage("ok"));
	connect(ok, SIGNAL(clicked()), SIGNAL(okClicked()));

	main->addLayout(top_layout);
	main->addLayout(days_layout);
	main->addWidget(ok, 0, Qt::AlignRight | Qt::AlignBottom);
}

QTime AlarmClockTimeDays::getAlarmTime() const
{
	BtTime t = edit->time();

	return QTime(t.hour(), t.minute());
}

void AlarmClockTimeDays::saveAlarmTime()
{
	alarm_time = getAlarmTime();
}

void AlarmClockTimeDays::resetAlarmTime()
{
	edit->setTime(alarm_time);
}

QList<bool> AlarmClockTimeDays::getAlarmDays() const
{
	QList<bool> days;

	for (int i = 0; i < 7; ++i)
		days.append(buttons[i]->isChecked());

	return days;
}

void AlarmClockTimeDays::saveAlarmDays()
{
	alarm_days = getAlarmDays();
}

void AlarmClockTimeDays::resetAlarmDays()
{
	for (int i = 0; i < 7; ++i)
	{
		buttons[i]->setStatus(alarm_days[i]);
		buttons[i]->setChecked(alarm_days[i]);
	}
}

void AlarmClockTimeDays::setActive(bool active)
{
	alarm_label->setPixmap(getBostikName(alarm_icon, active ? "on" : "off"));
}
