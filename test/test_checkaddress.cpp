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


#include "test_checkaddress.h"

#include <pulldevice.h> // checkAddressIsForMe()

#include <QtTest/QtTest>

void TestCheckAddress::testCheckAddressGlobal()
{
	QString global = "0";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), GLOBAL);
	QCOMPARE(checkAddressIsForMe(global, "01"), GLOBAL);

}

void TestCheckAddress::testCheckAddressGlobalLevel3()
{
	QString global = "0#3";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(global, "01"), GLOBAL);
}

void TestCheckAddress::testCheckAddressGlobalLevel4()
{
	QString global = "0#4#06";
	QCOMPARE(checkAddressIsForMe(global, "31"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(global, "59#4#06"), GLOBAL);
}

void TestCheckAddress::testCheckAddressGlobalInterface()
{
	QString global = "0#4#12";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), GLOBAL);
	QCOMPARE(checkAddressIsForMe(global, "01"), NOT_MINE);
}

void TestCheckAddress::testCheckAddressGlobalInterfaceFalse()
{
	QString global = "0#4#01";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(global, "01"), NOT_MINE);
}

void TestCheckAddress::testCheckAddressEnvironment10()
{
	QString environment = "100";
	QCOMPARE(checkAddressIsForMe(environment, "1015#4#12"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(environment, "1001"), ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressEnvironment0()
{
	QString environment = "00#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "01#4#12"), ENVIRONMENT);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#12"), ENVIRONMENT);
	QCOMPARE(checkAddressIsForMe(environment, "09"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#99"), NOT_MINE);
}

void TestCheckAddress::testCheckAddressEnvironmentLevel4()
{
	QString environment = "3#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "0313#4#12"), ENVIRONMENT);
	QCOMPARE(checkAddressIsForMe(environment, "31"), NOT_MINE);
}

void TestCheckAddress::testCheckAddressEnvironmentLevel3()
{
	QString environment = "3#3";
	QCOMPARE(checkAddressIsForMe(environment, "0313"), ENVIRONMENT);
	environment = "00#3";
	QCOMPARE(checkAddressIsForMe(environment, "01"), ENVIRONMENT);
	environment = "100#3";
	QCOMPARE(checkAddressIsForMe(environment, "1014"), ENVIRONMENT);
	environment = "3";
	QCOMPARE(checkAddressIsForMe(environment, "0313#4#12"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(environment, "31"), ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressPointToPoint()
{
	QString addr = "12";
	QCOMPARE(checkAddressIsForMe(addr, "11"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(addr, "12"), P2P);
	addr = "12#4#01";
	QCOMPARE(checkAddressIsForMe(addr, "19#4#01"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(addr, "12#4#02"), NOT_MINE);
}

void TestCheckAddress::testCheckAddressGroup()
{
	QString group = "#45";
	QCOMPARE(checkAddressIsForMe(group, "12"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(group, "34#4#45"), NOT_MINE);
}

