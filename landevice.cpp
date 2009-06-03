#include "landevice.h"
#include "generic_functions.h" // createRequestOpen

#include <openmsg.h>

#include <QDebug>
#include <QStringList>

#include <assert.h>

// the request delay in milliseconds
#define STATUS_REQUEST_DELAY 1000


LanDevice::LanDevice() : device(QString("13"), QString(""))
{
}

void LanDevice::enableLan(bool enable)
{
	int val = enable ? 1 : 0;
	int what = DIM_STATUS;
	sendFrame(QString("*#%1**#%2*%3##").arg(who).arg(what).arg(val));
	// We need to perform a status request after the command to toggle the status,
	// but if we put the requestStatus call immediately after the command frame
	// the result is that the status request is received by the openserver before
	// the command. To preserve the order, we have to use a singleshot timer with
	// some delay.
	QTimer::singleShot(STATUS_REQUEST_DELAY, this, SLOT(requestStatus()));
}

void LanDevice::sendRequest(int what) const
{
	sendInit(createRequestOpen(who, QString::number(what), ""));
}

void LanDevice::requestStatus() const
{
	sendRequest(DIM_STATUS);
}

void LanDevice::requestIp() const
{
	sendRequest(DIM_IP);
}

void LanDevice::requestNetmask() const
{
	sendRequest(DIM_NETMASK);
}

void LanDevice::requestMacAddress() const
{
	sendRequest(DIM_MACADDR);
}

void LanDevice::requestGateway() const
{
	sendRequest(DIM_GATEWAY);
}

void LanDevice::requestDNS1() const
{
	sendRequest(DIM_DNS1);
}

void LanDevice::requestDNS2() const
{
	sendRequest(DIM_DNS2);
}

void LanDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who())
		return;

	int what = msg.what();
	int what_args = msg.whatArgCnt();

	StatusList status_list;
	QVariant v;

	if (what == DIM_MACADDR || what == DIM_IP || what == DIM_NETMASK ||
		what == DIM_GATEWAY || what == DIM_DNS1 || what == DIM_DNS2 ||
		what == DIM_STATUS)
	{
		qDebug("LanDevice::frame_rx_handler -> frame read:%s", frame);

		switch (what)
		{
		case DIM_STATUS:
		{
			bool st = msg.whatArgN(0) == 1;
			v.setValue(st);
			break;
		}
		case DIM_MACADDR:
		{
			QStringList parts;
			for (int i = 0; i < what_args; ++i)
				parts << QString::number(msg.whatArgN(i), 16).rightJustified(2, '0');
			v.setValue(parts.join(":"));
			break;
		}
		default:
		{
			assert(what_args == 4); // IPv4 ip are composed by 4 parts
			QStringList parts;
			for (int i = 0; i < what_args; ++i)
				parts << QString::number(msg.whatArgN(i));
			v.setValue(parts.join("."));
			break;
		}
		}

		status_list[what] = v;
		emit status_changed(status_list);
	}
}
