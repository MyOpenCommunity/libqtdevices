#ifndef TEST_DEVICE_H
#define TEST_DEVICE_H

#include <QObject>

class OpenServerMock;


/**
 * The base class for all tests about device.
 */
class TestDevice : public QObject
{
public:
	TestDevice();
	virtual ~TestDevice();

protected:
	OpenServerMock *server;
};


#endif // TEST_DEVICE_H
