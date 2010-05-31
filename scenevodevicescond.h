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


#ifndef SCENEVODEVICESCOND_H
#define SCENEVODEVICESCOND_H

#include <QWidget>
#include <QList>

#include "main.h" // TemperatureScale
#include "pulldevice.h" // DeviceValues, PullMode


class QString;
class QLabel;
class BtButton;

/**
 * This abstract class is an object that can be used to represent a device
 * condition. Inherit and reimplement the updateText method to format properly
 * the condition when its value changes.
 */
class DeviceConditionDisplayInterface
{
public:
	virtual void updateText(int min_condition_value, int max_condition_value) = 0;
};



/**
 * This widget is an abstract class that can be used to display a device condition
 * in a standard way (different depending on the underlying hardware). Reimplement
 * the updateText method to format properly the condition when its value changes.
 */
class DeviceConditionDisplay : public QWidget, public DeviceConditionDisplayInterface
{
Q_OBJECT
public:
	DeviceConditionDisplay(QWidget *parent, QString descr, QString top_icon);

signals:
	void upClicked();
	void downClicked();

protected:
	QLabel *condition;
	BtButton *up_button, *down_button;
};


/**
 * This class can be used to represent a device condition which can have only
 * the ON or the OFF status.
 */
class DeviceConditionDisplayOnOff : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayOnOff(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


/**
 * This class can be used for represent a dimming device condition.
 */
class DeviceConditionDisplayDimming : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayDimming(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


/**
 * This class can be used for represent a volume device condition.
 */
class DeviceConditionDisplayVolume : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayVolume(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};


/**
 * This class can be used for represent a temperature device condition
 * (in celsius or Fahrenheit, depending from the configuration).
 */
class DeviceConditionDisplayTemperature : public DeviceConditionDisplay
{
Q_OBJECT
public:
	DeviceConditionDisplayTemperature(QWidget *parent, QString descr, QString top_icon) :
		DeviceConditionDisplay(parent, descr, top_icon) {}

public slots:
	void updateText(int min_condition_value, int max_condition_value);
};



/**
 * This abstract class represents a device based condition, used in the evolved
 * scenarios. Every DeviceCondition has a condition that depends on the actual
 * device and can trigger the signal condSatisfied if the device status matches the
 * condition set.
 */
class DeviceCondition : public QObject
{
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:
	enum Type
	{
		LIGHT = 1,
		DIMMING = 2,
		EXTERNAL_PROBE = 7,
		PROBE = 3,
		TEMPERATURE = 8,
		AUX = 9,
		AMPLIFIER = 4,
		DIMMING100 = 6
	};

	//! Returns true when the condition is satisfied
	bool isTrue();

	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);

	// Saves the current value as the condition
	void save();
	// Resets the current value to the condition
	void reset();

public slots:
	//! Invoked when UP button is pressed
	virtual void Up();
	//! Invoked when DOWN button is pressed
	virtual void Down();

signals:
	//! Emitted when the condition on device is satisfied
	void condSatisfied();

protected:
	DeviceCondition(DeviceConditionDisplayInterface *cond_display);

	// The method to update the descriptive text of the condition using the
	// the DeviceConditionDisplay widget.
	void updateText(int min_condition_value, int max_condition_value);

	void setDevice(device *d);

	// Parse the values cames from the correspondent device and set the 'satisfied'
	// var properly.
	virtual bool parseValues(const DeviceValues &values_list) = 0;

	//! Returns min value
	virtual int get_min();
	//! Returns max value
	virtual int get_max();
	//! Returns step
	virtual int get_step();
	//! Sets condition value
	void set_condition_value(int);
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	//! Gets condition value
	virtual int get_condition_value();
	//! Draws frame
	virtual void Draw() = 0;

	//! Returns current value for condition
	int get_current_value();
	//! Sets current value for condition
	int set_current_value(int);

	//! True when condition is satisfied
	bool satisfied;

	// Invoked when called the public methods save/reset
	virtual void onConditionSaved();
	virtual void onConditionReset();

	 // true if the condition is changed (but not yet saved)
	virtual bool conditionChanged();

private slots:
	void valueReceived(const DeviceValues &values_list);

private:
	//! Condition value
	int cond_value;
	//! Current value (displayed, not confirmed)
	int current_value;

	DeviceConditionDisplayInterface *condition_display;

	device *dev;
	// Because we won't trigger during the device initialization, we use this
	// flag to recognize if trigger or not.
	bool initialized;
};


class DeviceConditionLight : public DeviceCondition
{
Q_OBJECT
public:
	DeviceConditionLight(DeviceConditionDisplayInterface* condition_display, QString trigger, QString where,
		int openserver_id = 0, PullMode pull_mode = PULL_UNKNOWN);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);

protected:
	virtual void Draw();
	virtual int get_max();
	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);

	virtual bool parseValues(const DeviceValues &values_list);
};


class DeviceConditionDimming : public DeviceCondition
{
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:
	DeviceConditionDimming(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where,
		int openserver_id = 0, PullMode pull_mode = PULL_UNKNOWN);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);

public slots:
	virtual void Up();
	virtual void Down();

protected:
	virtual void onConditionSaved();
	virtual void onConditionReset();
	virtual bool conditionChanged();

	virtual int get_min();
	virtual int get_max();
	virtual void Draw();

	void set_condition_value_min(int);
	int get_condition_value_min();
	void set_condition_value_max(int);
	int get_condition_value_max();
	int get_current_value_min();
	void set_current_value_min(int min);
	int get_current_value_max();
	void set_current_value_max(int max);
	QString get_current_value();

	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);

	virtual bool parseValues(const DeviceValues &values_list);

private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
};


class DeviceConditionDimming100 : public DeviceCondition
{
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:
	DeviceConditionDimming100(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where,
		int openserver_id = 0, PullMode pull_mode = PULL_UNKNOWN);
	//! Translates current trigger condition to open
	virtual void get_condition_value(QString&);

public slots:
	virtual void Up();
	virtual void Down();

protected:
	//! Returns min value
	int get_min();
	//! Returns max value
	virtual int get_max();
	void set_condition_value_min(int);
	int get_condition_value_min();
	void set_condition_value_max(int);
	int get_condition_value_max();
	int get_current_value_min();
	void set_current_value_min(int min);
	int get_current_value_max();
	void set_current_value_max(int max);
	QString get_current_value();

	//! Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	virtual void Draw();


	virtual bool parseValues(const DeviceValues &values_list);
	virtual void onConditionSaved();
	virtual void onConditionReset();
	virtual bool conditionChanged();

private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
};


class DeviceConditionVolume : public DeviceCondition
{
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:
	DeviceConditionVolume(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where);
	void get_condition_value(QString& out);

public slots:
	virtual void Up();
	virtual void Down();

protected:
	int get_min();
	virtual int get_max();
	void set_condition_value_min(int);
	int get_condition_value_min();
	void set_condition_value_max(int);
	int get_condition_value_max();
	int get_current_value_min();
	void set_current_value_min(int min);
	int get_current_value_max();
	void set_current_value_max(int max);

	virtual void Draw();

	virtual bool parseValues(const DeviceValues &values_list);
	virtual void onConditionSaved();
	virtual void onConditionReset();
	virtual bool conditionChanged();

private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
};


class DeviceConditionTemperature : public DeviceCondition
{
Q_OBJECT
public:
	DeviceConditionTemperature(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where, bool external = false, int openserver_id = 0);
	virtual void get_condition_value(QString&);

protected:
	int get_min();
	virtual int get_max();
	int get_step();
	int intValue();
	virtual void Draw();

	virtual bool parseValues(const DeviceValues &values_list);

private:
	/// Maximum and minimum values for temperature conditions
	int max_temp, min_temp;
	/// Step value for temperature conditions
	int step;
	TemperatureScale temp_scale;
};


class DeviceConditionAux : public DeviceCondition
{
Q_OBJECT
public:
	DeviceConditionAux(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where);

protected:
	virtual void Draw();
	virtual int get_max();
	virtual void set_condition_value(QString);

	virtual bool parseValues(const DeviceValues &values_list);
};


#endif // SCENEVODEVICESCOND_H
