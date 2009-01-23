#include "landevice.h"

#include <openmsg.h>

#include <QDebug>


LanDevice::LanDevice() : device(QString("13"), QString(""))
{
}

void LanDevice::enableLan(bool enable)
{
	int val = enable ? 1 : 0;
	int what = 9;
	sendFrame(QString("*#%1**#%2*%3##").arg(who).arg(what).arg(val));
}

void LanDevice::sendRequest(int what)
{
	sendInit(QString("*#%1**%2##").arg(who).arg(what));
}

void LanDevice::requestStatus()
{
	sendRequest(9);
}

void LanDevice::requestIp()
{
	sendRequest(10);
}

void LanDevice::requestNetmask()
{
	sendRequest(11);
}

void LanDevice::requestMacAddress()
{
	sendRequest(12);
}

void LanDevice::requestGateway()
{
	sendRequest(20);
}

void LanDevice::requestDNS1()
{
	sendRequest(51);
}

void LanDevice::requestDNS2()
{
	sendRequest(52);
}

void LanDevice::frame_rx_handler(char *frame)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame, strlen(frame));

	if (who.toInt() != msg.who())
		return;

	if (msg.what() == 12 || msg.what() == 10 || msg.what() == 11 || msg.what() == 20 ||
		msg.what() == 51 || msg.what() == 52 || msg.what() == 9)
	{
		qDebug("LanDevice::frame_rx_handler");
		qDebug("frame read:%s", frame);
	}
}
