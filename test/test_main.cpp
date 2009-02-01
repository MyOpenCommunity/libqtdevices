#include <landevice.h>

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QHash>
#include <QVariant>
#include <QMetaType>

// To put/extract in QVariant
Q_DECLARE_METATYPE(StatusList)


void getDimFromSignal(const QSignalSpy &spy, int dim_type, QVariant &dim)
{
	QCOMPARE(spy.count(), 1);
	QVariant signal_arg = spy.at(0).at(0); // get the first argument from first signal
	QVERIFY(signal_arg.canConvert<StatusList>());
	StatusList sl = signal_arg.value<StatusList>();
	QVERIFY(sl.contains(dim_type));
	dim = sl[dim_type];
}


class TestLanDevice: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

	void readStatus();
	void readIp();
	void readNetmask();
	void readMacAddress();

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

void TestLanDevice::cleanupTestCase()
{
	delete dev;
}

void TestLanDevice::readStatus()
{
	QSignalSpy spy(dev, SIGNAL(status_changed(StatusList)));
	QVariant dim;

	dev->frame_rx_handler(const_cast<char*>("*#13**9*0##"));
	getDimFromSignal(spy, LanDevice::DIM_STATUS, dim);
	QVERIFY(dim.toBool() == false);

	spy.clear();

	dev->frame_rx_handler(const_cast<char*>("*#13**9*1##"));
	getDimFromSignal(spy, LanDevice::DIM_STATUS, dim);
	QVERIFY(dim.toBool() == true);
}

void TestLanDevice::readIp()
{
	QSignalSpy spy(dev, SIGNAL(status_changed(StatusList)));
	QVariant dim;

	dev->frame_rx_handler(const_cast<char*>("*#13**10*10*3*3*81##"));
	getDimFromSignal(spy, LanDevice::DIM_IP, dim);
	QVERIFY(dim.toString() == "10.3.3.81");
}

void TestLanDevice::readNetmask()
{
	QSignalSpy spy(dev, SIGNAL(status_changed(StatusList)));
	QVariant dim;

	dev->frame_rx_handler(const_cast<char*>("*#13**11*255*255*255*0##"));
	getDimFromSignal(spy, LanDevice::DIM_NETMASK, dim);
	QVERIFY(dim.toString() == "255.255.255.0");
}

void TestLanDevice::readMacAddress()
{
	QSignalSpy spy(dev, SIGNAL(status_changed(StatusList)));
	QVariant dim;

	dev->frame_rx_handler(const_cast<char*>("*#13**12*0*3*80*0*34*45##"));
	getDimFromSignal(spy, LanDevice::DIM_MACADDR, dim);
	QVERIFY(dim.toString() == "0:3:80:0:34:45");
}


QTEST_MAIN(TestLanDevice)
#include "test_main.moc"

