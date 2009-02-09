#include "test_energy_device.h"
#include "device_tester.h"
#include "openserver_mock.h"
#include "openclient.h"

#include <energy_device.h>

#include <QtTest/QtTest>
#include <QStringList>
#include <QVariant>


void TestEnergyDevice::initTestCase()
{
	where = "20";
	dev = new EnergyDevice(where);
}

void TestEnergyDevice::cleanupTestCase()
{
	delete dev;
}

void TestEnergyDevice::readCumulativeDay()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_DAY);
	t.check(QString("*#18*%1*54*150##").arg(where), 150);
}

void TestEnergyDevice::readCurrent()
{
	DeviceTester t(dev, EnergyDevice::DIM_CURRENT);
	t.check(QString("*#18*%1*113*74##").arg(where), 74);
}

void TestEnergyDevice::readCumulativeMonth()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH);
	t.check(QString("*#18*%1*52#8#2*106##").arg(where), 106);
	t.check(QString("*#18*%1*53*95##").arg(where), 95);
}

void TestEnergyDevice::readCumulativeYear()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR);
	t.check(QString("*#18*%1*51*33##").arg(where), 33);
}

void TestEnergyDevice::readDailyAverageGraph()
{
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAILY_AVERAGE);
	QStringList frames("*#18*20*57#9*1*2*5##");
	QVariant result = t.getResult(frames);

	QVERIFY(result.canConvert<GraphData>());

	GraphData data;
	data[1] = 517;

	QVERIFY(data == result.value<GraphData>());
}

void TestEnergyDevice::readDailyAverageGraph2()
{
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAILY_AVERAGE);
	QStringList frames;
	frames << "*#18*20*57#9*1*2*5##" << "*#18*20*57#9*2*1*9*3##";
	QVariant result = t.getResult(frames);

	QVERIFY(result.canConvert<GraphData>());

	GraphData data;
	data[1] = 517;
	data[2] = 265;

	QVERIFY(data == result.value<GraphData>());
}

void TestEnergyDevice::readDailyAverageGraph3()
{
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAILY_AVERAGE);
	QStringList frames;
	frames << "*#18*20*57#9*1*2*5##" << "*#18*20*57#9*2*1*9*3##";
	frames << "*#18*20*57#9*3*66*1*10##" << "*#18*20*57#9*4*5*1*0##";
	frames << "*#18*20*57#9*5*14*0*2##" << "*#18*20*57#9*6*2*9*4##";
	frames << "*#18*20*57#9*7*6*3*0##" << "*#18*20*57#9*8*7*11*3##";
	frames << "*#18*20*57#9*9*2*5*1##" << "*#18*20*57#9*10*0*120*1##";
	frames << "*#18*20*57#9*11*88*1*0##" << "*#18*20*57#9*12*2*31*2##";
	frames << "*#18*20*57#9*13*32*0*65##" << "*#18*20*57#9*14*1*180*0##";
	frames << "*#18*20*57#9*15*7*3*3##" << "*#18*20*57#9*16*1*2*4##";
	frames << "*#18*20*57#9*17*1*0*0##" << "*#18*20*57#9*18*0*0*0##";
	frames << "*#18*20*57#9*19*0##";

	QVariant result = t.getResult(frames);
	QVERIFY(result.canConvert<GraphData>());

	GraphData data;
	data[1] = 517;
	data[2] = 265;
	data[3] = 834;
	data[4] = 266;
	data[5] = 1281;
	data[6] = 14;
	data[7] = 2;
	data[8] = 521;
	data[9] = 1030;
	data[10] = 768;
	data[11] = 1803;
	data[12] = 770;
	data[13] = 1281;
	data[14] = 120;
	data[15] = 344;
	data[16] = 256;
	data[17] = 543;
	data[18] = 544;
	data[19] = 65;
	data[20] = 436;
	data[21] = 7;
	data[22] = 771;
	data[23] = 258;
	data[24] = 1025;

	QVERIFY(data == result.value<GraphData>());
}

void TestEnergyDevice::requestCumulativeMonth()
{
	dev->requestCumulativeMonth(QDate::currentDate());
	client_request->flush();
	QVERIFY(server->frameRequest() == "*#18*20*53##");
}

void TestEnergyDevice::requestCumulativeMonth2()
{
	dev->requestCumulativeMonth(QDate(2009, 1, 10));
	client_request->flush();
	QVERIFY(server->frameRequest() == "*#18*20*52#9#1##");
}

