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


#include "scenario_device.h"

#include "openmsg.h"
#include <QDebug>

enum
{
	START_PROG = 40,
	STOP_PROG = 41,
	DELETE = 42,
	LOCK = 43,
	UNLOCK = 44,
};

// PPTSce commands
#define CMD_PPT_SCE_ON "11#0"
#define CMD_PPT_SCE_OFF "12"
#define CMD_PPT_SCE_INC "13#0#1"
#define CMD_PPT_SCE_DEC "14#0#1"
#define CMD_PPT_SCE_STOP "15"


ScenarioDevice::ScenarioDevice(QString where, int openserver_id) :
	device(QString("0"), where, openserver_id)
{
	is_unlocked = false;
}

void ScenarioDevice::init()
{
	requestStatus();
}

void ScenarioDevice::activateScenario(int scen)
{
	Q_ASSERT_X(scen >= 1 && scen <= 31, "ScenarioDevice::activateScenario",
		"Valid scenarios are between 1 and 31");
	sendCommand(scen);
}

void ScenarioDevice::startProgramming(int scen)
{
	Q_ASSERT_X(scen >= 1 && scen <= 31, "ScenarioDevice::startProgramming",
		"Valid scenarios are between 1 and 31");
	sendCommand(QString("%1#%2").arg(START_PROG).arg(scen));
}

void ScenarioDevice::stopProgramming(int scen)
{
	Q_ASSERT_X(scen >= 1 && scen <= 31, "ScenarioDevice::stopProgramming",
		"Valid scenarios are between 1 and 31");
	sendCommand(QString("%1#%2").arg(STOP_PROG).arg(scen));
}

void ScenarioDevice::deleteAll()
{
	sendCommand(DELETE);
}

void ScenarioDevice::deleteScenario(int scen)
{
	Q_ASSERT_X(scen >= 1 && scen <= 31, "ScenarioDevice::deleteScenario",
		"Valid scenarios are between 1 and 31");
	sendCommand(QString("%1#%2").arg(DELETE).arg(scen));
}

void ScenarioDevice::requestStatus()
{
	sendRequest(QString());
}

void ScenarioDevice::manageFrame(OpenMsg &msg)
{
	int what = msg.what();
	int what_arg_count = msg.whatArgCnt();
	// Here we need to check if incoming frame is in the form
	// *0*40*<where>##
	// since this locks all devices (not only our own address).
	if ((!(what == START_PROG && what_arg_count == 0)) && (QString::fromStdString(msg.whereFull()) != where))
		return;

	QVariant v;
	DeviceValues sl;
	int status_index = -1;
	switch (what)
	{
	case LOCK:
		status_index = DIM_LOCK;
		v.setValue(true);
		is_unlocked = false;
		break;
	case UNLOCK:
		if (!is_unlocked)
		{
			status_index = DIM_LOCK;
			v.setValue(false);
			is_unlocked = true;
		}
		break;
	case START_PROG:
	{
		// if we receive a start programming, the device is unlocked
		is_unlocked = true;
		// use SCENARIO_ALL to indicate we are locking the whole device
		ScenarioProgrammingStatus p;
		if (what_arg_count > 0)
			p = ScenarioProgrammingStatus(true, msg.whatArgN(0));
		else
			p = ScenarioProgrammingStatus(true, ALL_SCENARIOS);
		status_index = DIM_START;
		v.setValue(p);
	}
		break;
	case STOP_PROG:
	{
		// use SCENARIO_ALL when receiving *0*41*<where>## frames
		ScenarioProgrammingStatus p;
		if (what_arg_count > 0)
			p = ScenarioProgrammingStatus(false, msg.whatArgN(0));
		else
			p = ScenarioProgrammingStatus(false, ALL_SCENARIOS);
		status_index = DIM_START;
		v.setValue(p);
	}
		break;
	}

	if (status_index > -1)
	{
		sl[status_index] = v;
		emit status_changed(sl);
	}
}


PPTSceDevice::PPTSceDevice(QString address, int openserver_id) : device(QString("25"), address, openserver_id)
{
}

void PPTSceDevice::turnOn() const
{
	sendCommand(CMD_PPT_SCE_ON);
}

void PPTSceDevice::turnOff() const
{
	sendCommand(CMD_PPT_SCE_OFF);
}

void PPTSceDevice::increase() const
{
	sendCommand(CMD_PPT_SCE_INC);
}

void PPTSceDevice::decrease() const
{
	sendCommand(CMD_PPT_SCE_DEC);
}

void PPTSceDevice::stop() const
{
	sendCommand(CMD_PPT_SCE_STOP);
}

