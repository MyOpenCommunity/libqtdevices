#include "scenario_device.h"

enum
{
	START_PROG = 40,
	STOP_PROG = 41,
	DELETE = 42,
};

ScenarioDevice::ScenarioDevice(QString where) :
	device(QString("0"), where)
{
}

void ScenarioDevice::activateScenario(int scen)
{
	Q_ASSERT_X(scen >= 1 && scen <= 31, "ScenarioDevice::activateScenario",
		"Valid scenarios are between 1 and 31");
	sendCommand(QString::number(scen));
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
	sendCommand(QString::number(DELETE));
}

void ScenarioDevice::deleteScenario(int scen)
{
	Q_ASSERT_X(scen >= 1 && scen <= 31, "ScenarioDevice::deleteScenario",
		"Valid scenarios are between 1 and 31");
	sendCommand(QString("%1#%2").arg(DELETE).arg(scen));
}

void ScenarioDevice::manageFrame(OpenMsg &msg)
{
	// TODO: implement me!
}
