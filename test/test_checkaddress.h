#ifndef TEST_CHECKADDRESS_H
#define TEST_CHECKADDRESS_H

#include "test_device.h"

class TestCheckAddress : public TestDevice
{
Q_OBJECT
private slots:
	void testCheckAddressGlobal();
	void testCheckAddressGlobalLevel3();
	void testCheckAddressGlobalInterface();
	void testCheckAddressGlobalInterfaceFalse();
	void testCheckAddressEnvironment10();
	void testCheckAddressEnvironment0();
	void testCheckAddressEnvironmentInt();
	void testCheckAddressEnvironmentLevel3();
	void testCheckAddressGroup();
	void testCheckAddressPointToPoint();
};

#endif // TEST_CHECKADDRESS_H
