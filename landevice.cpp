#include "landevice.h"

#include <openmsg.h>

#include <QDebug>

#define DIM_STATUS 9
#define DIM_IP 10
#define DIM_NETMASK 11
#define DIM_MACADDR 12
#define DIM_GATEWAY 50
#define DIM_DNS1 51
#define DIM_DNS2 52


LanDevice::LanDevice() : device(QString("13"), QString(""))
{
}

void LanDevice::enableLan(bool enable)
{
	int val = enable ? 1 : 0;
	int what = DIM_STATUS;
	sendFrame(QString("*#%1**#%2*%3##").arg(who).arg(what).arg(val));
}

void LanDevice::sendRequest(int what)
{
	sendInit(QString("*#%1**%2##").arg(who).arg(what));
}

void LanDevice::requestStatus()
{
	sendRequest(DIM_STATUS);
}

void LanDevice::requestIp()
{
	sendRequest(DIM_IP);
}

void LanDevice::requestNetmask()
{
	sendRequest(DIM_NETMASK);
}

void LanDevice::requestMacAddress()
{
	sendRequest(DIM_MACADDR);
}

void LanDevice::requestGateway()
{
	sendRequest(DIM_GATEWAY);
}

void LanDevice::requestDNS1()
{
	sendRequest(DIM_DNS1);
}

void LanDevice::requestDNS2()
{
	sendRequest(DIM_DNS2);
}

void LanDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who())
		return;

	if (msg.what() == DIM_MACADDR || msg.what() == DIM_IP || msg.what() == DIM_NETMASK ||
		msg.what() == DIM_GATEWAY || msg.what() == DIM_DNS1 || msg.what() == DIM_DNS2 ||
		msg.what() == DIM_STATUS)
	{
		qDebug("LanDevice::frame_rx_handler");
		qDebug("frame read:%s", frame);
	}
}
