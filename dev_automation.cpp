#include "dev_automation.h"
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


PPTStatDevice::PPTStatDevice(QString address) : device(QString("25"), address)
{
}

void PPTStatDevice::requestStatus() const
{
	sendInit(createStatusRequestOpen(who, where));
}

void PPTStatDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who() || where.toInt() != msg.where())
		return;

	int what = msg.what();

	if (what == REQ_STATUS_OPEN || what == REQ_STATUS_CLOSE)
	{
		StatusList status_list;
		status_list[DIM_STATUS] = QVariant(what == REQ_STATUS_OPEN);
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

