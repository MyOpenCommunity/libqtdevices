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
#include "contrast.h"
#include "generic_functions.h" // setCfgValue
#include "hardware_functions.h" // setBeep, getBeep, beep, setContrast, getContrast
#include "state_button.h"
#include "fontmanager.h"
#include "skinmanager.h" // bt_global::skin
#include "audiostatemachine.h" // bt_global::audio_states
#include "btmain.h" // bt_global::status
#if !defined(BT_HARDWARE_X11)
#include "calibration.h"
#endif

#include <QTimer>
#include <QDebug>
#include <QLabel>

#ifdef LAYOUT_TS_3_5

BannAlarmClock::BannAlarmClock(int item_id, int hour, int minute, QString icon_on,
	QString icon_off, QString icon_label, QString text, int enabled, int tipo, int freq)
{
	initBanner(icon_on, icon_label, text);

	left_button->setOnImage(icon_on);
	left_button->setOffImage(icon_off);
	left_button->setStatus(enabled == 1);

	alarm_clock = new AlarmClock(SET_ALARMCLOCK, item_id, static_cast<AlarmClock::Type>(tipo),
		static_cast<AlarmClock::Freq>(freq), 0, hour, minute);
	alarm_clock->setSerNum(getSerNum());
	alarm_clock->hide();

	alarm_clock->setActive(enabled == 1);
	connect(this, SIGNAL(rightClicked()), alarm_clock, SLOT(showPage()));
	connect(this, SIGNAL(leftClicked()), SLOT(toggleAbil()));

	connect(alarm_clock, SIGNAL(Closed()), SLOT(handleClose()));
	connect(alarm_clock, SIGNAL(alarmClockFired()), SLOT(setButtonIcon()));
}

void BannAlarmClock::setSerNum(int num)
{
	banner::setSerNum(num);
	alarm_clock->setSerNum(num);
}

void BannAlarmClock::handleClose()
{
	left_button->setStatus(alarm_clock->isActive());
	emit pageClosed();
}

void BannAlarmClock::setButtonIcon()
{
	if (!alarm_clock->isActive())
		left_button->setStatus(false);
}

void BannAlarmClock::setAbil(bool b)
{
	left_button->setStatus(b);
	alarm_clock->setActive(b);
}

void BannAlarmClock::toggleAbil()
{
	setAbil(!alarm_clock->isActive());
}

void BannAlarmClock::inizializza(bool forza)
{
	Bann2Buttons::inizializza(forza);
	alarm_clock->inizializza();
}

#else

BannAlarmClockIcon::BannAlarmClockIcon(int item_id, int hour, int minute, QString icon_on,
					   QString icon_off, QString icon_state, QString icon_edit, QString text, int enabled, int tipo, int days) :
	BannOnOffState(0, static_cast<StateButton*>(0))
{
	initBanner(icon_on, icon_state, icon_edit, enabled ? ON : OFF, text);

	left_button = static_cast<StateButton *>(Bann2Buttons::left_button);
	left_button->setOnImage(icon_off);
	left_button->setOffImage(icon_on);
	left_button->setStatus(enabled == 1);

	alarm_clock = new AlarmClock(SET_SVEGLIA_SINGLEPAGE, item_id,
		static_cast<AlarmClock::Type>(tipo), AlarmClock::NEVER, days, hour, minute);
	alarm_clock->setSerNum(getSerNum());
	alarm_clock->hide();

	alarm_clock->setActive(enabled == 1);
	connect(right_button, SIGNAL(clicked()), alarm_clock, SLOT(showPage()));
	connect(left_button, SIGNAL(clicked()), SLOT(toggleAbil()));

	connect(alarm_clock,SIGNAL(Closed()), SLOT(handleClose()));
	connect(alarm_clock, SIGNAL(alarmClockFired()), SLOT(setButtonIcon()));
}

void BannAlarmClockIcon::setSerNum(int num)
{
	banner::setSerNum(num);
	alarm_clock->setSerNum(num);
}

void BannAlarmClockIcon::handleClose()
{
	left_button->setStatus(alarm_clock->isActive());
	setState(alarm_clock->isActive() ? ON : OFF);
	emit pageClosed();
}

void BannAlarmClockIcon::setButtonIcon()
{
	if (!alarm_clock->isActive())
	{
		left_button->setStatus(false);
		setState(OFF);
	}
}

void BannAlarmClockIcon::setAbil(bool b)
{
	setState(b ? ON : OFF);
	left_button->setStatus(b);
	alarm_clock->setActive(b);
}

void BannAlarmClockIcon::toggleAbil()
{
	setAbil(!alarm_clock->isActive());
}

void BannAlarmClockIcon::inizializza(bool forza)
{
	BannOnOffState::inizializza(forza);
	alarm_clock->inizializza();
}
#endif


BannBeep::BannBeep(int _item_id, bool enabled, QString icon_on, QString icon_off, QString text) :
	Bann2StateButtons(0)
{
	initBanner(icon_on, QString(), text);

	left_button->setOffImage(icon_off);
	left_button->setOnImage(icon_on);
	left_button->setStatus(enabled);

	item_id = _item_id;

	connect(left_button, SIGNAL(clicked()), SLOT(toggleBeep()));

	setBeep(enabled);
}

void BannBeep::toggleBeep()
{
	bool beep_on = !getBeep();

	setBeep(beep_on);
	left_button->setStatus(beep_on);

#ifdef CONFIG_TS_3_5
	setCfgValue("value", beep_on, SET_BEEP);
#else
	setCfgValue("enabled", beep_on, item_id);
#endif

	if (beep_on)
		beep();
}


BannContrast::BannContrast(int _item_id, int val, QString icon, QString descr)
{
	initBanner(QString(), icon, descr);
	connectRightButton(new Contrast);
	item_id = _item_id;
	setContrast(val);
	connect(this, SIGNAL(pageClosed()), SLOT(done()));
}

void BannContrast::done()
{
	int c = getContrast();

	setContrast(c);
	setCfgValue("value", c, item_id); // TODO check if "value" is correct when removing CONFIG_TS_3_5
}


BannVersion::BannVersion(QString icon, QString text, Version *v)
{
	initBanner(QString(), icon, text);
	connect(this, SIGNAL(rightClicked()), SLOT(showVersionPage()));
	version_page = v;
}

void BannVersion::showVersionPage()
{
	version_page->showPage();
	QTimer::singleShot(10000, this, SIGNAL(pageClosed()));
}


PasswordChanger::PasswordChanger(int _item_id, QString pwd, bool check_active)
{
	item_id = _item_id;
	password = pwd;
	tasti = new Keypad();
	if (password.isEmpty())
	{
		setStatus(PASSWD_NOT_SET);
		tasti->setMode(Keypad::CLEAN);
	}
	else
	{
		setStatus(CHECK_OLD_PASSWORD);
		tasti->setMode(Keypad::HIDDEN);
	}

	connect(tasti, SIGNAL(Closed()), SIGNAL(finished()));
	connect(tasti, SIGNAL(Closed()), SLOT(resetState()));
	connect(tasti, SIGNAL(accept()), SLOT(checkPasswd()));

	active = check_active;
	bt_global::status.check_password = active;
	bt_global::status.password = password;
}

void PasswordChanger::changePassword()
{
	tasti->showPage();
}

void PasswordChanger::requestPasswdOn()
{
	setStatus(ASK_PASSWORD);
	tasti->showPage();
}

void PasswordChanger::toggleActivation()
{
	active = !active;
#ifdef CONFIG_TS_3_5
	setCfgValue("enabled", QString::number(active), item_id);
#else
	setCfgValue("actived", active, item_id);
#endif
	bt_global::status.check_password = active;
	bt_global::status.password = password;
	emit passwordActive(active);
}

void PasswordChanger::showEvent(QShowEvent *event)
{
	// TODO: all this thing seems useless...
	if (password.isEmpty())
	{
		setStatus(PASSWD_NOT_SET);
	}
	else
	{
		setStatus(CHECK_OLD_PASSWORD);
		tasti->setMode(Keypad::HIDDEN);
	}
	tasti->resetText();
}

void PasswordChanger::resetState()
{
	setStatus(CHECK_OLD_PASSWORD);
	tasti->resetText();
	tasti->showWrongPassword(false);
}

void PasswordChanger::checkPasswd()
{
	QString c = tasti->getText();
	tasti->resetText();
	switch (status)
	{
	// TODO: understand what must be done when password is not set
	case PASSWD_NOT_SET:
		savePassword(c);
		emit finished();
		break;

	case CHECK_OLD_PASSWORD:
		if (password != c)
		{
			qDebug() << "password errata doveva essere " << password;
			// only beep on error on TS 3.5''
#ifdef LAYOUT_TS_3_5
			sb = getBeep();
			setBeep(true);
			beep(200);
			QTimer::singleShot(1100, this, SLOT(restoreBeepState()));
#else
			tasti->showWrongPassword(true);
#endif
		}
		else //password is correct
		{
			qDebug("Old password correct, insert new password");
			setStatus(INSERT_NEW_PASSWORD);
			tasti->showPage();
		}
		break;

	case INSERT_NEW_PASSWORD:
		new_password = c;
		setStatus(VERIFY_PASSWORD);
		tasti->showPage();
		qDebug("New password: %s", qPrintable(new_password));
		break;

	case VERIFY_PASSWORD:
		if (new_password != c)
		{
			qDebug("Password mismatch, new: %s; repeated: %s", qPrintable(new_password), qPrintable(c));
			tasti->showWrongPassword(true);
			setStatus(INSERT_NEW_PASSWORD);
			tasti->showPage();
		}
		else
		{
			// save password and quit
			savePassword(c);
			emit finished();
		}
		break;

	case ASK_PASSWORD:
		if (c == password)
		{
			toggleActivation();
			setStatus(CHECK_OLD_PASSWORD);
			emit finished();
		}
		else
			tasti->showWrongPassword(true);
		break;
	}
}

void PasswordChanger::setStatus(PasswdStatus _status)
{
	status = _status;

	switch (status)
	{
	case CHECK_OLD_PASSWORD:
		tasti->setMessage(tr("Old password:"));
		break;

	case INSERT_NEW_PASSWORD:
	case PASSWD_NOT_SET:
		tasti->setMessage(tr("New password:"));
		break;

	case VERIFY_PASSWORD:
		tasti->setMessage(tr("Verify password:"));
		break;

	case ASK_PASSWORD:
		tasti->setMessage(tr("Check password:"));
		break;
	}
}

void PasswordChanger::savePassword(const QString &passwd)
{
	if (!passwd.isEmpty())
	{
		password = passwd;
#ifdef CONFIG_TS_3_5
		setCfgValue("value", password, item_id);
#else
		setCfgValue("password", password, item_id);
#endif
		bt_global::status.check_password = active;
		bt_global::status.password = password;
		setStatus(CHECK_OLD_PASSWORD);
	}
}

void PasswordChanger::restoreBeepState()
{
	setBeep(sb);
}


BannPassword::BannPassword(QString icon_on, QString icon_off, QString icon_label, QString descr,
			   int _item_id, QString pwd, int attiva) :
       Bann2StateButtons(0), changer(_item_id, pwd, attiva)
{
	initBanner(icon_off, icon_label, descr);

	left_button->setOffImage(icon_off);
	left_button->setOnImage(icon_on);
	left_button->setStatus(attiva);

	connect(right_button, SIGNAL(clicked()), &changer, SLOT(changePassword()));
	connect(left_button, SIGNAL(clicked()), &changer, SLOT(requestPasswdOn()));
	connect(&changer, SIGNAL(finished()), SIGNAL(pageClosed()));
	connect(&changer, SIGNAL(passwordActive(bool)), left_button, SLOT(setStatus(bool)));
}


BannRingtone::BannRingtone(const QString &descr, int id, Ringtones::Type type, int ring) :
	Bann2CentralButtons(false)
{
	// TODO: this is necessary because of a setSpacing(5) in Bann2CentralButtons,
	// that will be removed when all borders are removed from images.
	initBanner(bt_global::skin->getImage("prev_ring"), bt_global::skin->getImage("next_ring"), descr);
	center_left->enableBeep(false);
	center_right->enableBeep(false);
	connect(center_left, SIGNAL(clicked()), SLOT(minusClicked()));
	connect(center_right, SIGNAL(clicked()), SLOT(plusClicked()));
	current_ring = ring;
	ring_type = type;
	item_id = id;
	// initialize the global object bt_global::ringtones
	bt_global::ringtones->setRingtone(ring_type, item_id, ring);
	is_playing = false;
}

void BannRingtone::ringtoneFinished()
{
	if (is_playing)
	{
		is_playing = false;
		bt_global::audio_states->removeState(AudioStates::PLAY_RINGTONE);
		disconnect(bt_global::ringtones, SIGNAL(ringtoneFinished()), this, SLOT(ringtoneFinished()));
	}
}

void BannRingtone::playRingtone()
{
	if (!is_playing)
	{
		connect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), SLOT(startPlayRingtone()));
		bt_global::audio_states->toState(AudioStates::PLAY_RINGTONE);
	}
	else
		startPlayRingtone();
}

void BannRingtone::startPlayRingtone()
{
	disconnect(bt_global::audio_states, SIGNAL(stateTransition(int,int)), this, SLOT(startPlayRingtone()));
	if (!is_playing)
		connect(bt_global::ringtones, SIGNAL(ringtoneFinished()), SLOT(ringtoneFinished()));
	is_playing = true;

	bt_global::ringtones->playRingtone(current_ring);
	bt_global::ringtones->setRingtone(ring_type, item_id, current_ring);
}

void BannRingtone::minusClicked()
{
	if (--current_ring <= 0)
		current_ring = bt_global::ringtones->getRingtonesNumber();

	playRingtone();
}

void BannRingtone::plusClicked()
{
	if (++current_ring > bt_global::ringtones->getRingtonesNumber())
		current_ring %= bt_global::ringtones->getRingtonesNumber();

	playRingtone();
}


ScreensaverTiming::ScreensaverTiming(const QString &descr, int init_timing, int _delta, int min_timing, int max_timing) :
	Bann2Buttons(0)
{
	initBanner(bt_global::skin->getImage("minus"), bt_global::skin->getImage("plus"), QString::number(init_timing),
		FontManager::BANNERTEXT, descr);
	timing = init_timing;
	if (init_timing < min_timing)
		timing = min_timing;
	if (init_timing > max_timing)
		timing = max_timing;
	delta = _delta;
	max = max_timing;
	min = min_timing;

	connect(right_button, SIGNAL(clicked()), SLOT(increase()));
	connect(left_button, SIGNAL(clicked()), SLOT(decrease()));
	left_button->setAutoRepeat(true);
	right_button->setAutoRepeat(true);
	setFixedWidth(240);
	updateText();
}

void ScreensaverTiming::updateText()
{
	setCentralText(QString::number(timing / 1000) + tr(" sec"));
}

void ScreensaverTiming::increase()
{
	timing += delta;
	// clamp the result
	if (timing > max)
		timing = max;
	updateText();
}

void ScreensaverTiming::decrease()
{
	timing -= delta;
	if (timing < min)
		timing = min;
	updateText();
}

int ScreensaverTiming::getTiming() const
{
	return timing;
}
