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
	QVERIFY(checkAddressIsForMe(global, "0312#4#12") != GLOBAL);
	QCOMPARE(checkAddressIsForMe(global, "01"), GLOBAL);
}

void TestCheckAddress::testCheckAddressGlobalInterface()
{
	QString global = "0#4#12";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), GLOBAL);
	QVERIFY(checkAddressIsForMe(global, "01") != GLOBAL);
}

void TestCheckAddress::testCheckAddressGlobalInterfaceFalse()
{
	QString global = "0#4#01";
	QVERIFY(checkAddressIsForMe(global, "0312#4#12") != GLOBAL);
	QVERIFY(checkAddressIsForMe(global, "01") != GLOBAL);
}

void TestCheckAddress::testCheckAddressEnvironment10()
{
	QString environment = "100";
	QCOMPARE(checkAddressIsForMe(environment, "1015#4#12"), ENVIRONMENT);
	QCOMPARE(checkAddressIsForMe(environment, "1001"), ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressEnvironment0()
{
	QString environment = "00#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "01#4#12"), ENVIRONMENT);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#12"), ENVIRONMENT);
	QVERIFY(checkAddressIsForMe(environment, "09") != ENVIRONMENT);
	QVERIFY(checkAddressIsForMe(environment, "0015#4#99") != ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressEnvironmentInt()
{
	QString environment = "3#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "0313#4#12"), ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressEnvironmentLevel3()
{
	QString environment = "3#3";
	QCOMPARE(checkAddressIsForMe(environment, "0313"), ENVIRONMENT);
	environment = "00#3";
	QCOMPARE(checkAddressIsForMe(environment, "01"), ENVIRONMENT);
	environment = "100#3";
	QCOMPARE(checkAddressIsForMe(environment, "1014"), ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressPointToPoint()
{
	QString addr = "12";
	QCOMPARE(checkAddressIsForMe(addr, "11"), NOT_MINE);
	addr = "12#4#01";
	QCOMPARE(checkAddressIsForMe(addr, "19#4#01"), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(addr, "12#4#02"), NOT_MINE);
}

void TestCheckAddress::testCheckAddressGroup()
{
	QString group = "#45";
	QVERIFY(checkAddressIsForMe(group, "12") != ENVIRONMENT);
	QVERIFY(checkAddressIsForMe(group, "34#4#45") != ENVIRONMENT);
}

