#include <landevice.h>
#include <main.h>

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QHash>
#include <QVariant>
#include <QMetaType>

// To put/extract in QVariant
typedef QHash<int, QVariant> StatusList;
Q_DECLARE_METATYPE(StatusList)

// To use QHash<> in signal/slots and watch them through QSignalSpy
int trick = qRegisterMetaType<QHash<int, QVariant> >("QHash<int, QVariant>");


class TestLanDevice: public QObject
{
	Q_OBJECT
private slots:
	void readStatus();
	void initTestCase();
private:
	LanDevice *dev;
};

void TestLanDevice::initTestCase()
{
	device::setClients(0, 0, 0);
	dev = new LanDevice;
}

void TestLanDevice::readStatus()
{
	QSignalSpy spy(dev, SIGNAL(status_changed(QHash<int, QVariant>)));
	dev->frame_rx_handler(const_cast<char*>("*#13**9*0##"));
	QCOMPARE(spy.count(), 1);
	QVariant signal_arg = spy.takeFirst().at(0);
	qDebug() << "can convert:" << signal_arg.canConvert<QHash<int, QVariant> >();
//	StatusList status_list = signal_arg.value<StatusList>();
}

 QTEST_MAIN(TestLanDevice)
 #include "test_main.moc"

