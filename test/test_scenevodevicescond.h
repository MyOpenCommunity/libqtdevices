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

};

#endif // TEST_SCENEVODEVICESCOND_H
