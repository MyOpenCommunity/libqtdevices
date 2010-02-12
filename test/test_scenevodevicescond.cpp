#include "test_scenevodevicescond.h"
#include "scenevodevicescond.h"
#include "openserver_mock.h"
#include "openclient.h"

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QHash>
#include <QPair>
#include <QString>


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
}



void TestScenEvoDevicesCond::testLightOn()
{
	QString dev_where = "10";

	DeviceConditionLight *cond = new DeviceConditionLight(mock_display, "1", dev_where);
	QSignalSpy spy(cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testLightOff()
{
	QString dev_where = "10";

	DeviceConditionLight *cond = new DeviceConditionLight(mock_display, "0", dev_where);
	QSignalSpy spy(cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*1*0*%1##").arg(dev_where), true);
}

