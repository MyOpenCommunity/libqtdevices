#ifndef TEST_SCENEVODEVICESCOND_H
#define TEST_SCENEVODEVICESCOND_H

#include <QObject>

class DeviceConditionDisplayMock;
class OpenServerMock;
class Client;
class QSignalSpy;
class QString;


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

	DeviceConditionDisplayMock *mock_display;
	void checkCondition(QSignalSpy &spy, QString frame, bool satisfied);

private slots: 	//  tests below
	void testLightOn();
	void testLightOff();
};

#endif // TEST_SCENEVODEVICESCOND_H
