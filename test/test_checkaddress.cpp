#include "test_checkaddress.h"

#include <device.h> // checkAddressIsForMe()

#include <QtTest/QtTest>

void TestCheckAddress::testCheckAddressGlobal()
{
	QString global = "0";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12", NOT_PULL), GLOBAL);
	QCOMPARE(checkAddressIsForMe(global, "01", NOT_PULL), GLOBAL);

}

void TestCheckAddress::testCheckAddressGlobalLevel3()
{
	QString global = "0#3";
	QVERIFY(checkAddressIsForMe(global, "0312#4#12", NOT_PULL) != GLOBAL);
	QCOMPARE(checkAddressIsForMe(global, "01", NOT_PULL), GLOBAL);
}

void TestCheckAddress::testCheckAddressGlobalInterface()
{
	QString global = "0#4#12";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12", NOT_PULL), GLOBAL);
	QVERIFY(checkAddressIsForMe(global, "01", NOT_PULL) != GLOBAL);
}

void TestCheckAddress::testCheckAddressGlobalInterfaceFalse()
{
	QString global = "0#4#01";
	QVERIFY(checkAddressIsForMe(global, "0312#4#12", NOT_PULL) != GLOBAL);
	QVERIFY(checkAddressIsForMe(global, "01", NOT_PULL) != GLOBAL);
}

void TestCheckAddress::testCheckAddressEnvironment10()
{
	QString environment = "100";
	QCOMPARE(checkAddressIsForMe(environment, "1015#4#12", NOT_PULL), ENVIRONMENT);
	QCOMPARE(checkAddressIsForMe(environment, "1001", NOT_PULL), ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressEnvironment0()
{
	QString environment = "00#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "01#4#12", NOT_PULL), ENVIRONMENT);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#12", NOT_PULL), ENVIRONMENT);
	QVERIFY(checkAddressIsForMe(environment, "09", NOT_PULL) != ENVIRONMENT);
	QVERIFY(checkAddressIsForMe(environment, "0015#4#99", NOT_PULL) != ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressEnvironmentInt()
{
	QString environment = "3#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "0313#4#12", NOT_PULL), ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressEnvironmentLevel3()
{
	QString environment = "3#3";
	QCOMPARE(checkAddressIsForMe(environment, "0313", NOT_PULL), ENVIRONMENT);
	environment = "00#3";
	QCOMPARE(checkAddressIsForMe(environment, "01", NOT_PULL), ENVIRONMENT);
	environment = "100#3";
	QCOMPARE(checkAddressIsForMe(environment, "1014", NOT_PULL), ENVIRONMENT);
}

void TestCheckAddress::testCheckAddressPointToPoint()
{
	QString addr = "12";
	QCOMPARE(checkAddressIsForMe(addr, "11", NOT_PULL), NOT_MINE);
	addr = "12#4#01";
	QCOMPARE(checkAddressIsForMe(addr, "19#4#01", NOT_PULL), NOT_MINE);
	QCOMPARE(checkAddressIsForMe(addr, "12#4#02", NOT_PULL), NOT_MINE);
}

void TestCheckAddress::testCheckAddressGroup()
{
	QString group = "#45";
	QVERIFY(checkAddressIsForMe(group, "12", NOT_PULL) != ENVIRONMENT);
	QVERIFY(checkAddressIsForMe(group, "34#4#45", NOT_PULL) != ENVIRONMENT);
}

