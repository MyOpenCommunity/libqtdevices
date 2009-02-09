#ifndef TEST_DEVICE_H
#define TEST_DEVICE_H

#include <QObject>

class OpenServerMock;
class Client;


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
	Client *client_command;
	Client *client_request;
	Client *client_monitor;
};


#endif // TEST_DEVICE_H
