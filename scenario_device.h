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


#ifndef SCENARIO_DEVICE_H
#define SCENARIO_DEVICE_H

#include "device.h"

class OpenMsg;


/*!
	\ingroup Scenarios
	\brief The first parameter is true during the programmation of the scenario
	module, while the second identifies the number of the scenario.
*/
typedef QPair<bool, int> ScenarioProgrammingStatus;
Q_DECLARE_METATYPE(ScenarioProgrammingStatus);

/*!
	\ingroup Scenarios
	\brief Device for Scenarios system management.

	ScenarioDevice permits to start and stop the programming of a scenario
	(startProgramming() and stopProgramming()), activate a previusly programmed
	scenario (activateScenario()) and delete one or all scenarios
	(deleteScenario() and deleteAll()).

	To retrive the status of the scenarios you can use the requestStatus() method.

	\section ScenarioDevice-dimensions Dimensions
	\startdim
	\dim{DIM_START,ScenarioProgrammingStatus,,Scenario programming status.
	When stops the ScenarioProgrammingStatus.first() is false. A DIM_START update
	also means that the device is unlocked. }
	\dim{DIM_LOCK,bool,,True if device is locked\, false otherwise.}
	\enddim

	\sa \ref device-dimensions
*/
class ScenarioDevice : public device
{
friend class TestScenarioDevice;
public:
	/*!
		\refdim{ScenarioDevice}
	*/
	enum Type
	{
		DIM_START,
		DIM_LOCK,
	};

	enum ScenarioNumber
	{
		ALL_SCENARIOS = -1, /*!< All scenarios (dummy value, must be different from legal values) */
	};

	/*!
		\brief Constructor.

		Constructs a new ScenarioDevice with the given \a where and
		\a openserver_id.
	*/
	ScenarioDevice(QString where, int openserver_id = 0);
	virtual void init();

	/*!
		\brief Activates the scenario \a scen.
	*/
	void activateScenario(int scen);
	/*!
		\brief Starts the programming of scenario \a scen.
	*/
	void startProgramming(int scen);
	/*!
		\brief Stops the programming of scenario \a scen.
	*/
	void stopProgramming(int scen);
	/*!
		\brief Deletes all the scenarios.
	*/
	void deleteAll();
	/*!
		\brief Deletes the scenario \a scen.
	*/
	void deleteScenario(int scen);

	/*!
		\brief Requests the status of the device.
	*/
	void requestStatus();

	virtual bool parseFrame(OpenMsg &msg, DeviceValues &values_list);

private:
	bool is_unlocked;
};


/*!
	\ingroup Scenarios
	\brief Manages a scenario plus.

	This device can be used to send standard scenario frames that some physical
	devices can interpret to perform the scenario.
*/
class ScenarioPlusDevice : public device
{
Q_OBJECT
public:
	ScenarioPlusDevice(QString address, int openserver_id = 0);

	/*!
		\brief Send the increase command.
	*/
	void increase() const;

	/*!
		\brief Send the decrease command.
	*/
	void decrease() const;

	/*!
		\brief Send the stop command.
	*/
	void stop() const;

public slots:
	/*!
		\brief Send the turn on command.
	*/
	void turnOn() const;

	/*!
		\brief Send the turn off command.
	*/
	void turnOff() const;
};

#endif // SCENARIO_DEVICE_H
