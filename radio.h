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


#ifndef RADIO_H
#define RADIO_H

#include "page.h"

#include <QTimer>
#include <QLabel>
#include <QList>


class BtButton;
class StateButton;
class QLCDNumber;
class RadioSourceDevice;
class QStackedLayout;


/*!
	\ingroup SoundDiffusion
	\brief Label that shows info about the Radio (name, frequency and channel number).
 */
class RadioInfo : public QLabel
{
Q_OBJECT
public:
	RadioInfo( QString area, RadioSourceDevice *dev);
	void isShown(bool sh);
	void setArea(const QString &area);
#ifdef LAYOUT_TS_10
	void setBackgroundImage(const QString &background_image);
#else
	void showCycleButton(bool show);
#endif

#ifdef LAYOUT_TS_3_5
signals:
	void nextStation();
#endif

protected:
	virtual QSize sizeHint() const;

private slots:
	void valueReceived(const DeviceValues &values_list);
	void screensaverStarted();
	void screensaverStopped();

private:
	QString area;
	QLabel *radio_name, *channel;

#ifdef LAYOUT_TS_3_5
	QLCDNumber *frequency;
	BtButton *cycle;
#else
	QLabel *frequency;
#endif
	RadioSourceDevice *dev;
	bool shown;
	bool screensaver_running;

	void setFrequency(int frequency);
	void setChannel(int memory_channel);
	void setRadioName(const QString &rds);
};


/*!
	\ingroup SoundDiffusion
	\brief Details page for RDS radio, linked from RadioSource.
 */
class  RadioPage : public Page
{
Q_OBJECT
public:
	RadioPage(RadioSourceDevice *dev, const QString &area_descr, const QString &title = tr("RDS Radio"));

	// The page is shared between different areas, so we need to set the area
	// every time that it is shown.
	void setArea(const QString &area);

protected:
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);

private:
#ifdef LAYOUT_TS_10
	QWidget *createContent();
#endif

	// if set, use a fixed 50Hz step for the frequency up/down, do automatic tuning otherwise
	bool manual;

	QTimer memory_timer, request_frequency;
	int memory_number;
	BtButton *minus_button, *plus_button;
	StateButton *auto_button, *manual_button;
	RadioInfo *radio_info;
	RadioSourceDevice *dev;
	static int save_sound_delay;

	// Helper functions to share code between ts3 and ts10.
	void createFrequencyButtons();
	QList<BtButton*> createMemoryButtons();

#ifdef LAYOUT_TS_3_5
	QStackedLayout *buttons_stack;
#endif

private slots:
	void frequencyUp();
	void frequencyDown();
	void requestFrequency();
	void changeStation(int station_num);
	void memoryButtonPressed(int but_num);
	void memoryButtonReleased(int but_num);
	void storeMemoryStation();

	void previousStation();
	void nextStation();

	void enterBeepState(int new_state);
	void exitBeepState();

	// Play a "beep" to notifies that the radio is saved.
	void playSaveSound();

	/*!
	 * \brief Changes the state to automatic search
	 */
	void setAuto();

	/*!
	 * \brief Changes the state to manual search
	 */
	void setManual();

#ifdef LAYOUT_TS_3_5
	void showMemoryButtons();
	void backClick();
#endif
};

#endif // RADIO_H
