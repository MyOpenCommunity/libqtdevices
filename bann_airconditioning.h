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

typedef AdvancedAirConditioningDevice::AirConditionerStatus AirConditionerStatus;
class NonControlledProbeDevice;
class AdvancedSplitPage;


/**
 * The banner for a split
 */
class SingleSplit : public Bann2Buttons
{
Q_OBJECT
public:
	SingleSplit(QString descr, bool show_right_button, AirConditioningInterface *d, NonControlledProbeDevice *d_probe=0);

protected slots:
	virtual void status_changed(const StatusList &status_list);

private:
	AirConditioningInterface *dev;
	NonControlledProbeDevice *dev_probe;

private slots:
	void setDeviceOff();
};

/**
 * Banner for an advanced split.
 *
 * This is mostly the same as SingleSplit, except for setSerNum() call (and a member and a ctor parameter,
 * but they're needed for setSerNum implementation).
 * Its sole purpose is support for saving the configuration for an advanced split in conf file.
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


/**
 * The banner for managing the whole list of splits.
 */
class GeneralSplit : public Bann2Buttons
{
Q_OBJECT
public:
	GeneralSplit(QString descr, bool show_right_button);

signals:
	void sendGeneralOff();
};


/**
 * The banner for a scenario of a single split (configured as advanced)
 */
class AdvancedSplitScenario : public Bann2Buttons
{
Q_OBJECT
public:
	AdvancedSplitScenario(QString descr, int item_id, const QString &conf_node, AdvancedAirConditioningDevice *d, QWidget *parent = 0);
	/*
	 * Set initial values when 'ON' is pressed.
	 * Split settings parameters are parsed only in SplitSettings, at startup the 'ON' button sends
	 * the default command (ie. 'OFF'). This method is called to set the correct command.
	 */
	void setCurrentValues(const AirConditionerStatus &st);

public slots:
	void splitValuesChanged(const AirConditionerStatus &st);

private:
	AirConditionerStatus status;
	AdvancedAirConditioningDevice *dev;
	QString conf_name; // CONFIG_BTOUCH remove
	int item_id;

private slots:
	void onButtonClicked();
};

/**
 * Banner for the single button that accesses the custom page (no scenario).
 * This banner doesn't save on conf file.
 */
class CustomScenario : public Bann2Buttons
{
Q_OBJECT
public:
	CustomScenario(AdvancedAirConditioningDevice *d);

public slots:
	void splitValuesChanged(const AirConditionerStatus &st);

private:
	AdvancedAirConditioningDevice *dev;
};


/**
 * The banner that represent the temperature of the split
 */
class SplitTemperature : public Bann2Buttons
{
Q_OBJECT
public:
	/**
	 * All values are assumed to be in the temperature scale set in conf.xml
	 */
	SplitTemperature(int init_temp, int level_max, int level_min, int step, int initial_mode);

	/**
	 * Set the current temperature. new_temp must be in celsius degrees.
	 */
	void setTemperature(int new_temp_celsius);

	/**
	 * Return the current temperature in celsius degrees, so that external users don't need check
	 * the temperature scale.
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


/**
 * The banner that represent the mode of the split
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


/**
 * The banner that represent the speed of the split
 */
class SplitSpeed : public BannStates
{
Q_OBJECT
public:
	SplitSpeed(QList<int> speeds, int current_speed);
private:
	QHash<int, QString> speeds_descr;
};


/**
 * The banner that represent the swing of the split
 */
class SplitSwing : public Bann2Buttons
{
Q_OBJECT
public:
	SplitSwing(QString descr, bool init_swing);
	void setSwingOn(bool swing_on);
	bool swing();

private:
	QButtonGroup buttons;
};


/**
 * The banner for a scenario of a single split (configured as basic)
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


/**
 * The banner for a scenario of a general split
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
