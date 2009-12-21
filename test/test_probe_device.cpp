#include "test_probe_device.h"
#include "openserver_mock.h"
#include "device_tester.h"
#include "openclient.h"
#include "probe_device.h"

#include <openmsg.h>

#include <QtTest/QtTest>

#define ZERO_FILL QLatin1Char('0')

// TestNonControlledProbeDevice implementation

void TestNonControlledProbeDevice::initTestCase()
{
	dev = new NonControlledProbeDevice("11", NonControlledProbeDevice::INTERNAL);
}

void TestNonControlledProbeDevice::cleanupTestCase()
{
	delete dev;
	dev = NULL;
}

void TestNonControlledProbeDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*11*0##"));
}

void TestNonControlledProbeDevice::receiveTemperature()
{
	DeviceTester tst(dev, NonControlledProbeDevice::DIM_TEMPERATURE);

	tst.check("*#4*11*0*123##", 123);
}

// TestExternalProbeDevice implementation

void TestExternalProbeDevice::initTestCase()
{
	dev = new NonControlledProbeDevice("11", NonControlledProbeDevice::EXTERNAL);
}

void TestExternalProbeDevice::cleanupTestCase()
{
	delete dev;
	dev = NULL;
}

void TestExternalProbeDevice::sendRequestStatus()
{
	dev->requestStatus();
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#4*11*15#1##"));
}

void TestExternalProbeDevice::receiveTemperature()
{
	DeviceTester tst(dev, NonControlledProbeDevice::DIM_TEMPERATURE);

	tst.check("*#4*11*15*1*123*1111##", 123);
}
