#include "test_platform_device.h"
#include "device_tester.h"
#include "bttime.h"
#include "openclient.h"
#include "openserver_mock.h"

#include <platform_device.h>

#include <QVariant>


void TestPlatformDevice::initTestCase()
{
	dev = new PlatformDevice;
}

void TestPlatformDevice::cleanupTestCase()
{
	delete dev;
}

void TestPlatformDevice::receiveStatus()
{
	DeviceTester t(dev, PlatformDevice::DIM_STATUS);
	t.check("*#13**9*0##", false);
	t.check("*#13**9*1##", true);
}

void TestPlatformDevice::receiveIp()
{
	DeviceTester t(dev, PlatformDevice::DIM_IP);
	t.check("*#13**10*10*3*3*81##", "10.3.3.81");
}

void TestPlatformDevice::receiveNetmask()
{
	DeviceTester t(dev, PlatformDevice::DIM_NETMASK);
	t.check("*#13**11*255*255*255*0##", "255.255.255.0");
}

void TestPlatformDevice::receiveMacAddress()
{
	DeviceTester t(dev, PlatformDevice::DIM_MACADDR);
	t.check("*#13**12*0*3*80*0*34*45##", "00:03:50:00:22:2d");
}

void TestPlatformDevice::receiveGateway()
{
	DeviceTester t(dev, PlatformDevice::DIM_GATEWAY);
	t.check("*#13**50*27*238*64*1##", "27.238.64.1");
}

void TestPlatformDevice::receiveDns1()
{
	DeviceTester t(dev, PlatformDevice::DIM_DNS1);
	t.check("*#13**51*208*67*222*222##", "208.67.222.222");
}

void TestPlatformDevice::receiveDns2()
{
	DeviceTester t(dev, PlatformDevice::DIM_DNS2);
	t.check("*#13**52*208*67*220*220*##", "208.67.220.220");
}

void TestPlatformDevice::sendSetDate()
{
	dev->setDate(QDate(2010, 11, 12));
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#13**#1*00*12*11*2010##"));
}

void TestPlatformDevice::sendSetTime()
{
	dev->setTime(BtTime(13, 12, 11));
	client_command->flush();
	QCOMPARE(server->frameCommand(), QString("*#13**#0*13*12*11**##"));
}

void TestPlatformDevice::receiveFirmwareVersion()
{
	DeviceTester t(dev, PlatformDevice::DIM_FW_VERS);
	QString frame = "*#13**16*1*1*26##";
	t.check(frame, "1.1.26");
}

void TestPlatformDevice::receiveKernelVersion()
{
	DeviceTester t(dev, PlatformDevice::DIM_KERN_VERS);
	t.check("*#13**23*2*3*3##", "2.3.3");
}
