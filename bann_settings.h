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
#include "bann2_buttons.h" // bann2But
#include "ringtonesmanager.h" // RingtoneType

#include <QWidget>

class Version;
class AlarmClock;
class Keypad;
class Calibrate;
class Contrast;
class contdiff;


/*!
  \class bannAlarmClock
  \brief This class is made to make alarm clock settings.

  \date lug 2005
*/
class bannAlarmClock : public bann2But
{
Q_OBJECT
public:
	bannAlarmClock(int item_id, int hour, int minute, QString icon_on,
		QString icon_off, QString icon_label, int enabled, int freq, int tipo);
	/*!
	\brief changes the abilitation af the alarm set
	*/
	void setAbil(bool);
	/*!
	\brief forces a eeprom read to initialyze alarm set settings
	*/
	void inizializza(bool forza = false);

	virtual void setSerNum(int num);

public slots:
/*!
\brief changes the alarm set abilitation
*/
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
	/*!
	\brief changes the abilitation af the alarm set
	*/
	void setAbil(bool);
	/*!
	\brief forces a eeprom read to initialyze alarm set settings
	*/
	void inizializza(bool forza = false);

	virtual void setSerNum(int num);

public slots:
/*!
\brief changes the alarm set abilitation
*/
	void toggleAbil();
	void setButtonIcon();

private:
	AlarmClock *alarm_clock;

private slots:
	void handleClose();
};


/*!
  \class calibration
  \brief Calibrate the device

  \date lug 2005
*/
// TODO: rimuovere questa classe, e usare direttamente bannOnDx! E' necessario intervenire su Calibrate
// per modificare la gestione del grabMouse e del backup della vecchia calibrazione.
class calibration : public bannOnDx
{
Q_OBJECT
public:
	calibration(QWidget *parent, QString icon);

private slots:
	void doCalib();
	void fineCalib();
private:
	Calibrate* calib;
signals:
	void startCalib();
	void endCalib();
};


/*!
  \class setDataOra
  \brief Beep (dis)abilitation

  \date lug 2005
*/
class impBeep : public bannOnSx
{
Q_OBJECT
public:
	impBeep(int item_id, bool enabled, QString icon_on, QString icon_off);

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


/*!
  \class impPassword
  \brief Manages the password.

  It's possible (dis)abilitate the password and to change the actual password.
  \date lug 2005
*/
class impPassword : public Bann2Buttons
{
Q_OBJECT
public:
	impPassword(QString icon_on, QString icon_off, QString icon_label, QString descr,
		    int item_id, QString pwd, int attiva);

public slots:
/*!
  \brief  Changes the activation state
*/
	void toggleActivation();

/*!
  \brief  Stops the error beep made when the password insertion is wrong
*/
	void restoreBeepState();

protected:
	virtual void showEvent(QShowEvent *event);

private slots:
	void checkPasswd();

private:
	enum PasswdStatus
	{
	    PASSWD_SET,
	    PASSWD_NOT_SET,
	};
	bool active;
	PasswdStatus status;
	QString password;
	Keypad *tasti;
	bool sb;
	int item_id;

signals:
/*!
  \brief  Emitted when the password is (dis)abilitated so BtMain knows if has to ask password or not
*/
	void activatePaswd(bool);
};


/**
 * Banner to set ringtone for a specific function (ringtone 1-4 from external units, alarms etc.).
 */
class BannRingtone : public Bann2CentralButtons
{
Q_OBJECT
public:
	BannRingtone(const QString &descr, RingtoneType type);

private:
	int current_ring;

private slots:
	void plusClicked();
	void minusClicked();
};

#endif
