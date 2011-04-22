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
#include "audiostatemachine.h"
#include "devices_cache.h" // bt_global::add_device_to_cache
#include "mediaplayer.h" // bt_global::sound

#ifdef LAYOUT_TS_10
#include "sounddiffusionpage.h" // alarmClockPage
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



AlarmNavigation::AlarmNavigation(bool forwardButton, QWidget *parent)
	: AbstractNavigationBar(parent)
{
	// For now, it used only in TS 3.5'' code.
#ifdef LAYOUT_TS_3_5
	createButton(bt_global::skin->getImage("ok"), SIGNAL(okClicked()), 0);
	if (forwardButton)
		createButton(bt_global::skin->getImage("forward"), SIGNAL(forwardClicked()), 3);
#endif
}


AlarmClock::AlarmClock(int config_id, int _item_id, Type type, Freq freq, int days_active, int hour, int minute)
{
	id = config_id;
	item_id = _item_id;
	aumVolTimer = NULL;
	alarm_time = QTime(hour, minute);
	ring_alarm_timer = NULL;
	alarm_freq = freq;
	alarm_type = type;
	active = false;

	for (int i = 0; i < 7; ++i)
	{
		int mask = 1 << (6 - i);
		alarm_days.append((days_active & mask) ? true : false);
	}

        //left shift to align with software
        lShift(alarm_days);

	for (int i = 0; i < AMPLI_NUM; i++)
		alarm_volumes[i] = -1;

#ifdef LAYOUT_TS_3_5
	alarm_time_page = new AlarmClockTime(alarm_time);
	alarm_type_page = new AlarmClockFreq(alarm_type, alarm_freq);

	connect(alarm_time_page, SIGNAL(forwardClick()), alarm_type_page, SLOT(showPage()));
	connect(alarm_time_page, SIGNAL(okClicked()), this, SLOT(saveAndActivate()));

	connect(alarm_type_page, SIGNAL(forwardClick()), SLOT(showSoundDiffPage()));
	connect(alarm_type_page, SIGNAL(okClicked()), SLOT(saveAndActivate()));
#else
	alarm_time_page = alarm_type_page = new AlarmClockTimeFreq(alarm_time, alarm_type, alarm_days);
	connect(alarm_time_page, SIGNAL(Closed()), SIGNAL(Closed()));
	connect(alarm_time_page, SIGNAL(okClicked()), SLOT(saveAndActivate()));
	connect(alarm_time_page, SIGNAL(showSoundDiffusion()), SLOT(showSoundDiffPage()));
#endif

	if (alarm_type == SOUND_DIFF)
	{
		alarm_sound_diff = new AlarmClockSoundDiff;
		connect(alarm_sound_diff, SIGNAL(Closed()), SLOT(resetVolumes()));
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
	alarm_time_page->showPage();
}

void AlarmClock::saveAndActivate()
{
	setActive(true);
	alarm_time = alarm_time_page->getAlarmTime();
	alarm_freq = alarm_type_page->getAlarmFreq();
	alarm_days = alarm_type_page->getAlarmDays();

	QMap<QString, QString> data;
	data["hour"] = alarm_time.toString("hh");
	data["minute"] = alarm_time.toString("mm");
	if (id == SET_SVEGLIA_SINGLEPAGE)
	{
		int active = 0;
		for (int i = 0; i < 7; ++i)
			if (alarm_days[i])
				active |= 1 << (6 - i);
		data["days"] = QString::number(active);
	}
	else
		data["alarmset"] = QString::number(alarm_freq);

#ifdef CONFIG_TS_3_5
	setCfgValue(data, id, serial_number);
#else
	setCfgValue(data, item_id);
#endif

	emit Closed();
}

void AlarmClock::showSoundDiffPage()
{
	update_eeprom = true;
	sorgente = 1;
	stazione = 0;
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
		setAlarmVolumes(serial_number-1, alarm_volumes, sorgente, stazione);
	update_eeprom = false;
	showPage();
}

void AlarmClock::resetVolumes()
{
	dev->setReceiveFrames(false);
	general->turnOff();
	update_eeprom = false;
	showPage();
}

void AlarmClock::setActive(bool a)
{
	alarm_time_page->setActive(a);

	active = a;
	if (active)
	{
		if (!ring_alarm_timer)
		{
			ring_alarm_timer = new QTimer(this);
			ring_alarm_timer->start(200);
			connect(ring_alarm_timer, SIGNAL(timeout()), this, SLOT(checkAlarm()));
		}
	}
	else
	{
		if (ring_alarm_timer)
		{
			ring_alarm_timer->stop();
			disconnect(ring_alarm_timer, SIGNAL(timeout()), this, SLOT(checkAlarm()));
			delete ring_alarm_timer;
			ring_alarm_timer = NULL;
		}
	}

#ifdef CONFIG_TS_3_5
	setCfgValue("enabled", active, id, serial_number);
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
		sorgente = values_list[AlarmSoundDiffDevice::DIM_SOURCE].toInt();
		dev->requestStation(sorgente);
	}
	if (values_list.contains(AlarmSoundDiffDevice::DIM_RADIO_STATION))
		stazione = values_list[AlarmSoundDiffDevice::DIM_RADIO_STATION].toInt();
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

#ifdef LAYOUT_TS_3_5
	if (alarm_freq == ALWAYS || alarm_freq == ONCE ||
		(alarm_freq == WEEKDAYS && current.date().dayOfWeek() < 6) ||
		(alarm_freq == HOLIDAYS && current.date().dayOfWeek() > 5))
		ring_alarm = true;
#else
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
#endif

	if (ring_alarm)
	{
		if ((current.time() >= alarm_time) && (alarm_time.secsTo(current.time()) < 60))
		{
			if (alarm_type == BUZZER)
			{
				bt_global::audio_states->toState(AudioStates::ALARM_TO_SPEAKER);
#ifdef BT_HARDWARE_TS_10
				aumVolTimer = new QTimer(this);
				aumVolTimer->start(5000);
				connect(aumVolTimer, SIGNAL(timeout()), SLOT(wavAlarm()));
#else
				aumVolTimer = new QTimer(this);
				aumVolTimer->start(100);
				connect(aumVolTimer, SIGNAL(timeout()), SLOT(buzzerAlarm()));
#endif
				contaBuzzer = 0;
				conta2min = 0;
			}
			else if (alarm_type == SOUND_DIFF)
			{
				if (dev->isValid(sorgente, stazione, alarm_volumes))
				{
					aumVolTimer = new QTimer(this);
					aumVolTimer->start(3000);
					connect(aumVolTimer,SIGNAL(timeout()), SLOT(sounddiffusionAlarm()));
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

			if (alarm_freq == ONCE || ring_once)
				setActive(false);
		}
	}

	if (active)
		ring_alarm_timer->start((60 - current.time().second()) * 1000);
}

bool AlarmClock::isActive()
{
	return active;
}

void AlarmClock::sounddiffusionAlarm()
{
	if (conta2min == 0)
	{
#ifdef LAYOUT_TS_10
		// TODO fix sound diffusion for TS 3.5''
		dev->startAlarm(SoundDiffusionPage::isMultichannel(), sorgente, stazione, alarm_volumes);
#endif
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
		dev->stopAlarm(sorgente, alarm_volumes);

		// must reset forceOperativeMode before stopAlarm(), otherwise video playback
		// will not restart correctly
		bt_global::display->forceOperativeMode(false);
		alarmTimeout();
	}
}

void AlarmClock::buzzerAlarm()
{
	if (contaBuzzer == 0)
	{
		buzAbilOld = getBeep();
		setBeep(true);
	}
	if  (contaBuzzer % 2 == 0)
	{
		if (((contaBuzzer + 2) % 12) && (contaBuzzer % 12))
			beep(10);
	}

	if (contaBuzzer % 8 == 0)
		bt_global::display->changeBrightness(DISPLAY_OPERATIVE);
	else
		bt_global::display->changeBrightness(DISPLAY_FREEZED);

	contaBuzzer++;
	if (contaBuzzer >= 10*60*2)
	{
		setBeep(buzAbilOld);

		alarmTimeout();
	}
}

void AlarmClock::wavAlarm()
{
	bt_global::sound->play(SOUND_PATH "alarm.wav");

	contaBuzzer++;
	if (contaBuzzer >= 24)
		alarmTimeout();
}

void AlarmClock::alarmTimeout()
{
	qDebug("Alarm clock timeout");

	// stop alarm timer
	aumVolTimer->stop();
	delete aumVolTimer;
	aumVolTimer = NULL;

	if (alarm_type == BUZZER)
		bt_global::audio_states->removeState(AudioStates::ALARM_TO_SPEAKER);

	// restore display state
	bt_global::display->freeze(false);
	bt_global::status.alarm_clock_on = false;

	emit alarmClockFired();
}

void AlarmClock::stopAlarm()
{
	// should never happen
	if (!aumVolTimer || !aumVolTimer->isActive())
		return;

	qDebug("Stopping alarm clock");
	aumVolTimer->stop();
#ifdef BT_HARDWARE_TS_3_5
	if (alarm_type == BUZZER)
		setBeep(buzAbilOld);
#endif
	if (alarm_type == BUZZER)
		bt_global::audio_states->removeState(AudioStates::ALARM_TO_SPEAKER);

	delete aumVolTimer;
	aumVolTimer = NULL;
	bt_global::status.alarm_clock_on = false;

	emit alarmClockFired();
}

void AlarmClock::setSerNum(int s)
{
	serial_number = s;
}

void AlarmClock::inizializza()
{
	getAlarmVolumes(serial_number-1, alarm_volumes, &sorgente, &stazione);
}


AlarmClockTime::AlarmClockTime(QTime alarm_time)
{
	AlarmNavigation *navigation = new AlarmNavigation(true);

	QLabel *icon = new QLabel;
	icon->setPixmap(bt_global::skin->getImage("alarm_icon"));

	edit = new BtTimeEdit(this);
	edit->setTime(alarm_time);

	QWidget *content = new QWidget;
	QVBoxLayout *l = new QVBoxLayout(content);
	QHBoxLayout *r = new QHBoxLayout;

	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	r->setContentsMargins(0, 0, 0, 0);
	r->setSpacing(0);

	r->addSpacing(40);
	r->addWidget(edit);
	r->addSpacing(40);

	// top level layout
	l->addWidget(icon, 0, Qt::AlignHCenter);
	l->addSpacing(10);
	l->addLayout(r);

	connect(navigation, SIGNAL(okClicked()), SIGNAL(okClicked()));
	connect(navigation, SIGNAL(forwardClicked()), SIGNAL(forwardClick()));

	buildPage(content, navigation);
}

QTime AlarmClockTime::getAlarmTime() const
{
	BtTime t = edit->time();

	return QTime(t.hour(), t.minute());
}


AlarmClockFreq::AlarmClockFreq(AlarmClock::Type type, AlarmClock::Freq freq)
{
	AlarmNavigation *navigation = new AlarmNavigation(type == AlarmClock::SOUND_DIFF);

	content = new SingleChoiceContent;
	content->addBanner(SingleChoice::createBanner(tr("once")), AlarmClock::ONCE);
	content->addBanner(SingleChoice::createBanner(tr("always")), AlarmClock::ALWAYS);
	content->addBanner(SingleChoice::createBanner(tr("mon-fri")), AlarmClock::WEEKDAYS);
	content->addBanner(SingleChoice::createBanner(tr("sat-sun")), AlarmClock::HOLIDAYS);

	connect(content, SIGNAL(bannerSelected(int)), SLOT(setSelection(int)));
	connect(navigation, SIGNAL(forwardClicked()), SIGNAL(forwardClick()));
	connect(navigation, SIGNAL(okClicked()), SIGNAL(okClicked()));

	content->setCheckedId(freq);

	buildPage(content, navigation);
}

void AlarmClockFreq::setSelection(int freq)
{
	frequency = AlarmClock::Freq(freq);
}

AlarmClock::Freq AlarmClockFreq::getAlarmFreq() const
{
	return frequency;
}

QList<bool> AlarmClockFreq::getAlarmDays() const
{
	return QList<bool>();
}


void AlarmClockSoundDiff::showPage()
{
#ifdef LAYOUT_TS_10
	Page *difson = SoundDiffusionPage::alarmClockPage();
	disconnect(difson, SIGNAL(Closed()), 0, 0);
	disconnect(difson, SIGNAL(saveVolumes()), 0, 0);
	connect(difson, SIGNAL(Closed()), SIGNAL(Closed()));
	connect(difson, SIGNAL(saveVolumes()), SIGNAL(saveVolumes()));

	difson->showPage();
#else
	Q_ASSERT_X(false, "AlarmClockSoundDiff::showPage", "No sound diffusion alarm clock for TS 3.5'' yet");
#endif
}


AlarmClockTimeFreq::AlarmClockTimeFreq(QTime alarm_time, AlarmClock::Type type, QList<bool> active)
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

	edit = new BtTimeEdit(this);
	edit->setTime(alarm_time);

	QHBoxLayout *top = new QHBoxLayout;
	QGridLayout *days = new QGridLayout;
	QVBoxLayout *icon_label = new QVBoxLayout;

	main->setContentsMargins(25, 0, 25, 18);
	main->setSpacing(12);
	top->setSpacing(5);
	days->setSpacing(5);

	icon_label->addWidget(alarm_label, 0, Qt::AlignTop|Qt::AlignHCenter);
	icon_label->addWidget(descr, 0, Qt::AlignHCenter);

	top->addLayout(icon_label, 1);
	top->addWidget(edit, 1);
	if (type == AlarmClock::SOUND_DIFF)
	{
		BtButton *go_difson = new BtButton(bt_global::skin->getImage("goto_sounddiffusion"));
		connect(go_difson, SIGNAL(clicked()), SIGNAL(showSoundDiffusion()));

		top->addWidget(go_difson, 1, Qt::AlignCenter);
	}
	else
		top->addStretch(1);

	for (int i = 0; i < 7; ++i)
	{
		StateButton *toggle = new StateButton;
		toggle->setCheckable(true);
		toggle->setOffImage(bt_global::skin->getImage("day_off"));
		toggle->setOnImage(bt_global::skin->getImage("day_on"));
		toggle->setStatus(active[i]);
		toggle->setChecked(active[i]);

		buttons[i] = toggle;

		QLabel *day = new QLabel(tr(day_labels[i]));
		day->setAlignment(Qt::AlignHCenter);

		days->addWidget(toggle, 0, i);
		days->addWidget(day, 1, i);
	}

	BtButton *ok = new BtButton(bt_global::skin->getImage("ok"));
	connect(ok, SIGNAL(clicked()), SIGNAL(okClicked()));

	main->addLayout(top);
	main->addLayout(days);
	main->addWidget(ok, 0, Qt::AlignRight | Qt::AlignBottom);
}

QTime AlarmClockTimeFreq::getAlarmTime() const
{
	BtTime t = edit->time();

	return QTime(t.hour(), t.minute());
}

AlarmClock::Freq AlarmClockTimeFreq::getAlarmFreq() const
{
	return AlarmClock::NEVER;
}

QList<bool> AlarmClockTimeFreq::getAlarmDays() const
{
	QList<bool> active;

	for (int i = 0; i < 7; ++i)
		active.append(buttons[i]->isChecked());

	return active;
}

void AlarmClockTimeFreq::setActive(bool active)
{
	alarm_label->setPixmap(getBostikName(alarm_icon, active ? "on" : "off"));
}
