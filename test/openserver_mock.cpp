#include "openserver_mock.h"
#include "openclient.h"

#include <QTcpSocket>


OpenServerMock::OpenServerMock() : server(this)
{
	default_timeout = 3000;
	if (!server.listen(QHostAddress(QHostAddress::LocalHost)))
		qFatal("Fatal error: OpenServerMock cannot listen");
}

Client *OpenServerMock::connectMonitor()
{
	Client *client_monitor = new Client(Client::MONITOR, "127.0.0.1", server.serverPort());
	if (!client_monitor->socket->waitForConnected(default_timeout))
		qFatal("Fatal error: client_monitor cannot connect to OpenServerMock");
	server.waitForNewConnection(default_timeout);
	monitor = server.nextPendingConnection();
	return client_monitor;
}

Client *OpenServerMock::connectCommand()
{
	Client *client_command = new Client(Client::COMANDI, "127.0.0.1", server.serverPort());
	if (!client_command->socket->waitForConnected(default_timeout))
		qFatal("Fatal error: client_command cannot connect to OpenServerMock");
	server.waitForNewConnection(default_timeout);
	command = server.nextPendingConnection();
	client_command->flush();
	frameCommand(); // discard all data
	return client_command;
}

Client *OpenServerMock::connectRequest()
{
	Client *client_request = new Client(Client::RICHIESTE, "127.0.0.1", server.serverPort());
	if (!client_request->socket->waitForConnected(default_timeout))
		qFatal("Fatal error: client_request cannot connect to OpenServerMock");
	server.waitForNewConnection(default_timeout);
	request = server.nextPendingConnection();
	client_request->flush();
	frameRequest(); // discard all data
	return client_request;
}

QString OpenServerMock::frameRequest(unsigned int timeout)
{
	if (!timeout)
		timeout = default_timeout;
	request->waitForReadyRead(timeout);
	return request->readAll();
}

QString OpenServerMock::frameCommand(unsigned int timeout)
{
	if (!timeout)
		timeout = default_timeout;
	command->waitForReadyRead(timeout);
	return command->readAll();
}

