#include "landevice.h"

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
	sendInit(QString("*#%1**#%2##").arg(who).arg(what));
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
}
