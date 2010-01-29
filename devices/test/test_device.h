#ifndef TEST_DEVICE_H
#define TEST_DEVICE_H

#include <QObject>

class OpenServerMock;
class Client;


/**
 * The base class for all tests about device.
 *
 * The derived class should define tests using the following rules:
 * 1. receive* -> verify that the data structure built from the parsing of incoming frames
 *    from the server is correct.
 * 2. send* -> verify that the frame to be sent to the server is correctly created.
 * 3. test* -> tests that cannot be included in 1. & 2.
 */
class TestDevice : public QObject
{
public:
	TestDevice();
	void initTestDevice();
	virtual ~TestDevice();

protected:
	OpenServerMock *server;
	Client *client_command;
	Client *client_request;
	Client *client_monitor;
	void cleanBuffers();
};


#endif // TEST_DEVICE_H
