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

void TestScenEvoDevicesCond::testInternalTemperature1()
{
	DeviceConditionTemperature cond(mock_display, "0000", dev_where, false);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*0*0000##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*0*0100##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0010##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testInternalTemperature2()
{
	DeviceConditionTemperature cond(mock_display, "0235", dev_where, false);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*0*0000##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0225##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*0*0224##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0245##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*0*0246##").arg(dev_where), false);
}

void TestScenEvoDevicesCond::testInternalTemperature3()
{
	DeviceConditionTemperature cond(mock_display, "1010", dev_where, false);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*0*0400##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*1010##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*0*0001##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*0*0000##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testInternalTemperatureMinMax()
{
	// A temperature condition should be in the range [-0.5, + 50] Celsius.

	// Check the min condition temperature
	DeviceConditionTemperature cond_out_min(mock_display, "1100", dev_where, false);
	QSignalSpy spy_min(&cond_out_min, SIGNAL(condSatisfied()));

	checkCondition(spy_min, QString("*#4*%1*0*1050##").arg(dev_where), true);
	checkCondition(spy_min, QString("*#4*%1*0*1039##").arg(dev_where), false);
	checkCondition(spy_min, QString("*#4*%1*0*1040##").arg(dev_where), true);

	// Check the max condition temperature
	DeviceConditionTemperature cond_out_max(mock_display, "0700", dev_where);
	QSignalSpy spy_max(&cond_out_max, SIGNAL(condSatisfied()));

	checkCondition(spy_max, QString("*#4*%1*0*0499##").arg(dev_where), true);
	checkCondition(spy_max, QString("*#4*%1*0*0489##").arg(dev_where), false);
	checkCondition(spy_max, QString("*#4*%1*0*0500##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testExternalTemperature1()
{
	DeviceConditionTemperature cond(mock_display, "0000", dev_where, true);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*15*1*0000*1111##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*15*1*0100*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0010*1111##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testExternalTemperature2()
{
	DeviceConditionTemperature cond(mock_display, "0235", dev_where, true);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*15*1*0000*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0225*1111##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*15*1*0224*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0245*1111##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*15*1*0246*1111##").arg(dev_where), false);
}

void TestScenEvoDevicesCond::testExternalTemperature3()
{
	DeviceConditionTemperature cond(mock_display, "1010", dev_where, true);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));

	checkCondition(spy, QString("*#4*%1*15*1*0400*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*1010*1111##").arg(dev_where), true);
	checkCondition(spy, QString("*#4*%1*15*1*0001*1111##").arg(dev_where), false);
	checkCondition(spy, QString("*#4*%1*15*1*0000*1111##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testExternalTemperatureMinMax()
{
	// A temperature condition should be in the range [-0.5, + 50] Celsius.

	// Check the min condition temperature
	DeviceConditionTemperature cond_out_min(mock_display, "1100", dev_where, true);
	QSignalSpy spy_min(&cond_out_min, SIGNAL(condSatisfied()));

	checkCondition(spy_min, QString("*#4*%1*15*1*1050*1111##").arg(dev_where), true);
	checkCondition(spy_min, QString("*#4*%1*15*1*1039*1111##").arg(dev_where), false);
	checkCondition(spy_min, QString("*#4*%1*15*1*1040*1111##").arg(dev_where), true);

	// Check the max condition temperature
	DeviceConditionTemperature cond_out_max(mock_display, "0700", dev_where);
	QSignalSpy spy_max(&cond_out_max, SIGNAL(condSatisfied()));

	checkCondition(spy_max, QString("*#4*%1*15*1*0499*1111##").arg(dev_where), true);
	checkCondition(spy_max, QString("*#4*%1*15*1*0489*1111##").arg(dev_where), false);
	checkCondition(spy_max, QString("*#4*%1*15*1*0500*1111##").arg(dev_where), true);
}

void TestScenEvoDevicesCond::testAux()
{
	// TODO: verificare quale deve essere il comportamento desiderato! Per come
	// e' implementato il device la prima frame viene scartata.. e il condSatisfied
	// sulla seconda frame viene emesso solo se la prima frame non rispetta la
	// condizione. Questo sembra contrario alla logica delle altre condizioni
	// su device (per le quali il condSatisfied viene emesso subito), ma sembra
	// anche fatto di proposito (vedi ticket #77 trac develer),
	DeviceConditionAux cond(mock_display, "0", dev_where);
	QSignalSpy spy(&cond, SIGNAL(condSatisfied()));
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), true);
	checkCondition(spy, QString("*9*1*%1##").arg(dev_where), false);
	checkCondition(spy, QString("*9*0*%1##").arg(dev_where), true);
}
