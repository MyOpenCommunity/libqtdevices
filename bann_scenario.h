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


#ifndef BANN_SCENARIO_H
#define BANN_SCENARIO_H

#include "bann2_buttons.h" // Bann2Buttons
#include "bann3_buttons.h" // Bann3Buttons
#include "bann4_buttons.h" // Bann4ButtonsIcon, Bann4Buttons
#include "device.h" // DeviceValues

class ScenarioPlusDevice;
class ScenEvoCondition;
class ScenEvoTimeCondition;
class ScenEvoDeviceCondition;
class QTimerEvent;
class ScenarioDevice;


/*!
	\ingroup Scenarios
	\brief Activate a scenario.
*/
class BannSimpleScenario : public Bann2Buttons
{
Q_OBJECT
public:
	BannSimpleScenario(int scenario, const QString &descr, const QString &where, int openserver_id);

private slots:
	void activate();

private:
	ScenarioDevice *dev;
	int scenario_number;
};


/*!
	\ingroup Scenarios
	\brief Manages a module scenario.

	A module scenario is a physical device that can be used to send a set
	of frames, that represent the scenario, to other physical devices.
	This set of frames is stored in the module scenario itself, and it can be
	changed reprogramming the module scenario.
*/
class ScenarioModule : public Bann4ButtonsIcon
{
Q_OBJECT
public:
	ScenarioModule(int scenario, const QString &descr, const QString &where, int openserver_id);

private slots:
	void valueReceived(const DeviceValues &values_list);
	void activate();
	void editScenario();
	void startEditing();
	void deleteScenario();
	void stopEditing();

private:
	void changeLeftFunction(const char *slot);
	ScenarioDevice *dev;
	int scenario_number;
	bool is_editing;
	// if this banner has NOT started scenario programming, it must hide the right (pencil) button when
	// a START_PROG frame arrives
	bool has_started_prog;
};


/*!
	\ingroup Scenarios
	\brief Represents an improved scenario.

	This banner allows the user to set and modify and improved scenario, that
	can perform an action (sending a frame) depends on a device or/and a time
	condition.
*/
class ScenarioEvolved : public Bann3Buttons
{
Q_OBJECT
public:
	/*!
		\brief Constructor.

		Given a ScenEvoTimeCondition \a tconf and a ScenEvoDeviceCondition \a dcond
		build the %ScenarioEvolved banner that monitors the conditions in order
		to perform the \a action argument when both the conditions are satisfied.
	*/
	ScenarioEvolved(int item_id, QString descr, QString action, bool enabled,
		ScenEvoTimeCondition *tcond, ScenEvoDeviceCondition *dcond);

public slots:
	void inizializza(bool forza = false);

private slots:
	void toggleActivation();
	void forceTrig();
	void trig(bool forced = false);
	void trigOnStatusChanged();

	void reset();
	void save();

private:
	QString action, enable_icon, disable_icon;
	bool enabled;
	int item_id;
	ScenEvoTimeCondition *time_cond;
	ScenEvoDeviceCondition *device_cond;
};


/*!
	\ingroup Scenarios
	\brief Manages a scenario programmer.

	A scenario programmer is a physical device that can be used to send a set
	of frames, that represent the scenario, to other physical devices.
	Unlike the module scenario, the scenario programmer cannot be reprogrammed,
	but you can enable/disable the scenario and manually start/stop.
*/
class ScheduledScenario : public Bann4Buttons
{
Q_OBJECT
public:
	ScheduledScenario(const QString &enable, const QString &start, const QString &stop, const QString &disable, const QString &descr);

private slots:
	void enable();
	void disable();
	void start();
	void stop();

private:
	QString action_enable, action_disable, action_start, action_stop;
};


/*!
	\ingroup Scenarios
	\brief Represents a scenario plus.

	With the scenario plus there isn't a physical device that stores the frames
	(like the module scenario), instead every single devices have to manage
	the scenario plus frames in order to perform the scenario.

	\sa ScenarioPlusDevice
*/
class ScenarioPlus : public Bann4Buttons
{
Q_OBJECT
public:
	ScenarioPlus(const QString &descr, const QString &where, int openserver_id);

protected:
	virtual void timerEvent(QTimerEvent *e);

private:
	ScenarioPlusDevice *dev;
	int increase_timer, decrease_timer;

private slots:
	void startIncrease();
	void startDecrease();
	void stop();
};

#endif
