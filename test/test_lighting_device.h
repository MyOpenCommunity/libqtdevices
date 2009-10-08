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
protected:
	void initLightingDevice();
	void cleanupLightingDevice();
	void setParams(QString w, PullMode m);

private slots:
	void init();
	void initTestCase();
	void cleanupTestCase();

	void sendTurnOn();
	void sendRequestStatus();
	void sendFixedTiming();
	void sendVariableTiming();
	void sendRequestVariableTiming();

	void receiveLightOnOff();
	void receiveLightOnOffPull();
	void receiveLightOnOffNotPull();

	void receiveLightOnOffUnknown();
	void receiveLightOnOffUnknown2();
	void receiveLightOnOffPullExt();
	void receiveLightOnOffNotPullExt();
	void receiveLightOnOffUnknownExt2();
	void receiveLightOnOffUnknownExt();
	void receiveVariableTiming();
private:
	void checkPullUnknown();

	LightingDevice *dev;
};

class TestDimmer : public TestLightingDevice
{
Q_OBJECT
protected:
	void initDimmer();
	void cleanupDimmer();
	virtual void checkLevel();

private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendDimmerIncreaseLevel();
	void sendDimmerDecreaseLevel();

	void receiveDimmerLevel();
	void receiveDimmerProblem();

private:
	DimmerDevice *dimmer;
};

class TestDimmer100 : public TestDimmer
{
Q_OBJECT
protected:
	virtual void checkLevel();

private slots:
	void initTestCase();
	void cleanupTestCase();

	void sendDimmer100IncreaseLevel();
	void sendDimmer100DecreaseLevel();
	void sendRequestDimmer100Status();

	void receiveDimmer100Status();

private:
	Dimmer100Device *dimmer100;
};
#endif // TEST_LIGHTING_DEVICE_H
