#include "dev_automation.h"
#include "generic_functions.h" // createStatusRequestOpen

#include <openmsg.h>

#include <QDebug>


enum RequestDimension
{
	REQ_STATUS_OPEN = 32,
	REQ_STATUS_CLOSE = 31
};


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
