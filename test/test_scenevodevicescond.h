#ifndef TEST_SCENEVODEVICESCOND_H
#define TEST_SCENEVODEVICESCOND_H

#include <QObject>
#include <QString>

class DeviceConditionDisplayMock;
class OpenServerMock;
class Client;
class QSignalSpy;


/**
 * The test class used to check the device conditions of the evolved scenarios.
 */
class TestScenEvoDevicesCond : public QObject
{
Q_OBJECT
public:
	TestScenEvoDevicesCond();
	virtual ~TestScenEvoDevicesCond();

private:
	OpenServerMock *server;
	Client *client_monitor;
	QString dev_where;

	DeviceConditionDisplayMock *mock_display;
	void checkCondition(QSignalSpy &spy, QString frame, bool satisfied);

private slots:
	void init();

	// tests below
	void testLightOn();
	void testLightOff();
	void testDimmingOff();
	void testDimmingRange1();
	void testDimmingRange2();
	void testDimmingRange3();
	void testDimming100Off();
	void testDimming100Range1();
	void testDimming100Range2();
	void testDimming100Range3();
	void testDimming100Range4();
	void testInternalTemperature1();
	void testInternalTemperature2();
	void testInternalTemperature3();
	void testInternalTemperatureMinMax();
	void testExternalTemperature1();
	void testExternalTemperature2();
	void testExternalTemperature3();
	void testExternalTemperatureMinMax();
	void testAux();
};

#endif // TEST_SCENEVODEVICESCOND_H
