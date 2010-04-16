/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


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

        template<> char *toString(const GraphData &value)
        {
		QStringList gdata;
		QList<int> keys = value.graph.keys();

		qSort(keys);

		foreach (int  key, keys)
			gdata.append(QString("%1: %2").arg(key).arg(value.graph[key]));

		QByteArray ba = "GraphData(";
		ba += QString::number(value.type) + ", " + toString(value.date) + ", (" + gdata.join(", ") + ")";
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


void TestEnergyDevice::init()
{
	where = "20";
	dev = new EnergyDevice(where, 1);
	upd = &dev->current_updates;
}

void TestEnergyDevice::cleanup()
{
	delete dev;
	upd = NULL;
	dev = NULL;
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

void TestEnergyDevice::sendRequestCumulativeDayGraph8Bit()
{
	dev->requestCumulativeDayGraph8Bit(QDate(2009, 1, 9));
	client_command->flush();
	QString req(QString("*18*52#1#9*%1##").arg(where));
	QCOMPARE(server->frameCommand(), req);
}

void TestEnergyDevice::sendRequestCumulativeMonthGraph8Bit()
{
	dev->requestCumulativeMonthGraph8Bit(QDate(2009, 1, 9));
	client_command->flush();
	QString req(QString("*18*56#1*%1##").arg(where));
	QCOMPARE(server->frameCommand(), req);
}

void TestEnergyDevice::sendRequestDailyAverageGraph16Bit()
{
	dev->requestDailyAverageGraph16Bit(QDate(2009, 1, 9));
	client_command->flush();
	QString req(QString("*18*58#1*%1##").arg(where));
	QCOMPARE(server->frameCommand(), req);
}

void TestEnergyDevice::sendRequestCumulativeDayGraph16Bit()
{
	dev->requestCumulativeDayGraph16Bit(QDate(2009, 1, 9));
	client_command->flush();
	QString req(QString("*18*57#1#9*%1##").arg(where));
	QCOMPARE(server->frameCommand(), req);
}

void TestEnergyDevice::sendRequestCumulativeMonthGraph32Bit()
{
	dev->requestCumulativeMonthGraph32Bit(QDate(2009, 1, 9));
	client_command->flush();
	QString req(QString("*18*59#1*%1##").arg(where));
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
	dev->current_updates.mode = 2;
	dev->requestCurrent();
	client_request->flush();
	QString req(QString("*#18*%1*1134##").arg(where));
	QCOMPARE(server->frameRequest(), req);
}

void TestEnergyDevice::sendRequestCurrent3()
{
	dev->current_updates.mode = 3;
	dev->requestCurrent();
	client_request->flush();
	QString req(QString("*#18*%1*1130##").arg(where));
	QCOMPARE(server->frameRequest(), req);
}

void TestEnergyDevice::sendRequestCurrent4()
{
	dev->current_updates.mode = 4;
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
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_DAY, DeviceTester::MULTIPLE_VALUES);
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

	t.check(frames, EnergyValue(QDate(year, 8, 1), 52500));
}

void TestEnergyDevice::receiveCumulativeDay16Bit()
{
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_DAY, DeviceTester::MULTIPLE_VALUES);
	QString tmp(QString("*#18*%1*%2#8#1").arg(where).arg(511));
	QStringList frames;

	for (int i = 0; i < 24; ++i)
		frames << tmp + QString("*%1*%2##").arg(i + 1).arg(i * 37 + 13);
	frames << tmp + QString("*%1*%2##").arg(25).arg(17477);

	int year = QDate::currentDate().year();
	if (QDate::currentDate().month() < 8)
		--year;

	t.check(frames, EnergyValue(QDate(year, 8, 1), 17477));
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
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH, DeviceTester::MULTIPLE_VALUES);
	t.check(QString("*#18*%1*52#8#2*106##").arg(where), EnergyValue(QDate(2008, 2, 1), 106));
	t.check(QString("*#18*%1*53*95##").arg(where), EnergyValue(QDate::currentDate(), 95));
	t.check(QString("*#18*%1*53*4294967295##").arg(where), EnergyValue(QDate::currentDate(), 0));
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

	int divisor = getDate(month, 1).daysInMonth();

	GraphData data;
	data.graph[1] = 900 / divisor;
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

	int divisor = getDate(9, 1).daysInMonth();

	GraphData data;
	data.graph[1] = 26500 / divisor;
	data.graph[2] = 83400 / divisor;
	data.graph[3] = 26600 / divisor;
	data.graph[4] = 128100 / divisor;
	data.graph[5] = 1400 / divisor;
	data.graph[6] = 200 / divisor;
	data.graph[7] = 52100 / divisor;
	data.graph[8] = 103000 / divisor;
	data.graph[9] = 76800 / divisor;
	data.graph[10] = 180300 / divisor;
	data.graph[11] = 77000 / divisor;
	data.graph[12] = 128100 / divisor;
	data.graph[13] = 12000 / divisor;
	data.graph[14] = 34400 / divisor;
	data.graph[15] = 25600 / divisor;
	data.graph[16] = 54300 / divisor;
	data.graph[17] = 54400 / divisor;
	data.graph[18] = 6500 / divisor;
	data.graph[19] = 43600 / divisor;
	data.graph[20] = 700 / divisor;
	data.graph[21] = 77100 / divisor;
	data.graph[22] = 25800 / divisor;
	data.graph[23] = 102500 / divisor;
	data.graph[24] = 51600 / divisor;
	data.date = getDate(9, 1);
	data.type = EnergyDevice::DAILY_AVERAGE;

	t.check(frames, data);
}

void TestEnergyDevice::receiveDailyAverageGraph16Bit()
{
	int month = 8;
	int day = 1;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAILY_AVERAGE_GRAPH, DeviceTester::MULTIPLE_VALUES);
	QString tmp(QString("*#18*%1*%2#%3").arg(where).arg(512).arg(month));
	QStringList frames;

	GraphData data;
	for (int i = 0; i < 24; ++i)
	{
		frames << tmp + QString("*%1*%2##").arg(i + 1).arg(i * 37 + 13);
		data.graph[i + 1] = i * 37 + 13;
	}
	frames << tmp + QString("*%1*%2##").arg(25).arg(174);
	data.date = getDate(month, day);
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
	data.graph[1] = 21000;
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
	data.graph[1] = 21000;
	data.graph[2] = 1200;
	data.graph[3] = 6700;
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
	DeviceTester t(dev, EnergyDevice::DIM_DAY_GRAPH, DeviceTester::MULTIPLE_VALUES);
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
	data.graph[3] = 100;
	data.graph[4] = 1200;
	data.graph[5] = 20000;
	data.graph[6] = 15000;
	data.graph[7] = 0;
	data.graph[8] = 19000;
	data.graph[9] = 19100;
	data.graph[10] = 19200;
	data.graph[11] = 18000;
	data.graph[12] = 18100;
	data.graph[13] = 18200;
	data.graph[14] = 17000;
	data.graph[15] = 17100;
	data.graph[16] = 17200;
	data.graph[17] = 25300;
	data.graph[18] = 25400;
	data.graph[19] = 0;
	data.graph[20] = 22200;
	data.graph[21] = 22300;
	data.graph[22] = 22400;
	data.graph[23] = 25300;
	data.graph[24] = 25400;
	data.date = getDate(month, day);
	data.type = EnergyDevice::CUMULATIVE_DAY;

	t.check(frames, data);
}

void TestEnergyDevice::receiveDayGraph16Bit()
{
	int month = 8;
	int day = 1;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_DAY_GRAPH, DeviceTester::MULTIPLE_VALUES);
	QString tmp(QString("*#18*%1*%2#%3#%4").arg(where).arg(511).arg(month).arg(day));
	QStringList frames;

	GraphData data;
	for (int i = 0; i < 24; ++i)
	{
		frames << tmp + QString("*%1*%2##").arg(i + 1).arg(i * 37 + 13);
		data.graph[i + 1] = i * 37 + 13;
	}
	frames << tmp + QString("*%1*%2##").arg(25).arg(17477);
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
	data.graph[1] = 100 * ((3 << 8) + 255);
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
	data.graph[1] = 100 * ((3 << 8) + 255);
	data.graph[2] = 100 * ((20 << 8) + 90);
	data.graph[3] = 0;
	data.graph[4] = 4500;
	data.graph[5] = 100 * ((45 << 8) + 150);
	data.date = getDate(month, 1);
	data.type = EnergyDevice::CUMULATIVE_MONTH;

	t.check(frames, data);
}

void TestEnergyDevice::receiveCumulativeMonthGraph3()
{
	int month = 2;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH);
	QString tmp(QString("*#18*%1*%2#%3").arg(where).arg(EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH).arg(month));
	QStringList frames;

	for (int i = 1; i <= 22; ++i)
		frames << tmp + QString("*%1*0*0*0##").arg(i);

	QVariant result = t.getResult(frames);
	QVERIFY(result.canConvert<GraphData>());
	GraphData d = result.value<GraphData>();
	QVERIFY(d.graph.size() == QDate(QDate::currentDate().year(), month, 1).daysInMonth());
}

void TestEnergyDevice::receiveCumulativeMonthGraph32Bit()
{
	int month = 8;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH);
	QString tmp(QString("*#18*%1*%2#%3").arg(where).arg(513).arg(month));
	QStringList frames;

	frames << tmp + "*1*1778##";

	GraphData data;
	data.graph[1] = 1778;
	data.date = getDate(month, 1);
	data.type = EnergyDevice::CUMULATIVE_MONTH;

	t.check(frames, data);
}

void TestEnergyDevice::receiveCumulativeMonthGraph32Bit2()
{
	int month = 2;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH);
	QString tmp(QString("*#18*%1*%2#%3").arg(where).arg(513).arg(month));
	QStringList frames;

	GraphData data;
	data.date = getDate(month, 1);
	data.type = EnergyDevice::CUMULATIVE_MONTH;

	for (int i = 0; i < 31; ++i)
	{
		frames << tmp + QString("*%1*%2##").arg(i + 1).arg(i * 17);
		if (i < data.date.daysInMonth())
			data.graph[i + 1] = i * 17;
	}

	t.check(frames, data);
}

void TestEnergyDevice::receiveCumulativeMonthGraphPrevYear32Bit()
{
	int month = 8;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_MONTH_GRAPH);
	QString tmp(QString("*#18*%1*%2#%3").arg(where).arg(514).arg(month));
	QStringList frames;
	QDate d = getDate(month, 1);

	frames << tmp + "*1*17765##";

	GraphData data;
	data.graph[1] = 17765;
	data.date = QDate(d.year() - 1, d.month(), d.day());
	data.type = EnergyDevice::CUMULATIVE_MONTH;

	t.check(frames, data);
}

void TestEnergyDevice::receiveCumulativeYearGraph()
{
	// We have to reset the device buffer
	dev->buffer_year_data.clear();
	for (int i = 1; i <= 12; ++i)
		dev->buffer_year_data[i] = 0;

	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR_GRAPH, DeviceTester::MULTIPLE_VALUES);
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

	data.graph[12] = 9500;
	data.graph[index] = 10600;

	// Check for the month that has an invalid value
	int d = invalid_month - QDate::currentDate().month();
	int invalid_index = d < 0 ? d + 12 : d;
	data.graph[invalid_index] = 0;

	t.check(frames, data);
}

void TestEnergyDevice::receiveMonthlyAverage()
{
	DeviceTester t(dev, EnergyDevice::DIM_MONTLY_AVERAGE, DeviceTester::MULTIPLE_VALUES);

	t.check(QString("*#18*%1*52#8#2*106##").arg(where),
			EnergyValue(QDate(2008, 2, 1), qRound(1.0 * 106 / QDate(2008, 2, 1).daysInMonth())));
	t.check(QString("*#18*%1*53*95##").arg(where),
			EnergyValue(QDate::currentDate(), qRound(1.0 * 95 / QDate::currentDate().day())));
	t.check(QString("*#18*%1*53*4294967295##").arg(where),
			EnergyValue(QDate::currentDate(), 0));
}

void TestEnergyDevice::receiveMonthlyAverage16Bit()
{
	int month = 8;
	dev->buffer_frame.clear();
	DeviceTester t(dev, EnergyDevice::DIM_MONTLY_AVERAGE, DeviceTester::MULTIPLE_VALUES);
	QString tmp(QString("*#18*%1*%2#%3").arg(where).arg(512).arg(month));
	QStringList frames;

	GraphData data;
	for (int i = 0; i < 24; ++i)
	{
		frames << tmp + QString("*%1*%2##").arg(i + 1).arg(i * 37 + 13);
		data.graph[i + 1] = i * 37 + 13;
	}
	frames << tmp + QString("*%1*%2##").arg(25).arg(174);

	t.check(frames, EnergyValue(getDate(8, 1), 174));
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


// test switch from 8 bit graph frames to 16/32 bit graph frames

void TestEnergyDevice::receiveInvalidFrameRequestDailyAverageGraph16Bit()
{
	DeviceTester t(dev, EnergyDevice::DIM_MONTLY_AVERAGE); // dim is not used

	dev->requestDailyAverageGraph(QDate(2009, 9, 1));
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*18*53#9*%1##").arg(where));

	t.checkSignals(QString("*#18*%1*777##").arg(where), 0);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*18*58#9*%1##").arg(where));
}

void TestEnergyDevice::receiveInvalidFrameRequestCumulativeDayGraph16Bit()
{
	DeviceTester t(dev, EnergyDevice::DIM_MONTLY_AVERAGE); // dim is not used

	dev->requestCumulativeDayGraph(QDate(2009, 9, 1));
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*18*52#9#1*%1##").arg(where));

	t.checkSignals(QString("*#18*%1*777##").arg(where), 0);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*18*57#9#1*%1##").arg(where));
}

void TestEnergyDevice::receiveInvalidFrameRequestCumulativeMonthGraph32Bit()
{
	DeviceTester t(dev, EnergyDevice::DIM_MONTLY_AVERAGE); // dim is not used

	dev->requestCumulativeMonthGraph(QDate(2009, 9, 1));
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*18*56#9*%1##").arg(where));

	t.checkSignals(QString("*#18*%1*777##").arg(where), 0);
	client_command->flush();

	QCOMPARE(server->frameCommand(), QString("*18*59#9*%1##").arg(where));
}


// test automatic updates

void TestEnergyDevice::sendUpdateStart()
{
	upd->sendUpdateStart();
	client_command->flush();

	QString req(QString("*#18*%1*#1200#%2*255##").arg(where).arg(upd->mode));
	QCOMPARE(server->frameCommand(), req);
}

void TestEnergyDevice::sendUpdateStop()
{
	upd->sendUpdateStop();
	client_command->flush();

	QString req(QString("*#18*%1*#1200#%2*0##").arg(where).arg(upd->mode));
	QCOMPARE(server->frameCommand(), req);
}

void TestEnergyDevice::receiveUpdateInterval()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR); // the dim doesn't matter

	t.checkSignals(QString("*#18*%1*1200#%2*2##").arg(where).arg(upd->mode), 0);
	QCOMPARE(dev->has_new_frames, true);
	QCOMPARE(upd->has_new_frames, true);
	QCOMPARE(upd->update_timer, (QTimer *)NULL);
}

void TestEnergyDevice::receiveUpdateStop()
{
	DeviceTester t(dev, EnergyDevice::DIM_CUMULATIVE_YEAR); // the dim doesn't matter

	t.checkSignals(QString("*#18*%1*1200#%2*0##").arg(where).arg(upd->mode), 0);
	QCOMPARE(dev->has_new_frames, true);
	QCOMPARE(upd->has_new_frames, true);
	QCOMPARE(upd->update_timer, (QTimer *)NULL);
}

void TestEnergyDevice::testUpdateStartPolling()
{
	upd->requestCurrentUpdateStart();
	client_command->flush();
	client_request->flush();

	QString reqr = QString("*#18*%1*113##").arg(where);
	QString reqc = QString("*#18*%1*#1200#%2*255##").arg(where).arg(upd->mode);
	QCOMPARE(server->frameCommand(), reqc);
	QCOMPARE(server->frameRequest(), reqr);
	QCOMPARE(upd->update_timer->isActive(), true);
	QCOMPARE(upd->update_count, 1);
	QCOMPARE(upd->update_state, AutomaticUpdates::UPDATE_AUTO);

	upd->requestCurrentUpdateStart();
	QCOMPARE(upd->update_count, 2);
}

void TestEnergyDevice::testUpdateStartAutomatic()
{
	upd->setHasNewFrames();
	upd->requestCurrentUpdateStart();
	client_command->flush();

	QString req = QString("*#18*%1*#1200#%2*255##").arg(where).arg(upd->mode);
	QCOMPARE(server->frameCommand(), req);
	QCOMPARE(upd->update_count, 1);
	QCOMPARE(upd->update_state, AutomaticUpdates::UPDATE_AUTO);

	upd->requestCurrentUpdateStart();
	QCOMPARE(upd->update_count, 2);
}

void TestEnergyDevice::testUpdateStop()
{
	upd->update_count = 1;
	upd->update_state = AutomaticUpdates::UPDATE_AUTO;
	upd->requestCurrentUpdateStop();

	QCOMPARE(upd->update_state, AutomaticUpdates::UPDATE_STOPPING);

	// avoid waiting for the timer to expire
	upd->stoppingTimeout();
	upd->stoppingTimeout();

	client_command->flush();

	QString req = QString("*#18*%1*#1200#%2*0##").arg(where).arg(upd->mode);
	QCOMPARE(server->frameCommand(), req);
	QCOMPARE(upd->update_count, 0);
	QCOMPARE(upd->update_state, AutomaticUpdates::UPDATE_IDLE);

	upd->requestCurrentUpdateStop();
	upd->stoppingTimeout();
	QCOMPARE(upd->update_count, 0);
}

// TODO energy tests:
// - requestCurrentUpdateStart
// - requestCurrentUpdateStop
// - frame parsing
//
// combined width
// - polling mode
// - auto-update mode
//
// combined with
// - the three states for update state
//
// combined with
// - update_count == 0, update_count > 0
