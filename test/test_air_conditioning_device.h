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
	void sendSendOff();

private:
	void compareCommand(const QString &what);
	AirConditioningDevice *dev;
};



#endif // TESTAIRCONDITIONINGDEVICE_H
