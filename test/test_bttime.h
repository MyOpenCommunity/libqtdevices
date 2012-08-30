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


#ifndef TEST_BTTIME_H
#define TEST_BTTIME_H

#include <QtTest/QTest>

class BtTime;

namespace QTest
{
	template<> char *toString(const BtTime &t);
}

class TestBtTime : public QObject
{
Q_OBJECT

private slots:
	void testDefaultTime();
	void testSubtractOneMinute();
	void testSubtractLessThanMaxMinutes();
	void testSubtractLessThanMaxMinutes2();
	void testAddOneMinute();
	void testAddLessThanMaxMinutes();
	void testAddMoreThanMaxMinutes();
	void testAddMinutesWrapHour();

	void testAdd1Hour();
	void testAdd24Hours();
	void testAdd25Hours();
	void testAdd49Hours();
	void testSub1Hour();
	void testSub24Hours();
	void testSub25Hours();
	void testSub49Hours();

	void testAdd1Minute();
	void testAdd59Minutes();
	void testAdd60Minutes();
	void testAdd120Minutes();
	void testSub1Minute();
	void testSub59Minutes();
	void testSub60Minutes();
	void testSub120Minutes();

	void testAdd1Second();
	void testAdd59Seconds();
	void testAdd60Seconds();
	void testAdd61Seconds();
	void testAdd120Seconds();
	void testAdd3599Seconds();
	void testAdd3600Seconds();
	void testAdd7200Seconds();
	void testSub1Second();
	void testSub59Seconds();
	void testSub60Seconds();
	void testSub61Seconds();
	void testSub120Seconds();
	void testSub3599Seconds();
	void testSub3600Seconds();
	void testSub7200Seconds();

private:
	void testHourWithDelta(int delta, int h0, int h24);
	void testMinuteWithDelta(int delta, int h0, int m0, int h24, int m24);
	void testSecondWithDelta(int delta, int h0, int m0, int s0, int h24, int m24, int s24);
};

#endif // TEST_BTTIME_H
