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


#ifndef BANN_THERMAL_REGULATION_H
#define BANN_THERMAL_REGULATION_H

#include "bann1_button.h" // BannSinglePuls
#include "bann2_buttons.h" // Bann2CentralButtons
#include "main.h" // TemperatureScale

class ThermalDevice;
class NonControlledProbeDevice;

class QLabel;


/**
 * This banner shuts the thermal regulator off when clicked. Before using it, be sure to
 * set the address of the thermal regulator.
 * It displays a button in the center with icon "OFF".
 */
class BannOff : public BannCenteredButton
{
Q_OBJECT
public:
	BannOff(QWidget *parent, ThermalDevice *_dev);
private slots:
	/**
	 * Shut down the thermal regulator
	 */
	void performAction();
private:
	/// The device that this banner sends commands to
	ThermalDevice *dev;
signals:
	void clicked();
};


/**
 * This banner sets the thermal regulator in antifreeze protection. Be sure to set the
 * address of the device.
 * It displays one button at the center with icon antifreeze on it.
 */
class BannAntifreeze : public BannCenteredButton
{
Q_OBJECT
public:
	BannAntifreeze(QWidget *parent, ThermalDevice *_dev);
private slots:
	/**
	 * Set thermal regulator in antifreeze protection
	 */
	void performAction();
private:
	/// The device that this banner sends commands to
	ThermalDevice *dev;
signals:
	void clicked();
};


/**
 * Banner with two buttons, one to set the thermal regulator to "off" and one
 * to set antifreeze mode.
 */
class BannOffAntifreeze : public Bann2CentralButtons
{
Q_OBJECT
public:
	BannOffAntifreeze(QWidget *parent, ThermalDevice *_dev);

signals:
	void clicked();

private slots:
	void setOff();
	void setAntifreeze();

private:
	/// The device that this banner sends commands to
	ThermalDevice *dev;
};


/**
 * This banner sets the thermal regulator in summer or winter status, depending on the
 * button pressed.
 * It displays two buttons at the center, one with the summer icon and one with the winter icon.
 */
class BannSummerWinter : public Bann2CentralButtons
{
Q_OBJECT
public:
	BannSummerWinter(QWidget *parent, ThermalDevice *_dev);
	enum seasons {WINTER, SUMMER};
public slots:
	void setSummer();
	void setWinter();
private:
	seasons status;
	/// The device that this banner sends commands to
	ThermalDevice *dev;
signals:
	void clicked();
};


/**
 * This banner emits a signal with an int, corresponding to the program set with setProgram(). Default
 * program is 1, so be sure to set the program you want to be set before using it.
 * It displays a not clickable image on the center, an ok button on the right and program description
 * below.
 */
class BannWeekly : public BannSinglePuls
{
Q_OBJECT
public:
	BannWeekly(QWidget *parent, int index);

private slots:
	void performAction();

signals:
	void programNumber(int);

private:
	int index;
};


/**
 * A banner to display the temperature for not controlled and external probes.
 */
class BannTemperature : public BannerNew
{
Q_OBJECT
public:
	BannTemperature(QString descr, NonControlledProbeDevice *dev);

public slots:
	void valueReceived(const DeviceValues &values_list);

private:
	void updateTemperature(int temperature);

private:
	QLabel *temperature_label;

	TemperatureScale temperature_scale;
};

#endif // BANN_THERMAL_REGULATION_H
