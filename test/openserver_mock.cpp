#include "openserver_mock.h"
#include "openclient.h"

#include <QTcpSocket>


OpenServerMock::OpenServerMock() : server(this)
{
	timeout = 3000;
	if (!server.listen(QHostAddress(QHostAddress::LocalHost)))
		qFatal("Fatal error: OpenServerMock cannot listen");
}

Client *OpenServerMock::connectMonitor()
{
	Client *client_monitor = new Client(Client::MONITOR, "127.0.0.1", server.serverPort());
	if (!client_monitor->socket->waitForConnected(timeout))
		qFatal("Fatal error: client_monitor cannot connect to OpenServerMock");
	server.waitForNewConnection(timeout);
	monitor = server.nextPendingConnection();
	return client_monitor;
}

Client *OpenServerMock::connectCommand()
{
	Client *client_command = new Client(Client::COMANDI, "127.0.0.1", server.serverPort());
	if (!client_command->socket->waitForConnected(timeout))
		qFatal("Fatal error: client_command cannot connect to OpenServerMock");
	server.waitForNewConnection(timeout);
	command = server.nextPendingConnection();
	return client_command;
}

Client *OpenServerMock::connectRequest()
{
	Client *client_request = new Client(Client::RICHIESTE, "127.0.0.1", server.serverPort());
	if (!client_request->socket->waitForConnected(timeout))
		qFatal("Fatal error: client_request cannot connect to OpenServerMock");
	server.waitForNewConnection(timeout);
	request = server.nextPendingConnection();
	return client_request;
}

QString OpenServerMock::frameRequest()
{
	request->waitForReadyRead(timeout);
	return request->readAll();
}

