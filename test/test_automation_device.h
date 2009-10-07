#ifndef TEST_AUTOMATION_DEVICE_H
#define TEST_AUTOMATION_DEVICE_H

#include "test_checkaddress.h"

class AutomationDevice;

class TestAutomationDevice : public TestCheckAddress
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendGoUp();
	void sendGoDown();
	void sendStop();

	// TODO: test PULL modes. Use a base class to share code with lighting tests?
	void receiveUp();
	void receiveDown();
	void receiveStop();

private:
	AutomationDevice *dev;
};

#endif // TEST_AUTOMATION_DEVICE_H
