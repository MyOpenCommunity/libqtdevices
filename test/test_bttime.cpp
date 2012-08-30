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


#include "test_bttime.h"
#include "bttime.h"

#include <QDebug>

namespace QTest
{
	template<> char *toString(const BtTime &t)
	{
		QByteArray ba = "BtTime(";
		ba += QByteArray::number(t.hour()) + ", " + QByteArray::number(t.minute()) + ", " + QByteArray::number(t.second());
		ba += ")";
		return qstrdup(ba.data());
	}
}


void TestBtTime::testDefaultTime()
{
	BtTime time(0, 59, 59);
	time = time.addSecond(1);
	BtTime comp(1, 0, 0);
	QCOMPARE(time, comp);

	time = BtTime(23, 59, 0);
	time = time.addMinute(1);
	comp = BtTime(0, 0, 0);
	QCOMPARE(time, comp);
}

void TestBtTime::testAddOneMinute()
{
	BtTime time(1, 59, 0);
	time = time.addMinute(1);
	BtTime comp(2, 0, 0);
	QCOMPARE(time, comp);
}

void TestBtTime::testAddLessThanMaxMinutes()
{
	BtTime time(0, 58, 0);
	time = time.addMinute(2);
	BtTime comp(1, 0, 0);
	QCOMPARE(time, comp);
}

void TestBtTime::testAddMoreThanMaxMinutes()
{
	BtTime time(0, 1, 0);
	time = time.addMinute(60);
	BtTime comp(1, 1, 0);
	QCOMPARE(time, comp);
}

void TestBtTime::testSubtractOneMinute()
{
	BtTime time(1, 0, 0);
	time = time.addMinute(-1);
	BtTime comp(0, 59, 0);
	QCOMPARE(time, comp);
}

void TestBtTime::testSubtractLessThanMaxMinutes()
{
	BtTime time(1, 0, 0);
	time = time.addMinute(-3);
	BtTime comp(0, 57, 0);
	QCOMPARE(time, comp);
}

void TestBtTime::testSubtractLessThanMaxMinutes2()
{
	BtTime time(0, 56, 0);
	time = time.addMinute(-6);
	BtTime comp(0, 50, 0);
	QCOMPARE(time, comp);
}

void TestBtTime::testAddMinutesWrapHour()
{
	BtTime time(22, 59, 0);
	time = time.addMinute(62);
	BtTime comp(0, 1, 0);
	QCOMPARE(time, comp);
}

void TestBtTime::testAdd1Hour()
{
	testHourWithDelta(1, 1, 0);
}

void TestBtTime::testAdd24Hours()
{
	testHourWithDelta(24, 24, 23);
}

void TestBtTime::testAdd25Hours()
{
	testHourWithDelta(25, 0, 24);
}

void TestBtTime::testAdd49Hours()
{
	testHourWithDelta(49, 24, 23);
}

void TestBtTime::testSub1Hour()
{
	testHourWithDelta(-1, 24, 23);
}

void TestBtTime::testSub24Hours()
{
	testHourWithDelta(-24, 1, 0);
}

void TestBtTime::testSub25Hours()
{
	testHourWithDelta(-25, 0, 24);
}

void TestBtTime::testSub49Hours()
{
	testHourWithDelta(-49, 1, 0);
}

void TestBtTime::testAdd1Minute()
{
	testMinuteWithDelta(1, 0, 1, 0, 0);
}

void TestBtTime::testAdd59Minutes()
{
	testMinuteWithDelta(59, 0, 59, 0, 58);
}

void TestBtTime::testAdd60Minutes()
{
	testMinuteWithDelta(60, 1, 0, 0, 59);
}

void TestBtTime::testAdd120Minutes()
{
	testMinuteWithDelta(120, 2, 0, 1, 59);
}

void TestBtTime::testSub1Minute()
{
	testMinuteWithDelta(-1, 24, 59, 24, 58);
}

void TestBtTime::testSub59Minutes()
{
	testMinuteWithDelta(-59, 24, 1, 24, 0);
}

void TestBtTime::testSub60Minutes()
{
	testMinuteWithDelta(-60, 24, 0, 23, 59);
}

void TestBtTime::testSub120Minutes()
{
	testMinuteWithDelta(-120, 23, 0, 22, 59);
}

void TestBtTime::testAdd1Second()
{
	testSecondWithDelta(1, 0, 0, 1, 0, 0, 0);
}

void TestBtTime::testAdd59Seconds()
{
	testSecondWithDelta(59, 0, 0, 59, 0, 0, 58);
}

void TestBtTime::testAdd60Seconds()
{
	testSecondWithDelta(60, 0, 1, 0, 0, 0, 59);
}

void TestBtTime::testAdd61Seconds()
{
	testSecondWithDelta(61, 0, 1, 1, 0, 1, 0);
}

void TestBtTime::testAdd120Seconds()
{
	testSecondWithDelta(120, 0, 2, 0, 0, 1, 59);
}

void TestBtTime::testAdd3599Seconds()
{
	testSecondWithDelta(3599, 0, 59, 59, 0, 59, 58);
}

void TestBtTime::testAdd3600Seconds()
{
	testSecondWithDelta(3600, 1, 0, 0, 0, 59, 59);
}

void TestBtTime::testAdd7200Seconds()
{
	testSecondWithDelta(7200, 2, 0, 0, 1, 59, 59);
}

void TestBtTime::testSub1Second()
{
	testSecondWithDelta(-1, 24, 59, 59, 24, 59, 58);
}

void TestBtTime::testSub59Seconds()
{
	testSecondWithDelta(-59, 24, 59, 1, 24, 59, 0);
}

void TestBtTime::testSub60Seconds()
{
	testSecondWithDelta(-60, 24, 59, 0, 24, 58, 59);
}

void TestBtTime::testSub61Seconds()
{
	testSecondWithDelta(-61, 24, 58, 59, 24, 58, 58);
}

void TestBtTime::testSub120Seconds()
{
	testSecondWithDelta(-120, 24, 58, 0, 24, 57, 59);
}

void TestBtTime::testSub3599Seconds()
{
	testSecondWithDelta(-3599, 24, 0, 1, 24, 0, 0);
}

void TestBtTime::testSub3600Seconds()
{
	testSecondWithDelta(-3600, 24, 0, 0, 23, 59, 59);
}

void TestBtTime::testSub7200Seconds()
{
	testSecondWithDelta(-7200, 23, 0, 0, 22, 59, 59);
}

void TestBtTime::testHourWithDelta(int delta, int h0, int h24)
{
	BtTime t0(0, 0, 0), t24(24, 59, 59);
	t0.setMaxHours(25);
	t24.setMaxHours(25);
	BtTime t;
	t = t0.addHour(delta);
	QCOMPARE(t, BtTime(h0, 0, 0));
	t = t24.addHour(delta);
	QCOMPARE(t, BtTime(h24, 59, 59));
}

void TestBtTime::testMinuteWithDelta(int delta, int h0, int m0, int h24, int m24)
{
	BtTime t0(0, 0, 0), t24(24, 59, 59);
	t0.setMaxHours(25);
	t24.setMaxHours(25);
	BtTime t;
	t = t0.addMinute(delta);
	QCOMPARE(t, BtTime(h0, m0, 0));
	t = t24.addMinute(delta);
	QCOMPARE(t, BtTime(h24, m24, 59));
}

void TestBtTime::testSecondWithDelta(int delta, int h0, int m0, int s0, int h24, int m24, int s24)
{
	BtTime t0(0, 0, 0), t24(24, 59, 59);
	t0.setMaxHours(25);
	t24.setMaxHours(25);
	BtTime t;
	t = t0.addSecond(delta);
	QCOMPARE(t, BtTime(h0, m0, s0));
	t = t24.addSecond(delta);
	QCOMPARE(t, BtTime(h24, m24, s24));
}
