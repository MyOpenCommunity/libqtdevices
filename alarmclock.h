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


#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

#include "frame_receiver.h"
#include "hardware_functions.h"
#include "page.h"
#include "navigation_bar.h" // AbstractNavigationBar

#include <QTime>

class BtButton;
class BtTimeEdit;

class QWidget;
class QLabel;
class QTimer;
class AmplifierDevice;
class AlarmClockTime;
class AlarmClockFreq;
class AlarmClockTimeFreq;
class AlarmClockSoundDiff;
class AlarmSoundDiffDevice;
class SingleChoiceContent;


/*!
  \class AlarmClock
  \brief This class is the implementation af the alarm set.

*/
class AlarmClock : public Page
{
	friend class AlarmClockTime;
	friend class AlarmClockFreq;
	friend class AlarmClockTimeFreq;
	friend class AlarmClockSoundDiff;

Q_OBJECT
public:
/*! \enum  Freq
  Differentiate the alarm set frequencies of operation
*/
	enum Freq
	{
		// values used by BTouch
		SEMPRE = 1,  /*!< Always -> every day*/
		ONCE = 0,  /*!< Once -> only at the first occurrence of the time selected after the alarm set was setted*/
		FERIALI = 2,  /*!< Week days -> only from monday to friday*/
		FESTIVI = 3,  /*!< Festive days -> only on Sunday and Saturday*/
		NESSUNO = 50  /*!< Never*/
	};

/*! \enum Type
  Differentiate the alarm set type
*/
	enum Type
	{
		BUZZER = 0,  /*!< The buzzer sounds and backlight flashes*/
		DI_SON = 1  /*!< The sound diffusion system is used*/
	};

	AlarmClock(int id, int item_id, Type t, Freq f, QList<bool> days, int hour, int minute);

/*!
  \brief Reads from the eeprom the alarm set state.
*/
	void inizializza();

/*!
  \brief Sets the number of the actual instance of this class among all the alarm set present in the project.
*/
	void setSerNum(int);

/*!
  \brief Sets the alarm set (dis)active.
*/
	void setActive(bool);

	/**
	 * Returns the active state of the alarm clock. When alarm clock type is ONCE, this method
	 * returns false as soon as the alarm clock fires.
	 * \return True if the alarm clock is set, false otherwise.
	 */
	bool isActive();

public slots:
/*!
  \brief Show the frequency (once-always-mon/fri-sat-sun).
*/
	void showTypePage();

/*!
  \brief Show the sound diffusion page.
*/
	void showSoundDiffPage();

	void valueReceived(const DeviceValues &values_list);

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);

private slots:
	void saveVolumes();
	void resetVolumes();

/*!
  \brief Draws the first page for alarm set setting and initializes some connections.
*/
	virtual void showPage();

/*!
  \brief Executed when the alarm set sequency is closed to save the data and adjust sound diffusion page if necessary.
*/
	void saveAndActivate();

/*!
  \brief Executed every minute when alarm set is active to detect if it's time to make the alarm ser start.
*/
	void checkAlarm();

/*!
  \brief Executed every three seconds to increase the soud volume during \a sound \a diffusion \a alarm \a set starting up.
*/
	void sounddiffusionAlarm();

/*!
  \brief Executed every 100 ms to manage the \a buzzer  \a alarm \a set.
*/
	void buzzerAlarm();

/*!
  \brief Executed every 5 s to manage the \a wav  \a alarm \a set.
*/
	void wavAlarm();

/*!
  \brief Stops the alarm set.
*/
	void stopAlarm();

	// called if the alarm times out
	void alarmTimeout();

private:
	int id, item_id;
	Type type;
	Freq freq;
	QList<bool> days;
	uchar conta2min,sorgente,stazione, aggiornaDatiEEprom;
	int serNum;
	bool buzAbilOld;
	unsigned int contaBuzzer;
	QTime alarmTime;
	int volSveglia[AMPLI_NUM];
	bool active;
	QTimer *minuTimer,*aumVolTimer;
#ifdef LAYOUT_BTOUCH
	AlarmClockTime *alarm_time;
	AlarmClockFreq *alarm_type;
#else
	AlarmClockTimeFreq *alarm_time;
	AlarmClockTimeFreq *alarm_type;
#endif
	AlarmClockSoundDiff *alarm_sound_diff;
	AlarmSoundDiffDevice *dev;
	AmplifierDevice *general;

signals:
	void alarmClockFired();
};


/*!
  \class AlarmClockTime
  \brief Used to set the alarm time.

*/
class AlarmClockTime : public Page
{
Q_OBJECT
public:
	AlarmClockTime(AlarmClock *alarm_page);

	QTime getAlarmTime() const;
	void setActive(bool active) {}

private:
	BtTimeEdit *edit;
};


/*!
  \class AlarmClockFreq
  \brief Used to set the alarm frequency.

*/
class AlarmClockFreq : public Page
{
Q_OBJECT
public:
	AlarmClockFreq(AlarmClock *alarm_page);

	AlarmClock::Freq getAlarmFreq() const;
	QList<bool> getAlarmDays() const;

private slots:
	void setSelection(int freq);

private:
	SingleChoiceContent *content;
	AlarmClock::Freq frequency;
};


class AlarmClockSoundDiff : public Page
{
Q_OBJECT
public:
	AlarmClockSoundDiff(AlarmClock *alarm_page);

	virtual void showPage();

signals:
	void saveVolumes();
};


class AlarmClockTimeFreq : public Page
{
public:
	AlarmClockTimeFreq(AlarmClock *alarm_page);

	QTime getAlarmTime() const;
	AlarmClock::Freq getAlarmFreq() const;
	QList<bool> getAlarmDays() const;

	void setActive(bool active);

private:
	BtTimeEdit *edit;
	QString alarm_icon;
	QLabel *alarm_label;
	BtButton *buttons[7];
};


/*!
  \class AlarmNavigation
  \brief helper class for the bottom navigation bar.

*/
class AlarmNavigation : public AbstractNavigationBar
{
Q_OBJECT
public:
	AlarmNavigation(bool forwardButton, QWidget *parent = 0);

signals:
	void forwardClicked();
	void okClicked();
};


#endif // ALARMCLOCK_H
