#include "scenario_device.h"

#include "openmsg.h"

enum
{
	START_PROG = 40,
	STOP_PROG = 41,
	DELETE = 42,
	LOCK = 43,
	UNLOCK = 44,
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

void ScenarioDevice::requestStatus()
{
	sendRequest(QString());
}

void ScenarioDevice::manageFrame(OpenMsg &msg)
{
	if (QString::fromStdString(msg.whereFull()) != where)
		return;

	int what = msg.what();
	QVariant v;
	StatusList sl;
	int status_index;
	switch (what)
	{
	case LOCK:
		status_index = DIM_LOCK;
		v.setValue(true);
		break;
	case UNLOCK:
		status_index = DIM_LOCK;
		v.setValue(false);
		break;
	case START_PROG:
		// TODO: here I'm discarding the scenario being modified.
		// Does GUI need to know it?
		status_index = DIM_START;
		v.setValue(true);
		break;
	case STOP_PROG:
		status_index = DIM_START;
		v.setValue(false);
		break;
	}

	sl[status_index] = v;
	emit status_changed(sl);
}
