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

#include "hardware_functions.h" // AMPLI_NUM
#include "bannerpage.h"
#include "page.h"
#include "navigation_bar.h" // AbstractNavigationBar
#include "bann2_buttons.h" // Bann2StateButtons

#include <QTime>

class StateButton;
class BtTimeEdit;

class QWidget;
class QLabel;
class QTimer;
class AmplifierDevice;
class AlarmClockTime;
class AlarmClockDays;
class AlarmClockTimeDays;
class AlarmClockSoundDiff;
class AlarmSoundDiffDevice;
class SingleChoiceContent;


// The implementation af the alarm set.
class AlarmClock : public Page
{
Q_OBJECT
public:

	// The alarm set type
	enum Type
	{
		BUZZER = 0, // The buzzer sounds and backlight flashes
		SOUND_DIFF = 1  // The sound diffusion system is used
	};

	AlarmClock(int item_id, Type type, int days, int hour, int minute);

	// Reads from the eeprom the alarm set state.
	void inizializza();

	// Sets the number of the actual instance of this class among all the alarm set present in the project.
	void setSerNum(int);

	// Sets the alarm set (dis)active.
	void setActive(bool);

	/*
	 * Returns the active state of the alarm clock. When alarm clock type is ONCE, this method
	 * returns false as soon as the alarm clock fires.
	 * \return True if the alarm clock is set, false otherwise.
	 */
	bool isActive();

public slots:
	// Show the sound diffusion page.
	void showSoundDiffPage();

	void valueReceived(const DeviceValues &values_list);

signals:
	void alarmClockFired();

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev);

private slots:
	void saveVolumes();
	void resetVolumes();

	// Draws the first page for alarm set setting and initializes some connections.
	virtual void showPage();

	// Executed when the alarm set sequency is closed to save the data and adjust sound diffusion page if necessary.
	void saveAndActivate();

	// Executed every minute when alarm set is active to detect if it's time to make the alarm ser start.
	void checkAlarm();

	// Executed every three seconds to increase the sound volume during sound diffusion alarm set starting up.
	void sounddiffusionAlarm();

	// Executed every 100 ms to manage the buzzer alarm set.
	void buzzerAlarm();

	// Executed every 5 s to manage the wav alarm set.
	void wavAlarm();

	// Stops the alarm set.
	void stopAlarm();

	// called if the alarm times out
	void alarmTimeout();

private:
	int id, item_id;
	uchar conta2min;
	uchar source, station;
	bool update_eeprom;
	int serial_number;
	bool buzzer_enabled;
	unsigned int buzzer_counter;

	// The variables which represent an alarm.
	QList<bool> alarm_days;
	Type alarm_type;
	QTime alarm_time;

	int alarm_volumes[AMPLI_NUM];
	bool active;
	QTimer *check_alarm_timer, *timer_increase_volume;
#ifdef LAYOUT_TS_3_5
	AlarmClockTime *alarm_time_page;
	AlarmClockDays *alarm_days_page;
#else
	AlarmClockTimeDays *alarm_time_page;
	AlarmClockTimeDays *alarm_days_page;
#endif
	AlarmClockSoundDiff *alarm_sound_diff;
	AlarmSoundDiffDevice *dev;
	AmplifierDevice *general;

	void ringAlarm();
};


// The page used to set the alarm time for ts3
class AlarmClockTime : public Page
{
Q_OBJECT
public:
	AlarmClockTime(QTime time);

	QTime getAlarmTime() const;
	void setActive(bool active) {}

	void saveAlarmTime();
	void resetAlarmTime();

signals:
	void okClicked();

private:
	BtTimeEdit *edit;
	QTime alarm_time;
};


// A banner that represents a day
class BannAlarmDay : public Bann2StateButtons
{
Q_OBJECT
public:
	BannAlarmDay(QString img_off, QString img_on, QString descr);

	void setStatus(bool st);
	bool getStatus() const;

private slots:
	void toggleStatus();
};


// The page used to set the alarm days for ts3
class AlarmClockDays : public BannerPage
{
Q_OBJECT
public:
	AlarmClockDays(AlarmClock::Type type, QList<bool> days);

	QList<bool> getAlarmDays() const;

	void saveAlarmDays();
	void resetAlarmDays();

signals:
	void okClicked();

private:
	QList<bool> alarm_days;
};


// The page for the sound diffusion settings for the alarm clock.
class AlarmClockSoundDiff : public Page
{
Q_OBJECT
public:
	virtual void showPage();

signals:
	void saveVolumes();
};


// The page of the alarm clock for ts10, which contains both time and days settings.
class AlarmClockTimeDays : public Page
{
Q_OBJECT
public:
	AlarmClockTimeDays(QTime time, AlarmClock::Type type, QList<bool> days);

	QTime getAlarmTime() const;
	QList<bool> getAlarmDays() const;

	void setActive(bool active);

	void saveAlarmTime();
	void resetAlarmTime();
	void saveAlarmDays();
	void resetAlarmDays();

signals:
	void okClicked();
	void showSoundDiffusion();

private:
	BtTimeEdit *edit;
	QString alarm_icon;
	QLabel *alarm_label;
	StateButton *buttons[7];
	QTime alarm_time;
	QList<bool> alarm_days;
};


#endif // ALARMCLOCK_H
