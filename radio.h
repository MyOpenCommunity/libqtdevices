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

#include <QButtonGroup>
#include <QTimer>
#include <QLabel>

class BtButton;
class QLCDNumber;
class QLabel;
class RadioSourceDevice;


/**
 * The label that shows info about the Radio (the name, taken from the RDS system,
 * the frequency and the number of the channel).
 */
class RadioInfo : public QLabel
{
Q_OBJECT
public:
	RadioInfo(const QString &background_image, QString area, RadioSourceDevice *dev);

	void setFrequency(int frequency);
	void setChannel(int memory_channel);
	void setRadioName(const QString &rds);

	void isShown(bool sh);
	void setArea(const QString &area);

private slots:
	void valueReceived(const DeviceValues &values_list);
	void screensaverStarted();
	void screensaverStopped();

private:
	QString area;
	QLabel *radio_name, *frequency, *channel;
	RadioSourceDevice *dev;
	bool shown;
	bool screensaver_running;
};


/*!
 * \class RadioPage
 * \brief This class implements the management of the FM tuner specific page.
 */
class  RadioPage : public Page
{
Q_OBJECT
public:
	RadioPage(RadioSourceDevice *dev, const QString &amb = tr("RDS Radio"));

	// The page is shared between different areas, so we need to set the area
	// every time that it is shown.
	void setArea(const QString &area);

protected:
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);

private:
	QWidget *createContent();

	// if set, use a fixed 50Hz step for the frequency up/down, do automatic tuning otherwise
	bool manual;

	QTimer memory_timer, request_frequency;
	int memory_number;
	BtButton *minus_button, *plus_button, *auto_button, *manual_button;
	QButtonGroup button_group;
	QString manual_off, manual_on, auto_off, auto_on;
	RadioInfo *radio_info;
	RadioSourceDevice *dev;

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

	/*!
	 * \brief Changes the state to automatic search
	 */
	void setAuto();

	/*!
	 * \brief Changes the state to manual search
	 */
	void setManual();
};

#endif // RADIO_H
