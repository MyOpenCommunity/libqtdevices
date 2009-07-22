#ifndef OPENSERVER_MOCK_H
#define OPENSERVER_MOCK_H

#include <QObject>
#include <QPair> // prevent a bug in Qt 4.5.0
#include <QTcpServer>

class QTcpSocket;
class Client;


class OpenServerMock : public QObject
{
Q_OBJECT
public:
	OpenServerMock();

	Client *connectMonitor();
	Client *connectCommand();
	Client *connectRequest();
	QString frameRequest();
	QString frameCommand();

private:
	QTcpServer server;
	int port;
	int timeout;

	QTcpSocket *command, *monitor, *request;
};

#endif // OPENSERVER_MOCK_H
