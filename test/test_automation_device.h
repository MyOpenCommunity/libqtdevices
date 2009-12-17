#ifndef TEST_AUTOMATION_DEVICE_H
#define TEST_AUTOMATION_DEVICE_H

#include "test_checkaddress.h"

class AutomationDevice;
class PPTStatDevice;
class QString;

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


class TestPPTStatDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();
	void receiveStatus();

private:
	PPTStatDevice *dev;
	QString where;
};


#endif // TEST_AUTOMATION_DEVICE_H
