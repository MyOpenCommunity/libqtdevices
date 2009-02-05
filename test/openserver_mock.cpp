#include "openserver_mock.h"
#include "openclient.h"

#include <QTcpSocket>


OpenServerMock::OpenServerMock() : server(this)
{
	port = 10000;
	server.listen(QHostAddress(QHostAddress::LocalHost), port);
}

Client *OpenServerMock::connectMonitor()
{
	Client *client_monitor = new Client(Client::MONITOR, "127.0.0.1", port);
	monitor = server.nextPendingConnection();
	return client_monitor;
}

Client *OpenServerMock::connectCommand()
{
	Client *client_command = new Client(Client::COMANDI, "127.0.0.1", port);
	command = server.nextPendingConnection();
	return client_command;
}

Client *OpenServerMock::connectRequest()
{
	Client *client_request = new Client(Client::RICHIESTE, "127.0.0.1", port);
	request = server.nextPendingConnection();
	return client_request;
}

