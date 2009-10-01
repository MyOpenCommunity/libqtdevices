#ifndef TEST_LIGHTING_DEVICE_H
#define TEST_LIGHTING_DEVICE_H

#include "test_device.h"

#include <lighting_device.h>

const QString LIGHT_DEVICE_WHERE = "0313";
const QString LIGHT_ADDR_EXTENSION = "#4#12";
const QString LIGHT_ADDR_EXTENSION_2 = "#4#01";

class LightingDevice;

class TestLightingDevice : public TestDevice
{
Q_OBJECT
public:
	TestLightingDevice();

private slots:
	void initTestCase();
	void cleanupTestCase();

	// these tests must be executed before the others, since they test the commodity
	// function checkAddress()
	void testCheckAddressGlobal();
	void testCheckAddressGlobal3();
	void testCheckAddressGlobalInterface();
	void testCheckAddressGlobalInterfaceFalse();
	void testCheckAddressEnvironment10();
	void testCheckAddressEnvironment0();
	void testCheckAddressEnvironmentInt();

	void sendTurnOn();
	void sendRequestStatus();
	void sendDimmerIncreaseLevel();
	void sendDimmerDecreaseLevel();

	void receiveLightOnOff();
	void receiveLightOnOffPull();
	void receiveLightOnOffNotPull();
	void receiveLightOnOffUnknown();
	void receiveLightOnOffPullExt();
	void receiveLightOnOffNotPullExt();
	void receiveLightOnOffUnknownExt();

private:
	void checkPullUnknown();
	void setParams(QString w, LightingDevice::PullMode m);

	LightingDevice *dev;
	Dimmer *dimmer;
};

#endif // TEST_LIGHTING_DEVICE_H
