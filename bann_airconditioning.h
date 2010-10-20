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


#ifndef BANN_AIRCONDITIONING_H
#define BANN_AIRCONDITIONING_H

#include "bann2_buttons.h" // Bann2Buttons
#include "bann1_button.h" // BannStates
#include "airconditioning_device.h"
#include "main.h" // bt_global::config, TemperatureScale


#include <QList>
#include <QPair>
#include <QHash>
#include <QButtonGroup>

typedef AdvancedAirConditioningDevice::Status AirConditionerStatus;
class NonControlledProbeDevice;
class AdvancedSplitPage;


/*!
	\ingroup AirConditioning
	\brief Controls a basic split.
*/
class SingleSplit : public Bann2Buttons
{
Q_OBJECT
public:
	SingleSplit(QString descr, bool show_right_button, AirConditioningInterface *d, NonControlledProbeDevice *d_probe=0);

protected slots:
	virtual void valueReceived(const DeviceValues &values_list);

private:
	AirConditioningInterface *dev;
	NonControlledProbeDevice *dev_probe;

private slots:
	void setDeviceOff();
};


/*!
	\ingroup AirConditioning
	\brief Controls an advanced split.
*/
class AdvancedSingleSplit : public SingleSplit
{
Q_OBJECT
public:
	AdvancedSingleSplit(QString descr, AdvancedSplitPage *p, AirConditioningInterface *d, NonControlledProbeDevice *probe = 0);
	virtual void setSerNum(int ser);

private:
	AdvancedSplitPage *page;
};


/*!
	\ingroup AirConditioning
	\brief Manages the whole list of splits.
*/
class GeneralSplit : public Bann2Buttons
{
Q_OBJECT
public:
	GeneralSplit(QString descr, bool show_right_button);

signals:
	/*!
		\brief Request to switch off the entire splits list.
	*/
	void sendGeneralOff();
};


/*!
	\ingroup AirConditioning
	\brief Represents a preconfigured scenario for an advanced split.

	In this case, the scenario is a new AdvancedAirConditioningDevice::Status.
*/
class AdvancedSplitScenario : public Bann2Buttons
{
Q_OBJECT
public:
	AdvancedSplitScenario(const AirConditionerStatus &st, const QString &descr, AdvancedAirConditioningDevice *d, QWidget *parent = 0);

private:
	AirConditionerStatus status;
	AdvancedAirConditioningDevice *dev;
	QString conf_name; // CONFIG_TS_3_5 remove

private slots:
	void onButtonClicked();
};

/*!
	\ingroup AirConditioning
	\brief Allows the user to view and modify the AdvancedAirConditioningDevice::Status
	of the split through the SplitSettings page.
*/
class CustomScenario : public Bann2Buttons
{
Q_OBJECT
public:
	CustomScenario(AdvancedAirConditioningDevice *d);

public slots:
	/*!
		Forward the changes to the related device.
	*/
	void splitValuesChanged(const AirConditionerStatus &st);

private:
	AdvancedAirConditioningDevice *dev;
};


/*!
	\ingroup AirConditioning
	\brief Represents the temperature of the split.
*/
class SplitTemperature : public Bann2Buttons
{
Q_OBJECT
public:
	/*!
		\brief Constructor

		All values are assumed to be in the temperature scale set in the configuration
		file.
	*/
	SplitTemperature(int init_temp, int level_max, int level_min, int step, int initial_mode);

	/*!
		\brief Set the current temperature. \a new_temp_celsius must be in celsius degrees.
	*/
	void setTemperature(int new_temp_celsius);

	/*!
		\brief Return the current temperature in celsius degrees.
	*/
	int temperature();

public slots:
	void currentModeChanged(int new_mode);

private slots:
	void increaseTemp();
	void decreaseTemp();

private:
	// round a temperature to the nearest multiple of 5
	int roundTo5(int temp);
	void setBannerEnabled(bool enable);
	void updateText();
	int current_temp, max_temp, min_temp, temp_step;
	TemperatureScale scale;
	bool is_enabled;
	QString icon_plus, icon_plus_disabled, icon_minus, icon_minus_disabled;
};


/*!
	\ingroup AirConditioning
	\brief Represents the mode of the split.
*/
class SplitMode : public BannStates
{
Q_OBJECT
public:
	SplitMode(QList<int> modes, int current_mode);

private:
	QHash<int, QString> modes_descr;

signals:
	void modeChanged(int);
};


/*!
	\ingroup AirConditioning
	\brief Represents the speed of the split.
*/
class SplitSpeed : public BannStates
{
Q_OBJECT
public:
	SplitSpeed(QList<int> speeds, int current_speed);
private:
	QHash<int, QString> speeds_descr;
};


/*!
	\ingroup AirConditioning
	\brief Represents the swing of the split.
*/
class SplitSwing : public Bann2StateButtons
{
Q_OBJECT
public:
	SplitSwing(QString descr, bool init_swing);
	void setSwingOn(bool swing_on);
	bool swing();

private:
	QButtonGroup buttons;
};


/*!
	\ingroup AirConditioning
	\brief Represents a preconfigured scenario for a basic split.

	In this simple case, a scenario is just a 'what' to send to the connected device.
*/
class SplitScenario : public Bann2Buttons
{
Q_OBJECT
public:
	SplitScenario(QString descr, QString cmd, AirConditioningDevice *d);

private:
	AirConditioningDevice *dev;
	QString command;

private slots:
	void sendScenarioCommand();
};


/*!
	\ingroup AirConditioning
	\brief Represents a preconfigured scenario for a group of splits.

	In this case, a scenario is composed by a list of <'what', device>.
	When the button to trigger the scenario is pressed, for each device will be
	send the related command. In this way the scenario can be used to control
	a group of splits, even if each split requires a different command to set.
*/
class GeneralSplitScenario : public Bann2Buttons
{
Q_OBJECT
public:
	GeneralSplitScenario(QString descr);

	void appendDevice(QString cmd, AirConditioningInterface *d);

private:
	QList<QPair<QString, AirConditioningInterface*> > devices_list;

private slots:
	void sendScenarioCommand();
};

#endif // BANN_AIRCONDITIONING_H
