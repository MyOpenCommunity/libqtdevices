#include "test_device.h"
#include "openserver_mock.h"

#include <device.h>
#include <frame_receiver.h>

#include <QVariant>
#include <QMetaType>


TestDevice::TestDevice()
{
	// To use StatusList in signal/slots and watch them through QSignalSpy
	qRegisterMetaType<StatusList>("StatusList");

	server = new OpenServerMock;
	client_command = server->connectCommand();
	client_request = server->connectRequest();
	client_monitor = server->connectMonitor();
	device::setClients(client_command, client_request);
	FrameReceiver::setClientMonitor(client_monitor);
}

TestDevice::~TestDevice()
{
	delete server;
}
