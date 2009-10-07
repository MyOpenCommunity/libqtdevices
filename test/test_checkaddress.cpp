#include "test_checkaddress.h"

#include <generic_functions.h>

#include <QtTest/QtTest>

void TestCheckAddress::testCheckAddressGlobal()
{
	QString global = "0";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), true);
	QCOMPARE(checkAddressIsForMe(global, "01"), true);

}

void TestCheckAddress::testCheckAddressGlobalLevel3()
{
	QString global = "0#3";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), false);
	QCOMPARE(checkAddressIsForMe(global, "01"), true);
}

void TestCheckAddress::testCheckAddressGlobalInterface()
{
	QString global = "0#4#12";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), true);
	QCOMPARE(checkAddressIsForMe(global, "01"), false);
}

void TestCheckAddress::testCheckAddressGlobalInterfaceFalse()
{
	QString global = "0#4#01";
	QCOMPARE(checkAddressIsForMe(global, "0312#4#12"), false);
	QCOMPARE(checkAddressIsForMe(global, "01"), false);
}

void TestCheckAddress::testCheckAddressEnvironment10()
{
	QString environment = "100";
	QCOMPARE(checkAddressIsForMe(environment, "1015#4#12"), true);
	QCOMPARE(checkAddressIsForMe(environment, "1001"), true);
}

void TestCheckAddress::testCheckAddressEnvironment0()
{
	QString environment = "00#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "01#4#12"), true);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#12"), true);
	QCOMPARE(checkAddressIsForMe(environment, "09"), false);
	QCOMPARE(checkAddressIsForMe(environment, "0015#4#99"), false);
}

void TestCheckAddress::testCheckAddressEnvironmentInt()
{
	QString environment = "3#4#12";
	QCOMPARE(checkAddressIsForMe(environment, "0313#4#12"), true);
}

void TestCheckAddress::testCheckAddressEnvironmentLevel3()
{
	QString environment = "3#3";
	QCOMPARE(checkAddressIsForMe(environment, "0313"), true);
	environment = "00#3";
	QCOMPARE(checkAddressIsForMe(environment, "01"), true);
	environment = "100#3";
	QCOMPARE(checkAddressIsForMe(environment, "1014"), true);
}

void TestCheckAddress::testCheckAddressGroup()
{
	QString group = "#45";
	QCOMPARE(checkAddressIsForMe(group, "12"), false);
	QCOMPARE(checkAddressIsForMe(group, "34#4#45"), false);
}

