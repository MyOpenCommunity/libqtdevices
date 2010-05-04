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

class BtButton;
class QLCDNumber;
class QLabel;
class RadioSourceDevice;


class RadioInfo : public QWidget
{
Q_OBJECT
public:
	RadioInfo();

	void setFrequency(int frequency);
	void setChannel(int memory_channel);
	void setRadioName(const QString &rds);

protected:
	void paintEvent(QPaintEvent *);

private:
	QLabel *radio_name, *frequency, *channel;
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

private:
	QWidget *createContent();

	// if set, use a fixed 50Hz step for the frequency up/down, do automatic tuning otherwise
	bool manual;

	QTimer memory_timer;
	int memory_number;
	BtButton *minus_button, *plus_button, *auto_button, *manual_button;
	QButtonGroup button_group;
	QString manual_off, manual_on, auto_off, auto_on;
	RadioInfo *radio_info;
	RadioSourceDevice *dev;

private slots:
	void valueReceived(const DeviceValues &values_list);

	void frequencyUp();
	void frequencyDown();
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
