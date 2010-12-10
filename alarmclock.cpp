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
#include "openclient.h"
#include "datetime.h"
#include "fontmanager.h" // bt_global::font
#include "displaycontrol.h" // bt_global::display
#include "btmain.h" // bt_global::btmain
#include "singlechoicepage.h"
#include "skinmanager.h"
#include "media_device.h" // AlarmSoundDiffDevice
#include "navigation_bar.h"
#include "generic_functions.h" // getBostikName
#ifdef LAYOUT_TS_10
#include "sounddiffusionpage.h" // alarmClockPage
#endif
#include "audiostatemachine.h"
#include "devices_cache.h"
#include "mediaplayer.h" // bt_global::sound

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


// Alarmnavigation implementation

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

// AlarmClock implementation

AlarmClock::AlarmClock(int config_id, int _item_id, Type t, Freq f, QList<bool> active, int hour, int minute)
{
	id = config_id;
	item_id = _item_id;
	aumVolTimer = NULL;
	alarmTime = QTime(hour, minute);
	minuTimer = NULL;
	freq = f;
	type = t;
	days = active;

	for (uchar idx = 0; idx < AMPLI_NUM; idx++)
		volSveglia[idx] = -1;

#ifdef LAYOUT_TS_3_5
	alarm_time = new AlarmClockTime(this);
	alarm_type = new AlarmClockFreq(this);
#else
	alarm_time = alarm_type = new AlarmClockTimeFreq(this);
#endif

	if (type == DI_SON)
		alarm_sound_diff = new AlarmClockSoundDiff(this);
	else
		alarm_sound_diff = NULL;

	dev = bt_global::add_device_to_cache(new AlarmSoundDiffDevice());
	general = AmplifierDevice::createDevice("0");

	connect(dev, SIGNAL(valueReceived(DeviceValues)), SLOT(valueReceived(DeviceValues)));
}

void AlarmClock::showPage()
{
	alarm_time->showPage();
}

void AlarmClock::showTypePage()
{
	alarm_type->showPage();
}

void AlarmClock::saveAndActivate()
{
	setActive(true);
	alarmTime = alarm_time->getAlarmTime();
	freq = alarm_type->getAlarmFreq();
	days = alarm_type->getAlarmDays();

	QMap<QString, QString> data;
	data["hour"] = alarmTime.toString("hh");
	data["minute"] = alarmTime.toString("mm");
	if (id == SET_SVEGLIA_SINGLEPAGE)
	{
		int active = 0;
		for (int i = 0; i < 7; ++i)
			if (days[i])
				active |= 1 << (6 - i);
		data["days"] = QString::number(active);
	}
	else
		data["alarmset"] = QString::number(freq);

#ifdef CONFIG_TS_3_5
	setCfgValue(data, id, serNum);
#else
	setCfgValue(data, item_id);
#endif

	emit Closed();
}

void AlarmClock::showSoundDiffPage()
{
	aggiornaDatiEEprom = 1;
	sorgente = 1;
	stazione = 0;
	dev->setReceiveFrames(true);
	for (unsigned idx = 0; idx < AMPLI_NUM; idx++)
		volSveglia[idx] = -1;

	alarm_sound_diff->showPage();
}

void AlarmClock::saveVolumes()
{
	dev->setReceiveFrames(false);
	general->turnOff();

	if (aggiornaDatiEEprom)
		setAlarmVolumes(serNum-1, volSveglia, sorgente, stazione);
	aggiornaDatiEEprom = 0;
	showPage();
}

void AlarmClock::resetVolumes()
{
	dev->setReceiveFrames(false);
	general->turnOff();
	aggiornaDatiEEprom = 0;
	showPage();
}

void AlarmClock::setActive(bool a)
{
	alarm_time->setActive(a);

	active = a;
	if (active)
	{
		if (!minuTimer)
		{
			minuTimer = new QTimer(this);
			minuTimer->start(200);
			connect(minuTimer,SIGNAL(timeout()), this,SLOT(checkAlarm()));
		}
	}
	else
	{
		if (minuTimer)
		{
			minuTimer->stop();
			disconnect(minuTimer,SIGNAL(timeout()), this,SLOT(checkAlarm()));
			delete minuTimer;
			minuTimer = NULL;
		}
	}

#ifdef CONFIG_TS_3_5
	setCfgValue("enabled", active, id, serNum);
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

			volSveglia[amplifier] = volume;
		}
		else
			volSveglia[amplifier] = -1;
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

	QDateTime actualDateTime = QDateTime::currentDateTime();

	bool ring_alarm = false;
#ifdef LAYOUT_TS_3_5
	if (freq == SEMPRE || freq == ONCE ||
		(freq == FERIALI && actualDateTime.date().dayOfWeek() < 6) ||
		(freq == FESTIVI && actualDateTime.date().dayOfWeek() > 5))
		ring_alarm = true;
#else
	if (days[actualDateTime.date().dayOfWeek() - 1])
		ring_alarm = true;
#endif

	if (ring_alarm)
	{
		if ((actualDateTime.time() >= alarmTime) && (alarmTime.secsTo(actualDateTime.time())<60))
		{
			if (type == BUZZER)
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
			else if (type == DI_SON)
			{
				if (dev->isValid(sorgente, stazione, volSveglia))
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
				qFatal("Unknown sveglia type!");

			// When the alarm ring we have to put the light on (like in the
			// operative mode) but with a screen "locked" (like in the freezed
			// mode). We do that with an event filter.
			bt_global::btmain->freeze(false); // To stop a screensaver, if running
			bt_global::display->forceOperativeMode(true); // Prevent the screeensaver start
			qApp->installEventFilter(this);
			bt_global::btmain->alarm_clock_on = true;

			qDebug("Starting alarm clock");

			if (freq == ONCE)
				setActive(false);
		}
	}

	if (active)
		minuTimer->start((60-actualDateTime.time().second())*1000);
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
		dev->startAlarm(SoundDiffusionPage::isMultichannel(), sorgente, stazione, volSveglia);
#endif
		conta2min = 9;
	}

	conta2min++;
	if (conta2min < 32)
	{
		for (int idx = 0; idx < AMPLI_NUM; idx++)
		{
			if (volSveglia[idx] >= conta2min)
				dev->setVolume(idx, conta2min);
		}
	}
	else if (conta2min > 49)
	{
		dev->stopAlarm(sorgente, volSveglia);

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
		bt_global::display->setState(DISPLAY_OPERATIVE);
	else
		bt_global::display->setState(DISPLAY_FREEZED);

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

	if (type == BUZZER)
		bt_global::audio_states->removeState(AudioStates::ALARM_TO_SPEAKER);

	// restore display state
	bt_global::btmain->freeze(false);
	bt_global::btmain->alarm_clock_on = false;

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
	if (type == BUZZER)
		setBeep(buzAbilOld);
#endif
	if (type == BUZZER)
		bt_global::audio_states->removeState(AudioStates::ALARM_TO_SPEAKER);

	delete aumVolTimer;
	aumVolTimer = NULL;
	bt_global::btmain->alarm_clock_on = false;

	emit alarmClockFired();
}

void AlarmClock::setSerNum(int s)
{
	serNum = s;
}

void AlarmClock::inizializza()
{
	getAlarmVolumes(serNum-1, volSveglia, &sorgente, &stazione);
}

// AlarmClockTime implementation

AlarmClockTime::AlarmClockTime(AlarmClock *alarm_page)
{
	AlarmNavigation *navigation = new AlarmNavigation(true);

	QLabel *icon = new QLabel;
	icon->setPixmap(bt_global::skin->getImage("alarm_icon"));

	edit = new BtTimeEdit(this);
	edit->setTime(alarm_page->alarmTime);

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

	connect(navigation, SIGNAL(forwardClicked()), alarm_page, SLOT(showTypePage()));
	connect(navigation, SIGNAL(okClicked()), alarm_page, SLOT(saveAndActivate()));

	buildPage(content, navigation);
}

QTime AlarmClockTime::getAlarmTime() const
{
	BtTime t = edit->time();

	return QTime(t.hour(), t.minute());
}

// AlarmClockFreq implementation

AlarmClockFreq::AlarmClockFreq(AlarmClock *alarm_page)
{
	AlarmNavigation *navigation = new AlarmNavigation(alarm_page->type == AlarmClock::DI_SON);

	content = new SingleChoiceContent;
	content->addBanner(SingleChoice::createBanner(tr("once")), AlarmClock::ONCE);
	content->addBanner(SingleChoice::createBanner(tr("always")), AlarmClock::SEMPRE);
	content->addBanner(SingleChoice::createBanner(tr("mon-fri")), AlarmClock::FERIALI);
	content->addBanner(SingleChoice::createBanner(tr("sat-sun")), AlarmClock::FESTIVI);

	connect(content, SIGNAL(bannerSelected(int)),
		SLOT(setSelection(int)));

	connect(navigation,SIGNAL(forwardClicked()),alarm_page,SLOT(showSoundDiffPage()));
	connect(navigation, SIGNAL(okClicked()), alarm_page, SLOT(saveAndActivate()));

	content->setCheckedId(alarm_page->freq);

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

// AlarmClockSoundDiff

AlarmClockSoundDiff::AlarmClockSoundDiff(AlarmClock *alarm_page)
{
	connect(this, SIGNAL(Closed()), alarm_page, SLOT(resetVolumes()));
	// when confirming the sound diffusion status, also save and activate this alarm
	connect(this, SIGNAL(saveVolumes()), alarm_page, SLOT(saveVolumes()));
	connect(this, SIGNAL(saveVolumes()), alarm_page, SLOT(saveAndActivate()));
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

// AlarmClockTimeFreq implementation

AlarmClockTimeFreq::AlarmClockTimeFreq(AlarmClock *alarm_page)
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

	connect(nav, SIGNAL(backClick()), alarm_page, SIGNAL(Closed()));

	alarm_label = new QLabel;
	alarm_icon = bt_global::skin->getImage("alarm_icon");

	QLabel *descr = new QLabel;
	descr->setText(tr("Set Time-Hour & Minute-Day"));
	descr->setAlignment(Qt::AlignHCenter);

	edit = new BtTimeEdit(this);
	edit->setTime(alarm_page->alarmTime);

	QHBoxLayout *top = new QHBoxLayout;
	QGridLayout *days = new QGridLayout;
	QVBoxLayout *icon_label = new QVBoxLayout;

	main->setContentsMargins(25, 0, 25, 18);
	main->setSpacing(12);
	top->setSpacing(20);
	days->setSpacing(5);

	icon_label->addWidget(alarm_label, 0, Qt::AlignTop|Qt::AlignHCenter);
	icon_label->addWidget(descr, 0, Qt::AlignHCenter);

	top->addLayout(icon_label, 1);
	top->addWidget(edit, 1);
	if (alarm_page->type == AlarmClock::DI_SON)
	{
		BtButton *go_difson = new BtButton(bt_global::skin->getImage("goto_sounddiffusion"));
		connect(go_difson, SIGNAL(clicked()), alarm_page, SLOT(showSoundDiffPage()));

		top->addWidget(go_difson, 1, Qt::AlignCenter);
	}
	else
		top->addStretch(1);

	QList<bool> active = alarm_page->days;

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
	connect(ok, SIGNAL(clicked()), alarm_page, SLOT(saveAndActivate()));

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
	return AlarmClock::NESSUNO;
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
