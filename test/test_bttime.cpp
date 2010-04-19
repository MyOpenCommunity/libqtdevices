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

namespace QTest {
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
