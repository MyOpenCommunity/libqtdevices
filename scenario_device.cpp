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


ScenarioDevice::ScenarioDevice(QString where) :
	device(QString("0"), where)
{
}

void ScenarioDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));
	if (who.toInt() == msg.who())
		manageFrame(msg);
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
	int what = msg.what();
	int what_arg_count = msg.whatArgCnt();
	// Here we need to check if incoming frame is in the form
	// *0*40*<where>##
	// since this locks all devices (not only our own address).
	if ((!(what == START_PROG && what_arg_count == 0)) && (QString::fromStdString(msg.whereFull()) != where))
		return;

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
	{
		// TODO: use SCENARIO_ALL to indicate we are locking the whole device
		ScenarioProgrammingStatus p;
		if (what_arg_count > 0)
			p = ScenarioProgrammingStatus(true, msg.whatArgN(0));
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

	sl[status_index] = v;
	emit status_changed(sl);
}


PPTSceDevice::PPTSceDevice(QString address) : device(QString("25"), address)
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

