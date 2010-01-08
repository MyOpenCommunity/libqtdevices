#ifndef TESTAIRCONDITIONINGDEVICE_H
#define TESTAIRCONDITIONINGDEVICE_H

#include "test_device.h"

class AirConditioningDevice;
class AdvancedAirConditioningDevice;

class TestAirConditioningDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendActivateScenario();
	void sendSendCommand();
	void sendTurnOff();

private:
	void compareCommand(const QString &what);
	AirConditioningDevice *dev;
};

class TestAdvancedAirConditioningDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendRequestStatus();

	void testStatusToString();
	void testStatusToString2();
	void testStatusToString3();
	void testStatusToString4();

private:
	AdvancedAirConditioningDevice *dev;
};

#endif // TESTAIRCONDITIONINGDEVICE_H
