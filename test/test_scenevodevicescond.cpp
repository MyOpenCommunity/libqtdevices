#include "test_scenevodevicescond.h"
#include "scenevodevicescond.h"
#include "openserver_mock.h"
#include "openclient.h"
#include "devices_cache.h" // bt_global::devices_cache

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QHash>
#include <QPair>


/**
 * This empty class is required because every DeviceCondition use a
 * DeviceConditionDisplayInterface* to draw a graphical representation of the
 * condition. In this tests we don't care about it.
 */
class DeviceConditionDisplayMock : public DeviceConditionDisplayInterface
{
public:
	virtual void updateText(int min_condition_value, int max_condition_value)
	{
		Q_UNUSED(min_condition_value)
		Q_UNUSED(max_condition_value)
	}
};


TestScenEvoDevicesCond::TestScenEvoDevicesCond()
{
	server = new OpenServerMock;
	QHash<int, Client*> monitors;
	client_monitor = server->connectMonitor();
	monitors[0] = client_monitor;
	FrameReceiver::setClientsMonitor(monitors);

	QHash<int, QPair<Client*, Client*> > clients;
	clients[0].first = server->connectCommand();
	clients[0].second = server->connectRequest();
	device::setClients(clients);

	mock_display = new DeviceConditionDisplayMock;
	dev_where = "10";
}

void TestScenEvoDevicesCond::init()
{
	// We need to clear the device cache because we need to insert in the cache
	// different devices with the same key.
	bt_global::devices_cache.clear();
}

TestScenEvoDevicesCond::~TestScenEvoDevicesCond()
{
	delete server;
}

void TestScenEvoDevicesCond::checkCondition(QSignalSpy &spy, QString frame, bool satisfied)
{
	client_monitor->manageFrame(frame.toAscii());
	QCOMPARE(spy.count(), satisfied ? 1 : 0);
	spy.clear();
	char str[] = "";
	client_monitor->last_msg_open_read.CreateMsgOpen(str, 0);
}

void TestScenEvoDevicesCond::testLightOn()
{
	DeviceConditionLight cond(mock_display, "1", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testLightOff()
{
	DeviceConditionLight cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimmingOff()
{
	DeviceConditionDimming cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimmingRange1()
{
	DeviceConditionDimming cond(mock_display, "2-4", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*2*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*4*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*3*%1##").arg(dev_where), false);
}

void TestScenEvoDevicesCond::testDimmingRange2()
{
	DeviceConditionDimming cond(mock_display, "5-7", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*6*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*7*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*8*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimmingRange3()
{
	DeviceConditionDimming cond(mock_display, "8-10", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*10*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*7*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*8*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*9*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*5*%1##").arg(dev_where), false);
}

void TestScenEvoDevicesCond::testDimming100Off()
{
	DeviceConditionDimming100 cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*#1*%1*1*100*0##").arg(dev_where), true);
	checkCondition(spy, QString("*#1*%1*1*101*0##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testDimming100Range1()
{
	DeviceConditionDimming100 cond(mock_display, "1-20", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
//	checkCondition(spy, QString("*1*2*%1##").arg(dev_where), true);
	// TODO: Complete the tests of the DeviceConditionDimming100 asap!
}

