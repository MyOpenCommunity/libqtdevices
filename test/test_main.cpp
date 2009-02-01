#include <landevice.h>

#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QHash>
#include <QVariant>
#include <QMetaType>

// To put/extract in QVariant
Q_DECLARE_METATYPE(StatusList)


void getDimFromSignal(const QSignalSpy &spy, int dim_type, QVariant &res)
{
	QCOMPARE(spy.count(), 1);
	QVariant signal_arg = spy.at(0).at(0); // get the first argument from first signal
	QVERIFY(signal_arg.canConvert<StatusList>());
	StatusList sl = signal_arg.value<StatusList>();
	QVERIFY(sl.contains(dim_type));
	res = sl[dim_type];
}


class TestDevice
{
public:
	TestDevice(device *d, int dim);
	void check(QString frame, const QVariant &v);

private:
	QSignalSpy spy;
	int dim_type;
	device *dev;
};


TestDevice::TestDevice(device *d, int type) : spy(d, SIGNAL(status_changed(StatusList)))
{
	dim_type = type;
	dev = d;
}

void TestDevice::check(QString frame, const QVariant& v)
{
	QVariant res;
	spy.clear();
	dev->frame_rx_handler(frame.toAscii().data());
	getDimFromSignal(spy, dim_type, res);
	QVERIFY(res == v);
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
	void readGateway();
	void readDns1();
	void readDns2();

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
	TestDevice t(dev, LanDevice::DIM_STATUS);
	t.check("*#13**9*0##", false);
	t.check("*#13**9*1##", true);
}

void TestLanDevice::readIp()
{
	TestDevice t(dev, LanDevice::DIM_IP);
	t.check("*#13**10*10*3*3*81##", "10.3.3.81");
}

void TestLanDevice::readNetmask()
{
	TestDevice t(dev, LanDevice::DIM_NETMASK);
	t.check("*#13**11*255*255*255*0##", "255.255.255.0");
}

void TestLanDevice::readMacAddress()
{
	TestDevice t(dev, LanDevice::DIM_MACADDR);
	t.check("*#13**12*0*3*80*0*34*45##", "0:3:80:0:34:45");
}

void TestLanDevice::readGateway()
{
	TestDevice t(dev, LanDevice::DIM_GATEWAY);
	t.check("*#13**50*27*238*64*1##", "27.238.64.1");
}

void TestLanDevice::readDns1()
{
	TestDevice t(dev, LanDevice::DIM_DNS1);
	t.check("*#13**51*208*67*222*222##", "208.67.222.222");
}

void TestLanDevice::readDns2()
{
	TestDevice t(dev, LanDevice::DIM_DNS2);
	t.check("*#13**52*208*67*220*220*##", "208.67.220.220");
}

QTEST_MAIN(TestLanDevice)
#include "test_main.moc"

