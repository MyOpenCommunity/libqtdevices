#include "automation_device.h"

#include <openmsg.h>

#include <QDebug>
#include <QVariant>


enum RequestDimension
{
	REQ_STATUS_OPEN = 32,
	REQ_STATUS_CLOSE = 31
};


AutomationDevice::AutomationDevice(QString where, PullMode m, int openserver_id) :
	PullDevice(QString("2"), where, m, openserver_id)
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


PPTStatDevice::PPTStatDevice(QString address, int openserver_id) :
	device(QString("25"), address, openserver_id)
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

