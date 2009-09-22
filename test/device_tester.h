#ifndef DEVICE_TESTER_H
#define DEVICE_TESTER_H

#include <QSignalSpy>
#include <QStringList>
#include <QtTest/QtTest>
#include <QVariant>
#include <QMetaType>


class QStringList;
class QVariant;
class device;


template<class T> QString objToString(const T &val)
{
	return "unknown object";
}

template<> inline QString objToString(const bool &val)
{
	return val ? "true" : "false";
}

template<> inline QString objToString(const int &val)
{
	return QString::number(val);
}

template<> inline QString objToString(const QString &val)
{
	return val;
}


/**
  * An utility class to test the signal emitted by device after the parsing of
  * a frame from openserver.
  */
class DeviceTester
{
public:
	DeviceTester(device *d, int dim);

	template<class T> void check(const QStringList &frames, const T &result);

	// Overload methods, provided for convenience.
	template<class T> void check(QString frame, const T &result);
	void check(QString frame, const char *result);

	// Verify the number of signals emitted
	void checkSignals(const QStringList &frames, int num_signals);
	void checkSignals(QString frame, int num_signals);

private:
	QSignalSpy spy;
	int dim_type;
	device *dev;
	void sendFrames(const QStringList& frames);
	QVariant getResult(const QStringList& frames);
};


template<class T> void DeviceTester::check(const QStringList &frames, const T &result)
{
	QVariant r = getResult(frames);
	QVERIFY2(r.canConvert<T>(), "Unable to convert the result in the proper type");
	QString exp = objToString(result);
	QString obt = objToString(r.value<T>());
	QVERIFY2(result == r.value<T>(), qPrintable(QString("Expected result: %1, obtained: %2").arg(exp).arg(obt)));
}

template<class T> void DeviceTester::check(QString frame, const T &result)
{
	check(QStringList(frame), result);
}

#endif // DEVICE_TESTER_H
