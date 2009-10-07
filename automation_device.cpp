#include "automation_device.h"

#include <openmsg.h>

#include <QDebug>
#include <QVariant>


enum RequestDimension
{
	REQ_STATUS_OPEN = 32,
	REQ_STATUS_CLOSE = 31
};

// PPTSce commands
#define CMD_PPT_SCE_ON "11#0"
#define CMD_PPT_SCE_OFF "12"
#define CMD_PPT_SCE_INC "13#0#1"
#define CMD_PPT_SCE_DEC "14#0#1"
#define CMD_PPT_SCE_STOP "15"


AutomationDevice::AutomationDevice(QString where, PullMode m) :
	device(QString("2"), where)
{
	mode = m;
}

void AutomationDevice::goUp()
{
	sendCommand(QString::number(DIM_UP));
}

void AutomationDevice::goDown()
{
	sendCommand(QString::number(DIM_DOWN));
}

void AutomationDevice::stop()
{
	sendCommand(QString::number(DIM_STOP));
}

void AutomationDevice::requestStatus()
{
	sendRequest(QString());
}

void AutomationDevice::manageFrame(OpenMsg &msg)
{
	// true if the frame is general or environment (not group).
	bool is_multi_receiver_frame = false;

	switch (checkAddressIsForMe(QString::fromStdString(msg.whereFull()), where, mode))
	{
	case NOT_MINE:
		return;
	case GLOBAL:
	case ENVIRONMENT:
		is_multi_receiver_frame = true;
		break;
	default:
		break;
	}

	StatusList sl;
	QVariant v;
	int what = msg.what();
	switch (what)
	{
	case DIM_UP:
		v.setValue(true);
		break;
	case DIM_DOWN:
		v.setValue(true);
		break;
	case DIM_STOP:
		v.setValue(true);
		break;
	}
	sl[msg.what()] = v;

	// when mode is unknown and the frame is for multiple receivers (ie it's a general or
	// environment frame), we must send a status request to the device before sending
	// a status_changed()
	if (mode == PULL_UNKNOWN && is_multi_receiver_frame)
		// TODO: optimize this scenario
		requestStatus();
	else
		emit status_changed(sl);
}




PPTStatDevice::PPTStatDevice(QString address) : device(QString("25"), address)
{
}

void PPTStatDevice::requestStatus() const
{
	sendRequest(QString());
}

void PPTStatDevice::manageFrame(OpenMsg &msg)
{
	if (where.toInt() != msg.where())
		return;

	int what = msg.what();

	if (what == REQ_STATUS_OPEN || what == REQ_STATUS_CLOSE)
	{
		StatusList status_list;
		status_list[DIM_STATUS] = QVariant(what == REQ_STATUS_CLOSE);
		emit status_changed(status_list);
	}
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

