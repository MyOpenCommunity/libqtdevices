#ifndef TEST_SCENARIO_DEVICE_H
#define TEST_SCENARIO_DEVICE_H

#include "test_device.h"

class ScenarioDevice;

class TestScenarioDevice : public TestDevice
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendActivateScenario();
	void sendStartProgramming();
	void sendStopProgramming();
	void sendDeleteAll();
	void sendDeleteScenario();
	void sendRequestStatus();

	void receiveStartProgramming();
	void receiveStopProgramming();
	void receiveLockDevice();
	void receiveUnlockDevice();
	void receiveGenericModuleIsProgramming();
	void receiveGenericStopProgramming();
	void receiveGenericStartProgramming();

private:
	ScenarioDevice *dev;
};

#endif // TEST_SCENARIO_DEVICE_H
