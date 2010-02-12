#ifndef TEST_BTTIME_H
#define TEST_BTTIME_H

#include "test_device.h"

#include <QtTest/QTest>

class BtTime;

namespace QTest {
	template<> char *toString(const BtTime &t);
}

class TestBtTime : public TestDevice
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
};

#endif // TEST_BTTIME_H
