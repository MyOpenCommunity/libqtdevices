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


/*!
	\ingroup Scenarios
	\brief An interface that can be used to represent graphically a device condition.

	Inherit and reimplement the updateText() method to format properly the condition
	when its value changes.

	\internal This interface is required in order to execute unit tests on the
	text drawed by the DeviceCondition.
*/
class DeviceConditionDisplayInterface
{
public:
	virtual void updateText(int min_condition_value, int max_condition_value) = 0;
};


/*!
	\ingroup Scenarios
	\brief Represents a device based condition, used in the improved scenarios.

	Every DeviceCondition has a condition which depends on the actual (physical)
	device and can triggers the signal condSatisfied() if the device status
	changes and matches the condition previously set.
*/
class DeviceCondition : public QObject
{
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:
	typedef QPair<int, int> ConditionState;

	/*!
		\brief The types of the device conditions.
	*/
	enum Type
	{
		LIGHT = 1,          /*!< A light based device condition. */
		DIMMING = 2,        /*!< A dimmer based device condition. */
		EXTERNAL_PROBE = 7, /*!< An external probe device condition. */
		PROBE = 3,          /*!< A probe device condition. */
		TEMPERATURE = 8,    /*!< A temperature device condition. */
		AUX = 9,            /*!< An aux device condition. */
		AMPLIFIER = 4,      /*!< An amplifier device condition. */
		DIMMING100 = 6      /*!< A dimmer 100 device condition. */
	};

	/*!
		\brief Returns true when the condition is satisfied
	*/
	bool isTrue();

	/*!
		\brief Returns a textual representation of the condition.
	*/
	virtual QString getConditionAsString();

	/*!
		\brief Saves the current value as the condition.
	*/
	void save();

	/*!
		\brief Resets the current value to the condition.
	*/
	void reset();

	/*!
		\brief Set condition value state

		Set the trigger value/range to the specified state; only set it to a
		value obtained through  \ref getState(), otherwise \ref Up() and
		\ref Down() might break.

		\sa getState
		\sa getDefaultState
		\sa getOffState
	*/
	virtual void setState(ConditionState state);

	/*!
		\brief Get current condition range
	*/
	virtual ConditionState getState();

	/*!
		\brief Get default on state

		Can be used when changing the device condition state from off to on
		but the previous on state for the condition is not known.

		Only makes sense for light/dimming and volume conditions.

		\sa setState
	*/
	virtual ConditionState getDefaultState();

	/*!
		\brief Get off state

		Device condition state that corresponds to an "off" device state.

		Only makes sense for light/dimming and volume conditions.

		\sa setState
	*/
	virtual ConditionState getOffState();

public slots:
	// Invoked when UP button is pressed
	virtual void Up();
	// Invoked when DOWN button is pressed
	virtual void Down();

signals:
	/*!
		\brief Emitted when the condition on device is satisfied.
	*/
	void condSatisfied();

protected:
	/*!
		\brief Constructor

		Build the %DeviceCondition object and use the DeviceConditionDisplayInterface
		\a cond_display object the draw the graphical representation.
		\a start_initialized initial value for the 'initialized' field
	*/
	DeviceCondition(DeviceConditionDisplayInterface *cond_display, bool start_initialized = false);

	// The method to update the descriptive text of the condition using the
	// the DeviceConditionDisplay widget.
	void updateText(int min_condition_value, int max_condition_value);

	void setDevice(device *d);

	// Parse the values cames from the correspondent device and set the 'satisfied'
	// var properly.
	virtual bool parseValues(const DeviceValues &values_list) = 0;

	virtual int get_min();
	virtual int get_max();
	virtual int get_step();
	// Sets condition value
	void set_condition_value(int);
	// Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);
	// Gets condition value
	virtual int get_condition_value();
	// Draws frame
	virtual void Draw() = 0;

	// Returns current value for condition
	int get_current_value();
	// Sets current value for condition
	int set_current_value(int);

	// True when condition is satisfied
	bool satisfied;

	// Invoked when called the public methods save/reset
	virtual void onConditionSaved();
	virtual void onConditionReset();

	 // true if the condition is changed (but not yet saved)
	virtual bool conditionChanged();

private slots:
	void valueReceived(const DeviceValues &values_list);

private:
	// Condition value
	int cond_value;
	// Current value (displayed, not confirmed)
	int current_value;

	DeviceConditionDisplayInterface *condition_display;

	device *dev;
	// Because we won't trigger during the device initialization, we use this
	// flag to recognize if trigger or not.
	bool initialized;
};


/*!
	\ingroup Scenarios
	\brief Represents a light based device condition.
*/
class DeviceConditionLight : public DeviceCondition
{
Q_OBJECT
public:
	DeviceConditionLight(DeviceConditionDisplayInterface* condition_display, QString trigger, QString where,
		int openserver_id = 0, PullMode pull_mode = PULL_UNKNOWN);

	virtual ConditionState getDefaultState();
	virtual ConditionState getOffState();

protected:
	virtual void Draw();
	virtual int get_max();
	// Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);

	virtual bool parseValues(const DeviceValues &values_list);
};


/*!
	\ingroup Scenarios
	\brief Represents a dimmer based device condition.
*/
class DeviceConditionDimming : public DeviceCondition
{
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:
	DeviceConditionDimming(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where,
		int openserver_id = 0, PullMode pull_mode = PULL_UNKNOWN, bool down_off = true);

	virtual QString getConditionAsString();

	virtual void setState(ConditionState state);
	virtual ConditionState getState();
	virtual ConditionState getDefaultState();
	virtual ConditionState getOffState();

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

	// Translates trigger condition from open encoding to int and sets val
	virtual void set_condition_value(QString);

	virtual bool parseValues(const DeviceValues &values_list);

private:
	int min_val;
	int max_val;
	int current_value_min;
	int current_value_max;
	// prevent Down() from changing device state to "off"
	bool down_off;
};


/*!
	\ingroup Scenarios
	\brief Represents a dimmer 100 based device condition.
*/
class DeviceConditionDimming100 : public DeviceCondition
{
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:
	DeviceConditionDimming100(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where,
		int openserver_id = 0, PullMode pull_mode = PULL_UNKNOWN, bool down_off = true);

	virtual QString getConditionAsString();

	virtual void setState(ConditionState state);
	virtual ConditionState getState();
	virtual ConditionState getDefaultState();
	virtual ConditionState getOffState();

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
	QString get_current_value();

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
	// prevent Down() from changing device state to "off"
	bool down_off;
};


/*!
	\ingroup Scenarios
	\brief Represents a volume based device condition.
*/
class DeviceConditionVolume : public DeviceCondition
{
friend class TestScenEvoDevicesCond;
Q_OBJECT
public:
	DeviceConditionVolume(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where, bool down_off = true);

	virtual QString getConditionAsString();

	virtual void setState(ConditionState state);
	virtual ConditionState getState();
	virtual ConditionState getDefaultState();
	virtual ConditionState getOffState();

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
	bool amplifier_on;
	// prevent Down() from changing device state to "off"
	bool down_off;
};


/*!
	\ingroup Scenarios
	\brief Represents a temperature based device condition.
*/
class DeviceConditionTemperature : public DeviceCondition
{
Q_OBJECT
public:
	DeviceConditionTemperature(DeviceConditionDisplayInterface* cond_display, QString trigger, QString where, bool external = false, int openserver_id = 0);

	virtual QString getConditionAsString();

protected:
	int get_min();
	virtual int get_max();
	int intValue();
	virtual void Draw();

	virtual bool parseValues(const DeviceValues &values_list);

private:
	// Maximum and minimum values for temperature conditions
	int max_temp, min_temp;

	TemperatureScale temp_scale;
};


/*!
	\ingroup Scenarios
	\brief Represents an aux based device condition.
*/
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
