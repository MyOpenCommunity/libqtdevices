#include "automation_device.h"

#include <openmsg.h>

#include <QDebug>
#include <QVariant>


enum RequestDimension
{
	REQ_STATUS_OPEN = 32,
	REQ_STATUS_CLOSE = 31
};


AutomationDevice::AutomationDevice(QString where, PullMode m) :
	PullDevice(QString("2"), where, m)
{
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

void AutomationDevice::requestPullStatus()
{
	requestStatus();
}

void AutomationDevice::parseFrame(OpenMsg &msg, StatusList *sl)
{
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
	(*sl)[msg.what()] = v;
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

void PPTStatDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));
	if (who.toInt() == msg.who())
		manageFrame(msg);
}

