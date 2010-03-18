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


#include "bann_settings.h"
#include "version.h"
#include "alarmclock.h"
#include "keypad.h"
#include "multisounddiff.h" // contdiff
#include "contrast.h"
#include "generic_functions.h" // setCfgValue
#include "hardware_functions.h" // setBeep, getBeep, beep, setContrast, getContrast
#include "btmain.h" // bt_global::btmain
#include "state_button.h"
#include "fontmanager.h"
#include "skinmanager.h" // bt_global::skin
#if !defined(BT_HARDWARE_X11)
#include "calibration.h"
#endif

#include <QTimer>
#include <QDebug>
#include <QLabel>



bannAlarmClock::bannAlarmClock(int item_id, int hour, int minute, QString icon_on,
			       QString icon_off, QString icon_label, QString text, int enabled, int tipo, int freq) :
	Bann2StateButtons(0)
{
	initBanner(icon_on, icon_label, text);

	left_button->setOnOff();
	left_button->setOnImage(icon_on);
	left_button->setOffImage(icon_off);
	left_button->setStatus(enabled == 1);

	alarm_clock = new AlarmClock(SET_SVEGLIA, item_id,
				     static_cast<AlarmClock::Type>(tipo),
				     static_cast<AlarmClock::Freq>(freq), QList<bool>(),
				     hour, minute);
	alarm_clock->setSerNum(getSerNum());
	alarm_clock->hide();

	alarm_clock->setActive(enabled == 1);
	connect(this, SIGNAL(rightClicked()), alarm_clock, SLOT(showPage()));
	connect(this, SIGNAL(leftClicked()), SLOT(toggleAbil()));

	connect(alarm_clock, SIGNAL(Closed()), SLOT(handleClose()));
	connect(alarm_clock, SIGNAL(alarmClockFired()), SLOT(setButtonIcon()));
}

void bannAlarmClock::setSerNum(int num)
{
	banner::setSerNum(num);
	alarm_clock->setSerNum(num);
}

void bannAlarmClock::handleClose()
{
	// When the page of the alarmclock is closed, the alarm is always set as 'on'.
	left_button->setStatus(true);
	emit pageClosed();
}

void bannAlarmClock::setButtonIcon()
{
	if (!alarm_clock->isActive())
		left_button->setStatus(false);
}

void bannAlarmClock::setAbil(bool b)
{
	left_button->setStatus(b);
	alarm_clock->setActive(b);
}

void bannAlarmClock::toggleAbil()
{
	setAbil(!alarm_clock->isActive());
}

void bannAlarmClock::inizializza(bool forza)
{
	Bann2Buttons::inizializza(forza);
	alarm_clock->inizializza();
}


bannAlarmClockIcon::bannAlarmClockIcon(int item_id, int hour, int minute, QString icon_on,
				       QString icon_off, QString icon_state, QString icon_edit, QString text, int enabled, int tipo, QList<bool> days) :
	BannOnOffState(0, static_cast<StateButton*>(0))
{
	initBanner(icon_on, icon_state, icon_edit, enabled ? ON : OFF, text);

	left_button = static_cast<StateButton *>(Bann2Buttons::left_button);
	left_button->setOnOff();
	left_button->setOnImage(icon_off);
	left_button->setOffImage(icon_on);
	left_button->setStatus(enabled == 1);

	alarm_clock = new AlarmClock(SET_SVEGLIA_SINGLEPAGE, item_id,
				     static_cast<AlarmClock::Type>(tipo),
				     AlarmClock::NESSUNO,
				     days, hour, minute);
	alarm_clock->setSerNum(getSerNum());
	alarm_clock->hide();

	alarm_clock->setActive(enabled == 1);
	connect(right_button, SIGNAL(clicked()), alarm_clock, SLOT(showPage()));
	connect(left_button, SIGNAL(clicked()), SLOT(toggleAbil()));

	connect(alarm_clock,SIGNAL(Closed()), SLOT(handleClose()));
	connect(alarm_clock, SIGNAL(alarmClockFired()), SLOT(setButtonIcon()));
}

void bannAlarmClockIcon::setSerNum(int num)
{
	banner::setSerNum(num);
	alarm_clock->setSerNum(num);
}

void bannAlarmClockIcon::handleClose()
{
	// When the page of the alarmclock is closed, the alarm is always set as 'on'.
	left_button->setStatus(true);
	setState(ON);
	emit pageClosed();
}

void bannAlarmClockIcon::setButtonIcon()
{
	if (!alarm_clock->isActive())
		left_button->setStatus(false);
}

void bannAlarmClockIcon::setAbil(bool b)
{
	setState(b ? ON : OFF);
	left_button->setStatus(b);
	alarm_clock->setActive(b);
}

void bannAlarmClockIcon::toggleAbil()
{
	setAbil(!alarm_clock->isActive());
}

void bannAlarmClockIcon::inizializza(bool forza)
{
	BannOnOffState::inizializza(forza);
	alarm_clock->inizializza();
}



calibration::calibration(QWidget *parent, QString icon) : bannOnDx(parent)
{
	SetIcons(icon, 1);
#if !defined(BT_HARDWARE_X11)
	Calibration *cal = new Calibration;
	connect(cal, SIGNAL(Closed()), this, SIGNAL(endCalibration()));
	connect(this, SIGNAL(click()), this, SIGNAL(startCalibration()));
	connect(this, SIGNAL(click()), cal, SLOT(showWindow()));
#endif
}


impBeep::impBeep(int _item_id, bool enabled, QString icon_on, QString icon_off, QString text) :
	Bann2StateButtons(0)
{
	initBanner(icon_on, QString(), text);

	left_button->setOnOff();
	left_button->setOffImage(icon_off);
	left_button->setOnImage(icon_on);
	left_button->setStatus(enabled);

	item_id = _item_id;

	connect(left_button, SIGNAL(clicked()), SLOT(toggleBeep()));

	setBeep(enabled);
}

void impBeep::toggleBeep()
{
	bool beep_on = !getBeep();

	setBeep(beep_on);
	left_button->setStatus(beep_on);

#ifdef CONFIG_BTOUCH
	setCfgValue("value", beep_on, SUONO);
#else
	setCfgValue("enabled", beep_on, item_id);
#endif

	if (beep_on)
		beep();
}


bannContrast::bannContrast(int _item_id, int val, QString icon) :
	bannOnDx(0, icon, new Contrast())
{
	item_id = _item_id;
	setContrast(val);
	connect(linked_dx_page, SIGNAL(Closed()), SLOT(done()));
}

void bannContrast::done()
{
	int c = getContrast();

	setContrast(c);
	setCfgValue("value", c, item_id); // TODO check if "value" is correct when removing CONFIG_BTOUCH
}


bannVersion::bannVersion(QWidget *parent, QString icon, Version *ver)
	: bannOnDx(parent, icon)
{
	connect(this, SIGNAL(click()), this, SLOT(showVers()));
	v = ver;
}

void bannVersion::showVers()
{
	v->showPage();
	QTimer::singleShot(10000, this, SIGNAL(pageClosed()));
}


impPassword::impPassword(QString icon_on, QString icon_off, QString icon_label, QString descr,
			 int _item_id, QString pwd, int attiva) :
	Bann2StateButtons(0)
{
	item_id = _item_id;
	password = pwd;
	tasti = new Keypad();
	if (password.isEmpty())
	{
		status = PASSWD_NOT_SET;
		tasti->setMode(Keypad::CLEAN);
	}
	else
	{
		status = PASSWD_SET;
		tasti->setMode(Keypad::HIDDEN);
	}
	initBanner(icon_off, icon_label, descr);

	connect(right_button, SIGNAL(clicked()), tasti, SLOT(showPage()));
	connect(left_button, SIGNAL(clicked()), SLOT(toggleActivation()));
	connect(tasti, SIGNAL(Closed()), SLOT(checkPasswd()));

	active = (attiva == 1);
	bt_global::btmain->setPwd(active, password);

	left_button->setOnOff();
	left_button->setOffImage(icon_off);
	left_button->setOnImage(icon_on);
	left_button->setStatus(active);
}

void impPassword::toggleActivation()
{
	active = !active;
#ifdef CONFIG_BTOUCH
	setCfgValue("enabled", QString::number(active), item_id);
#else
	setCfgValue("actived", active, item_id);
#endif
	bt_global::btmain->setPwd(active, password);
	left_button->setStatus(active);
}

void impPassword::showEvent(QShowEvent *event)
{
	qDebug() << "password = " << password;
	if (password.isEmpty())
	{
		qDebug("password = ZERO");
		status = PASSWD_NOT_SET;
	}
	else
	{
		status = PASSWD_SET;
		tasti->setMode(Keypad::HIDDEN);
		qDebug("password not ZERO");
	}
}

void impPassword::checkPasswd()
{
	QString c = tasti->getText();
	tasti->resetText();
	if (status == PASSWD_NOT_SET)
	{
		if (!c.isEmpty())
		{
			password = c;
#ifdef CONFIG_BTOUCH
			setCfgValue("value", password, item_id);
#else
			setCfgValue("password", password, item_id);
#endif
			bt_global::btmain->setPwd(active, password);
			status = PASSWD_SET;
		}
		emit pageClosed();
	}
	else // status == PASSWD_SET
	{
		if (c.isEmpty())
		{
			emit pageClosed();
			return;
		}
		if (password != c)
		{
			qDebug() << "password errata doveva essere " << password;
			sb = getBeep();
			setBeep(true);
			beep(1000);
			QTimer::singleShot(1100, this, SLOT(restoreBeepState()));
			emit pageClosed();
		}
		else //password is correct
		{
			tasti->setMode(Keypad::CLEAN);
			qDebug("password giusta");
			status = PASSWD_NOT_SET;
			tasti->showPage();
		}
	}
}

void impPassword::restoreBeepState()
{
	setBeep(sb);
}



BannRingtone::BannRingtone(const QString &descr, RingtoneType type) :
	Bann2CentralButtons(false)
{
	// TODO: this is necessary because of a setSpacing(5) in Bann2CentralButtons,
	// that will be removed when all borders are removed from images.
	initBanner(bt_global::skin->getImage("prev_ring"), bt_global::skin->getImage("next_ring"), descr);
	connect(center_left, SIGNAL(clicked()), SLOT(minusClicked()));
	connect(center_right, SIGNAL(clicked()), SLOT(plusClicked()));

	current_ring = bt_global::ringtones->getRingtone(type);
}

void BannRingtone::minusClicked()
{
	--current_ring;
	if (current_ring < 0)
		current_ring = bt_global::ringtones->getRingtonesNumber() - 1;
	bt_global::ringtones->playRingtone(current_ring);
}

void BannRingtone::plusClicked()
{
	current_ring = (current_ring + 1) % bt_global::ringtones->getRingtonesNumber();
	bt_global::ringtones->playRingtone(current_ring);
}
