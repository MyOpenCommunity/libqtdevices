#include "device_tester.h"

#include <device.h>

#include <QStringList>
#include <QVariant>
#include <QMetaType>
#include <QtTest/QtTest>

#include <assert.h>

// To put/extract in QVariant
Q_DECLARE_METATYPE(StatusList)


DeviceTester::DeviceTester(device *d, int type) : spy(d, SIGNAL(status_changed(const StatusList&)))
{
	dim_type = type;
	dev = d;
}

void DeviceTester::sendFrames(const QStringList& frames)
{
	spy.clear();
	for (int i = 0; i < frames.size(); ++i)
		dev->frame_rx_handler(frames[i].toAscii().data());
}

void DeviceTester::checkSignals(QString frame, int num_signals)
{
	sendFrames(QStringList(frame));
	QVERIFY(spy.count() == num_signals);
}

void DeviceTester::checkSignals(const QStringList& frames, int num_signals)
{
	sendFrames(frames);
	QVERIFY(spy.count() == num_signals);
}

QVariant DeviceTester::getResult(const QStringList& frames)
{
	sendFrames(frames);
	assert(spy.count() > 0 && "DeviceTester: No signal emitted!");
	assert(spy.last().count() > 0 && "DeviceTester: No arguments for the last signal emitted!");
	QVariant signal_arg = spy.last().at(0); // get the first argument from last signal
	if (signal_arg.canConvert<StatusList>())
	{
		StatusList sl = signal_arg.value<StatusList>();
		if (sl.contains(dim_type))
			return sl[dim_type];
	}
	return QVariant();
}

void DeviceTester::check(QString frame, const char *result)
{
	check(QStringList(frame), QString(result));
}

