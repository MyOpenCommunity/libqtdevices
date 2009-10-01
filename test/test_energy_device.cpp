#include "test_energy_device.h"
#include "device_tester.h"
#include "openserver_mock.h"
#include "openclient.h"

#include <energy_device.h>
#include <openmsg.h>

#include <QtTest/QtTest>
#include <QStringList>
#include <QVariant>


namespace QTest
{
	template<> char *toString(const EnergyValue &value)
	{
		QByteArray ba = "EnergyValue(";
		ba += value.first.toString() + ", " + QByteArray::number(value.second);
		ba += ")";
		return qstrdup(ba.data());
	}
}

namespace
{
	// Return the complete date (in the range [previus year/month+1, current_year/month]) for
	// the request month.
	QDate getDate(int month, int day)
	{
		QDate curr = QDate::currentDate();
		int year = curr.year();
		if (month > curr.month())
			--year;
		return QDate(year, month, day);
	}
}


void TestEnergyDevice::initTestCase()
{
	where = "20";
	dev = new EnergyDevice(where, 1);
}

void TestEnergyDevice::cleanupTestCase()
{
	delete dev;
}

void TestEnergyDevice::sendRequestCumulativeMonth()
{
	dev->requestCumulativeMonth(QDate::currentDate());
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*20*53##"));
}

void TestEnergyDevice::sendRequestCumulativeMonth2()
{
	dev->requestCumulativeMonth(QDate(2009, 1, 10));
	client_request->flush();
	QCOMPARE(server->frameRequest(), QString("*#18*20*52#9#1##"));
}

void TestEnergyDevice::sendRequestCumulativeDayGraph()
{
	dev->requestCumulativeDayGraph(QDate(2009, 1, 9));
	client_command->flush();
	QString req(QString("*18*52#1#9*%1##").arg(where));
	QCOMPARE(server->frameCommand(), req);
}

void TestEnergyDevice::sendRequestCumulativeMonthGraph()
{
	dev->requestCumulativeMonthGraph(QDate(2009, 1, 9));
	client_command->flush();
	QString req(QString("*18*56#1*%1##").arg(where));
	QCOMPARE(server->frameCommand(), req);
}

void TestEnergyDevice::sendRequestCumulativeYearGraph()
{
	dev->requestCumulativeYearGraph();
	client_request->flush();

	QStringList frames;
	frames << "*#18*20*53##";

	QDate d = QDate::currentDate();
	for (int i = 1; i < 12; ++i)
	{
		d = d.addMonths(-1);
		frames << QString("*#18*%1*52#%2#%3##").arg(where).arg(d.year() - 2000).arg(d.month());
	}
	QCOMPARE(server->frameRequest(), frames.join(""));
}

void TestEnergyDevice::sendRequestCurrent()
{
	dev->requestCurrent();
	client_request->flush();
	QString req(QString("*#18*%1*113##").arg(where));
	QCOMPARE(server->frameRequest(), req);
}

void TestEnergyDevice::sendRequestCurrent2()
{
	dev->mode = 2;
	dev->requestCurrent();
	client_request->flush();
	QString req(QString("*#18*%1*1134##").arg(where));
	QCOMPARE(server->frameRequest(), req);
}

void TestEnergyDevice::sendRequestCurrent3()
{
	dev->mode = 3;
	dev->requestCurrent();
	client_request->flush();
	QString req(QString("*#18*%1*1130##").arg(where));
	QCOMPARE(server->frameRequest(), req);
}

void TestEnergyDevice::sendRequestCurrent4()
{
	dev->mode = 4;
	dev->requestCurrent();
	client_request->flush();
	QString req(QString("*#18*%1*1132##").arg(where));
	QCOMPARE(server->frameRequest(), req);
}

void TestEnergyDevice::receiveCumulativeDay()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_DAY);
	t.check(QString("*#18*%1*54*150##").arg(where), EnergyValue(QDate::currentDate(), 150));
}

void TestEnergyDevice::receiveCumulativeDay2()
{
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_DAY);
	QString tmp(QString("*#18*%1*%2#8#1").arg(where).arg(EnergyDevice::DIM_DAY_GRAPH));
	QStringList frames;

	frames << tmp + "*1*3*255##"
		<< tmp + "*2*0*1*12##"
		<< tmp + "*3*200*150*255##"
		<< tmp + "*4*190*191*192##"
		<< tmp + "*5*180*181*182##"
		<< tmp + "*6*170*171*172##"
		<< tmp + "*7*253*254*255##"
		<< tmp + "*8*222*223*224##"
		<< tmp + "*9*253*254*2##"
		<< tmp + "*10*13*6666##";

	int year = QDate::currentDate().year();
	if (QDate::currentDate().month() < 8)
		--year;

	t.check(frames, EnergyValue(QDate(year, 8, 1), 525));
}

void TestEnergyDevice::receiveCurrent()
{
	DeviceTester t(dev, EnergyDevice::DIM_CURRENT);
	t.check(QString("*#18*%1*113*74##").arg(where), EnergyValue(QDate::currentDate(),74));
	t.check(QString("*#18*%1*1134*81##").arg(where), EnergyValue(QDate::currentDate(), 81));
	t.check(QString("*#18*%1*1130*60##").arg(where), EnergyValue(QDate::currentDate(), 60));
}

void TestEnergyDevice::receiveCumulativeMonth()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH);
	t.check(QString("*#18*%1*52#8#2*106##").arg(where), EnergyValue(QDate(2008, 2, 1), 106));
	t.check(QString("*#18*%1*53*95##").arg(where), EnergyValue(QDate::currentDate(), 95));
	t.check(QString("*#18*%1*53*4294967295##").arg(where), EnergyValue(QDate::currentDate(), 0));
	t.check(QString("*#18*%1*53*2147483647##").arg(where), EnergyValue(QDate::currentDate(), 2147483647));
}

void TestEnergyDevice::receiveCumulativeYear()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR);
	t.check(QString("*#18*%1*51*33##").arg(where), EnergyValue(QDate::currentDate(), 33));
}

void TestEnergyDevice::receiveDailyAverageGraph()
{
	int month = 9;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAILY_AVERAGE_GRAPH);
	QStringList frames;
	frames << "*#18*20*57#9*1*2*5##" << "*#18*20*57#9*2*0*9*3##";

	GraphData data;
	data.graph[1] = 9;
	data.date = getDate(month, 1);
	data.type = EnergyDevice::DAILY_AVERAGE;
	t.check(frames, data);
}

void TestEnergyDevice::receiveDailyAverageGraph2()
{
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAILY_AVERAGE_GRAPH);
	QStringList frames;
	frames << "*#18*20*57#9*1*2*5##" << "*#18*20*57#9*2*1*9*3##";
	frames << "*#18*20*57#9*3*66*1*10##" << "*#18*20*57#9*4*5*1*0##";
	frames << "*#18*20*57#9*5*14*0*2##" << "*#18*20*57#9*6*2*9*4##";
	frames << "*#18*20*57#9*7*6*3*0##" << "*#18*20*57#9*8*7*11*3##";
	frames << "*#18*20*57#9*9*2*5*1##" << "*#18*20*57#9*10*0*120*1##";
	frames << "*#18*20*57#9*11*88*1*0##" << "*#18*20*57#9*12*2*31*2##";
	frames << "*#18*20*57#9*13*32*0*65##" << "*#18*20*57#9*14*1*180*0##";
	frames << "*#18*20*57#9*15*7*3*3##" << "*#18*20*57#9*16*1*2*4##";
	frames << "*#18*20*57#9*17*1*2*4##" << "*#18*20*57#9*18*0*0*0##";
	frames << "*#18*20*57#9*19*0##";

	GraphData data;
	data.graph[1] = 265;
	data.graph[2] = 834;
	data.graph[3] = 266;
	data.graph[4] = 1281;
	data.graph[5] = 14;
	data.graph[6] = 2;
	data.graph[7] = 521;
	data.graph[8] = 1030;
	data.graph[9] = 768;
	data.graph[10] = 1803;
	data.graph[11] = 770;
	data.graph[12] = 1281;
	data.graph[13] = 120;
	data.graph[14] = 344;
	data.graph[15] = 256;
	data.graph[16] = 543;
	data.graph[17] = 544;
	data.graph[18] = 65;
	data.graph[19] = 436;
	data.graph[20] = 7;
	data.graph[21] = 771;
	data.graph[22] = 258;
	data.graph[23] = 1025;
	data.graph[24] = 516;
	data.date = getDate(9, 1);
	data.type = EnergyDevice::DAILY_AVERAGE;

	t.check(frames, data);
}

void TestEnergyDevice::receiveDayGraph()
{
	int month = 8;
	int day = 1;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAY_GRAPH);
	QString tmp(QString("*#18*%1*%2#%3#%4*1*45*210##").arg(where).arg(EnergyDevice::DIM_DAY_GRAPH).arg(month).arg(day));
	QStringList frames;
	frames << tmp;

	GraphData data;
	data.graph[1] = 210;
	data.date = getDate(month, day);
	data.type = EnergyDevice::CUMULATIVE_DAY;

	t.check(frames, data);
}

void TestEnergyDevice::receiveDayGraph2()
{
	int month = 8;
	int day = 1;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAY_GRAPH);
	QString tmp(QString("*#18*%1*%2#%3#%4").arg(where).arg(EnergyDevice::DIM_DAY_GRAPH).arg(month).arg(day));
	QStringList frames;

	frames << tmp + "*1*45*210##"
		<< tmp + "*2*12*67*255##";

	GraphData data;
	data.graph[1] = 210;
	data.graph[2] = 12;
	data.graph[3] = 67;
	data.graph[4] = 0;
	data.date = getDate(month, day);
	data.type = EnergyDevice::CUMULATIVE_DAY;

	t.check(frames, data);
}

void TestEnergyDevice::receiveDayGraph3()
{
	int month = 8;
	int day = 1;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAY_GRAPH);
	QString tmp(QString("*#18*%1*%2#%3#%4").arg(where).arg(EnergyDevice::DIM_DAY_GRAPH).arg(month).arg(day));
	QStringList frames;

	frames << tmp + "*1*3*255##"
		<< tmp + "*2*0*1*12##"
		<< tmp + "*3*200*150*255##"
		<< tmp + "*4*190*191*192##"
		<< tmp + "*5*180*181*182##"
		<< tmp + "*6*170*171*172##"
		<< tmp + "*7*253*254*255##"
		<< tmp + "*8*222*223*224##"
		<< tmp + "*9*253*254*9999##"
		<< tmp + "*10*9999*6666##";

	GraphData data;
	data.graph[1] = 0;
	data.graph[2] = 0;
	data.graph[3] = 1;
	data.graph[4] = 12;
	data.graph[5] = 200;
	data.graph[6] = 150;
	data.graph[7] = 0;
	data.graph[8] = 190;
	data.graph[9] = 191;
	data.graph[10] = 192;
	data.graph[11] = 180;
	data.graph[12] = 181;
	data.graph[13] = 182;
	data.graph[14] = 170;
	data.graph[15] = 171;
	data.graph[16] = 172;
	data.graph[17] = 253;
	data.graph[18] = 254;
	data.graph[19] = 0;
	data.graph[20] = 222;
	data.graph[21] = 223;
	data.graph[22] = 224;
	data.graph[23] = 253;
	data.graph[24] = 254;
	data.date = getDate(month, day);
	data.type = EnergyDevice::CUMULATIVE_DAY;

	t.check(frames, data);
}

void TestEnergyDevice::receiveCumulativeMonthGraph()
{
	int month = 8;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH);
	QString tmp(QString("*#18*%1*%2#%3").arg(where).arg(EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH).arg(month));
	QStringList frames;

	frames << tmp + "*1*3*255##";

	GraphData data;
	data.graph[1] = (3 << 8) + 255;
	data.date = getDate(month, 1);
	data.type = EnergyDevice::CUMULATIVE_MONTH;

	t.check(frames, data);
}

void TestEnergyDevice::receiveCumulativeMonthGraph2()
{
	int month = 8;
	// test one missing frame
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH);
	QString tmp(QString("*#18*%1*%2#%3").arg(where).arg(EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH).arg(month));
	QStringList frames;

	frames << tmp + "*1*3*255##"
		<< tmp + "*2*20*90*255##"
		<< tmp + "*3*255*0*45##"
		<< tmp + "*4*45*150*185##";


	GraphData data;
	data.graph[1] = (3 << 8) + 255;
	data.graph[2] = (20 << 8) + 90;
	data.graph[3] = 0;
	data.graph[4] = 45;
	data.graph[5] = (45 << 8) + 150;
	data.date = getDate(month, 1);
	data.type = EnergyDevice::CUMULATIVE_MONTH;

	t.check(frames, data);
}

void TestEnergyDevice::receiveCumulativeYearGraph()
{
	// We have to reset the device buffer
	dev->buffer_year_data.clear();
	for (int i = 1; i <= 12; ++i)
		dev->buffer_year_data[i] = 0;

	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR_GRAPH);
	QStringList frames;
	int month = 1;
	int invalid_month = 3;
	frames << QString("*#18*%1*52#9#%2*106##").arg(where).arg(month);
	frames << QString("*#18*%1*52#9#%2*4294967295##").arg(where).arg(invalid_month);
	frames << QString("*#18*%1*53*95##").arg(where);

	GraphData data;
	data.type = EnergyDevice::CUMULATIVE_YEAR;
	for (int i = 1; i <= 12; ++i)
		data.graph[i] = 0;

	// We can't use a fixed month because the month index depends by the currentDate.
	int month_distance = month - QDate::currentDate().month();
	int index = month_distance < 0 ? month_distance + 12 : month_distance;

	data.graph[12] = 95;
	data.graph[index] = 106;

	// Check for the month that has an invalid value
	int d = invalid_month - QDate::currentDate().month();
	int invalid_index = d < 0 ? d + 12 : d;
	data.graph[invalid_index] = 0;

	t.check(frames, data);
}

void TestEnergyDevice::receiveMonthlyAverage()
{
	DeviceTester t(dev, EnergyDevice::DIM_MONTLY_AVERAGE);

	t.check(QString("*#18*%1*52#8#2*106##").arg(where),
			EnergyValue(QDate(2008, 2, 1), qRound(1.0 * 106 / QDate(2008, 2, 1).daysInMonth())));
	t.check(QString("*#18*%1*53*95##").arg(where),
			EnergyValue(QDate::currentDate(), qRound(1.0 * 95 / QDate::currentDate().day())));
	t.check(QString("*#18*%1*53*4294967295##").arg(where),
			EnergyValue(QDate::currentDate(), 0));
}

void TestEnergyDevice::receiveCumulativeDayRequest()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR); // the dim doesn't matter
	t.checkSignals(QString("*#18*%1*54##").arg(where), 0);
}

void TestEnergyDevice::receiveCumulativeMonthRequest()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR); // the dim doesn't matter
	t.checkSignals(QString("*18*56#10*%1##").arg(where), 0);
}

void TestEnergyDevice::testGetDateFromFrame()
{
	int month = 10;
	int day = 1;
	OpenMsg frame(qPrintable(QString("*#18*20*57#%1*%2*22*33##").arg(month).arg(day)));
	QVERIFY(getDate(month, day) == dev->getDateFromFrame(frame));
}

void TestEnergyDevice::testConsecutiveGraphFrames()
{
	dev->buffer_frame.clear();
	QStringList frames;
	frames << "*#18*20*57#8*1*22*33##"
		<< "*#18*20*57#8*2*28*33*55##"
		<< "*#18*20*56#6#2*1*1*44##";
	for (int i = 0; i < frames.size(); ++i)
	{
		OpenMsg msg(qPrintable(frames[i]));
		dev->manageFrame(msg);
	}
	QCOMPARE(dev->buffer_frame.size(), 1);
}


