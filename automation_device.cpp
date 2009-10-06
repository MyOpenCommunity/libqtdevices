#include "automation_device.h"
#include "generic_functions.h" // createStatusRequestOpen

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


AutomationDevice::AutomationDevice(QString where, AutomationDevice::PullMode m) :
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
	// TODO: this must be refactored, since is cut-and-pasted from
	// lighting device
	// Probably a PullEnabledDevice will do the trick: reimplement manageFrame()
	// to handle the common part and let each device parse the frame by calling
	// a virtual protected parseFrame().
	// However, I don't still understand completely the mechanics of mode changing
	// so I'm pasting the code for now.

	// true if the frame is general or environment (not group).
	bool is_multi_receiver_frame = false;

	bool is_our = (QString::fromStdString(msg.whereFull()) == where);
	if (!is_our && mode != PULL)
	{
		// here we check if address is general or environment
		is_our = checkAddressIsForMe(msg.Extract_dove(), where);
		is_multi_receiver_frame = is_our;
	}
	if (!is_our)
		return;

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

