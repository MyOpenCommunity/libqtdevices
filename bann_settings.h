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


#ifndef BANN_SETTINGS_H
#define BANN_SETTINGS_H

#include "banner.h"
#include "bann1_button.h" // bannOnDx, bannOnSx
#include "bann2_buttons.h"
#include "ringtonesmanager.h" // RingtoneType

#include <QWidget>

class Version;
class AlarmClock;
class Keypad;
class Contrast;
class contdiff;
class StateButton;


// This class is made to make alarm clock settings.
class bannAlarmClock : public Bann2StateButtons
{
Q_OBJECT
public:
	bannAlarmClock(int item_id, int hour, int minute, QString icon_on,
		QString icon_off, QString icon_label, QString text, int enabled, int freq, int tipo);
	void setAbil(bool);
	void inizializza(bool forza = false);

	virtual void setSerNum(int num);

public slots:
	// changes the alarm set abilitation
	void toggleAbil();
	void setButtonIcon();

private:
	AlarmClock *alarm_clock;

private slots:
	void handleClose();
};


class bannAlarmClockIcon : public BannOnOffState
{
Q_OBJECT
public:
	bannAlarmClockIcon(int item_id, int hour, int minute, QString icon_on,
		QString icon_off, QString icon_state, QString icon_edit, QString text,
		int enabled, int tipo, QList<bool> days);

	void setAbil(bool);
	void inizializza(bool forza = false);

	virtual void setSerNum(int num);

public slots:
	void toggleAbil();
	void setButtonIcon();

protected:
	StateButton *left_button;

private:
	AlarmClock *alarm_clock;

private slots:
	void handleClose();
};


class calibration : public bannOnDx
{
Q_OBJECT
public:
	calibration(QWidget *parent, QString icon);

signals:
	void startCalibration();
	void endCalibration();
};


class impBeep : public Bann2StateButtons
{
Q_OBJECT
public:
	impBeep(int item_id, bool enabled, QString icon_on, QString icon_off, QString text);

public slots:
	void toggleBeep();

private:
	int item_id;
};


class bannContrast : public bannOnDx
{
Q_OBJECT
public:
	bannContrast(int item_id, int val, QString icon);

private slots:
	void done();

private:
	int item_id;
};


class bannVersion : public bannOnDx
{
Q_OBJECT
public:
	bannVersion(QWidget *parent, QString icon, Version *ver);
private slots:
	void showVers();
private:
	Version *v;
};


// Logic to enable/disable password on freeze and change password.
class PasswordChanger : public QObject
{
Q_OBJECT
public:
	PasswordChanger(int item_id, QString pwd, bool check_active);

public slots:
	/*
	 * User tries to activate password checking on freeze.
	 *
	 * Check if the user still remembers the password. Shows Keypad and compare input to current password before
	 * actually activating password on freeze.
	 */
	void requestPasswdOn();

	/*!
		\brief Ask for the current password, then the new one (with confirm).
	 */
	void changePassword();

signals:
	/*!
		\brief Emitted when password check (de)activation or password change completes.
	 */
	void finished();

	/*!
		\brief Emitted when password check is activated/deactivated.
	 */
	void passwordActive(bool active);

protected:
	virtual void showEvent(QShowEvent *event);

private slots:
	// called on tasti::accept() (user has confirmed password entry)
	void checkPasswd();
	//
	void resetState();

	/**
	 * Stops the error beep made when the password insertion is wrong
	 */
	void restoreBeepState();

private:
	/**
	 * Enable/disable password checking on freeze
	 */
	void toggleActivation();

	/*
	 * Save password to conf.xml
	 */
	void savePassword(const QString &passwd);

	enum PasswdStatus
	{
	    PASSWD_NOT_SET,                     // TODO: maybe not needed?
	    ASK_PASSWORD,                       // check if user remembers password then activate password on freeze
	    CHECK_OLD_PASSWORD,                 // check if user remembers old password
	    INSERT_NEW_PASSWORD,                // let the user insert the new password (not showing text)
	    VERIFY_PASSWORD,                    // insert again the new password, to avoid wrong touches
	};

	void setStatus(PasswdStatus status);

	bool active;
	PasswdStatus status;
	QString password, new_password;
	Keypad *tasti;
	bool sb;
	int item_id;
};


class BannPassword : public Bann2StateButtons
{
Q_OBJECT
public:
	BannPassword(QString icon_on, QString icon_off, QString icon_label, QString descr,
			 int item_id, QString pwd, int attiva);

private:
	PasswordChanger changer;
};


// Set ringtone for a specific function (ringtone 1-4 from external units, alarms etc.).
class BannRingtone : public Bann2CentralButtons
{
Q_OBJECT
public:
	BannRingtone(const QString &descr, int id, Ringtones::Type type, int ring);

private slots:
	void plusClicked();
	void minusClicked();
	void playRingtone();
	void startPlayRingtone();
	void ringtoneFinished();

private:
	int current_ring, item_id;
	Ringtones::Type ring_type;
	bool is_playing;

};


// Set the time between screensaver frames.
// For example, it can be the timeout between images in the slideshow.
class ScreensaverTiming : public Bann2Buttons
{
Q_OBJECT
public:
	ScreensaverTiming(const QString &descr, int init_timing, int _delta = 1000, int min_timing = 7000, int max_timing = 30000);

	/*
	 * Get the current value, in msec.
	 */
	int getTiming() const;

private slots:
	void increase();
	void decrease();

private:
	void updateText();
	int timing, delta, min, max;
};

#endif
