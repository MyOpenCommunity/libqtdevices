#include "device_tester.h"

#include <device.h>

#include <QString>
#include <QVariant>
#include <QMetaType>
#include <QtTest/QtTest>

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

DeviceTester::DeviceTester(device *d, int type) : spy(d, SIGNAL(status_changed(const StatusList&)))
{
	dim_type = type;
	dev = d;
}

void DeviceTester::check(QString frame, const QVariant& v)
{
	QVariant res;
	spy.clear();
	dev->frame_rx_handler(frame.toAscii().data());
	getDimFromSignal(spy, dim_type, res);
	QVERIFY(res == v);
}

