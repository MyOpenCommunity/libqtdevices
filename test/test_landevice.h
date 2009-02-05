#ifndef TEST_LANDEVICE_H
#define TEST_LANDEVICE_H

#include <QObject>

class LanDevice;
class OpenServerMock;


class TestLanDevice : public QObject
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void readStatus();
	void readIp();
	void readNetmask();
	void readMacAddress();
	void readGateway();
	void readDns1();
	void readDns2();

private:
	LanDevice *dev;
	OpenServerMock *server;
};

#endif // TEST_LANDEVICE_H
