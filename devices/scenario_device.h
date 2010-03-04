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

typedef QPair<bool, int> ScenarioProgrammingStatus;
Q_DECLARE_METATYPE(ScenarioProgrammingStatus);


class ScenarioDevice : public device
{
friend class TestScenarioDevice;
public:
	enum Type
	{
		DIM_START,     // scenario programming start (false when stops). A DIM_START update means
		               // the device is also unlocked.
		DIM_LOCK,      // device lock enabled (false when unlock)
	};

	enum
	{
		ALL_SCENARIOS = -1,     // all scenarios (dummy value, must be different from legal values)
	};

	ScenarioDevice(QString where, int openserver_id = 0);
	virtual void init();

	void activateScenario(int scen);
	void startProgramming(int scen);
	void stopProgramming(int scen);
	void deleteAll();
	void deleteScenario(int scen);

	void requestStatus();

	virtual void manageFrame(OpenMsg &msg);

private:
	bool is_unlocked;
};


class PPTSceDevice : public device
{
Q_OBJECT
public:
	PPTSceDevice(QString address, int openserver_id = 0);
	void increase() const;
	void decrease() const;
	void stop() const;

public slots:
	void turnOn() const;
	void turnOff() const;
};

#endif // SCENARIO_DEVICE_H
