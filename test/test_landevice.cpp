#include "test_landevice.h"
#include "device_tester.h"
#include "openserver_mock.h"

#include <landevice.h>

#include <QVariant>
#include <QMetaType>


void TestLanDevice::initTestCase()
{
	// To use StatusList in signal/slots and watch them through QSignalSpy
	qRegisterMetaType<StatusList>("StatusList");

	server = new OpenServerMock;
	device::setClients(server->connectCommand(), server->connectMonitor(), server->connectRequest());
	dev = new LanDevice;
}

void TestLanDevice::cleanupTestCase()
{
	delete dev;
	delete server;
}

void TestLanDevice::readStatus()
{
	DeviceTester t(dev, LanDevice::DIM_STATUS);
	t.check("*#13**9*0##", false);
	t.check("*#13**9*1##", true);
}

void TestLanDevice::readIp()
{
	DeviceTester t(dev, LanDevice::DIM_IP);
	t.check("*#13**10*10*3*3*81##", "10.3.3.81");
}

void TestLanDevice::readNetmask()
{
	DeviceTester t(dev, LanDevice::DIM_NETMASK);
	t.check("*#13**11*255*255*255*0##", "255.255.255.0");
}

void TestLanDevice::readMacAddress()
{
	DeviceTester t(dev, LanDevice::DIM_MACADDR);
	t.check("*#13**12*0*3*80*0*34*45##", "00:03:50:00:22:2d");
}

void TestLanDevice::readGateway()
{
	DeviceTester t(dev, LanDevice::DIM_GATEWAY);
	t.check("*#13**50*27*238*64*1##", "27.238.64.1");
}

void TestLanDevice::readDns1()
{
	DeviceTester t(dev, LanDevice::DIM_DNS1);
	t.check("*#13**51*208*67*222*222##", "208.67.222.222");
}

void TestLanDevice::readDns2()
{
	DeviceTester t(dev, LanDevice::DIM_DNS2);
	t.check("*#13**52*208*67*220*220*##", "208.67.220.220");
}

