#include <landevice.h>

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QHash>
#include <QVariant>
#include <QMetaType>

// To put/extract in QVariant
Q_DECLARE_METATYPE(StatusList)


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
	// To use StatusList in signal/slots and watch them through QSignalSpy
	qRegisterMetaType<StatusList>("StatusList");

	device::setClients(0, 0, 0);
	dev = new LanDevice;
}

void TestLanDevice::readStatus()
{
	QSignalSpy spy(dev, SIGNAL(status_changed(StatusList)));
	dev->frame_rx_handler(const_cast<char*>("*#13**9*0##"));
	QCOMPARE(spy.count(), 1);

	QVariant signal_arg = spy.takeFirst().at(0);
	QVERIFY(signal_arg.canConvert<StatusList>());
	StatusList sl = signal_arg.value<StatusList>();
	QVERIFY(sl.contains(LanDevice::DIM_STATUS));
	QVERIFY(sl[LanDevice::DIM_STATUS].toBool() == false);
}

 QTEST_MAIN(TestLanDevice)
 #include "test_main.moc"

