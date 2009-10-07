#ifndef TEST_AUTOMATION_DEVICE_H
#define TEST_AUTOMATION_DEVICE_H

#include "test_device.h"

class AutomationDevice;

class TestAutomationDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendGoUp();
	void sendGoDown();
	void sendStop();

private:
	AutomationDevice *dev;
};

#endif // TEST_AUTOMATION_DEVICE_H
