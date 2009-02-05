#include "test_device.h"
#include "openserver_mock.h"

#include <device.h>

#include <QVariant>
#include <QMetaType>


TestDevice::TestDevice()
{
	// To use StatusList in signal/slots and watch them through QSignalSpy
	qRegisterMetaType<StatusList>("StatusList");

	server = new OpenServerMock;
	device::setClients(server->connectCommand(), server->connectMonitor(), server->connectRequest());
}

TestDevice::~TestDevice()
{
	delete server;
}
