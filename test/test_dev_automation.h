#ifndef TEST_DEV_AUTOMATION_H
#define TEST_DEV_AUTOMATION_H

#include "test_device.h"

#include <QString>

class PPTStatDevice;


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

#endif // TEST_DEV_AUTOMATION_H
