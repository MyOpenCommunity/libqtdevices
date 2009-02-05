#ifndef TEST_ENERGY_DEVICE_H
#define TEST_ENERGY_DEVICE_H

#include <QObject>
#include <QString>

class EnergyDevice;
class OpenServerMock;

class TestEnergyDevice : public QObject
{
Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void readCumulativeDay();
	void readCurrent();
	void readCumulativeMonth();
	void readCumulativeYear();

private:
	OpenServerMock *server;
	EnergyDevice *dev;
	QString where;
};


#endif // TEST_ENERGY_DEVICE_H
