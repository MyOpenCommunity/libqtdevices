#ifndef TEST_PULL_MANAGER_H
#define TEST_PULL_MANAGER_H
#include "test_device.h"

class TestPullManager : public TestDevice
{
Q_OBJECT
private slots:
	void testSimpleLight();
	void testSimpleLight2();
	void testDimmer();
	void testDimmer2();
	void testDimmer3();
	void testDimmer100();
	void testDimmer100_2();
	void testDimmer100_3();
	void testVariableTiming();
	void testVariableTiming2();
	void testVariableTiming3();
};

#endif //TEST_PULL_MANAGER_H
