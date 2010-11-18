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


/*!
	\ingroup ThermalRegulation
	\brief Display a single centered button to turn off the thermal regulator.
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


/*!
	\ingroup ThermalRegulation
	\brief Display a single centered button to put the thermal regulator into antifreeze mode.
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


/*!
	\ingroup ThermalRegulation
	\brief Display two centered buttons to turn off and put into antifreeze mode the thermal regulator.
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


/*!
	\ingroup ThermalRegulation
	\brief Display two centered buttons to change the summer/winter mode of the thermal regulator.
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


/*!
	\ingroup ThermalRegulation
	\brief Allow choosing a program scenario.

	This banner is used in WeeklyMenu and ScenarioMenu; when the right button is clicked
	it emits the programNumber() signal.

	Displays an image on the center, an ok button on the right and program description below.
 */
class BannWeekly : public BannSinglePuls
{
Q_OBJECT
public:
	BannWeekly(int index);

	int getProgramNumber() { return index; }

private slots:
	void performAction();

signals:
	void programNumber(int);

private:
	int index;
};


/*!
	\ingroup ThermalRegulation
	\brief Display description and temperature for non-controlled zones and external probes.
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
