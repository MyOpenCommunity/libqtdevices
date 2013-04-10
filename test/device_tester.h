/* 
 * BTouch - Graphical User Interface to control MyHome System
 *
 * Copyright (C) 2010 BTicino S.p.A.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef DEVICE_TESTER_H
#define DEVICE_TESTER_H

#include <QSignalSpy>
#include <QStringList>
#include <QtTest/QtTest>
#include <QVariant>
#include <QMetaType>
#include <QList>
#include <QPair>

#include <device.h>


/**
  * An utility class to test the signal emitted by device, usually after the
  * parsing of a frame from openserver.
  *
  */
class DeviceTester
{
public:
	enum ValuesNumber
	{
		ONE_VALUE,
		MULTIPLE_VALUES,
	};
	DeviceTester(device *d, int dim, ValuesNumber it = ONE_VALUE);

	// Simulate an incoming a list of frames from the openserver and check
	// the signal emitted
	template<class T> void check(const QStringList &frames, const T &result);

	// Overload methods, provided for convenience.
	template<class T> void check(QString frame, const T &result);
	void check(QString frame, const char *result);

	// Check the signal emitted by the device without simulate incoming frames.
	template<class T> void check(const T &result);
	void check(const char *result) { check(QString(result)); }

	// Verify the number of signals emitted
	void checkSignals(const QStringList &frames, int num_signals);
	void checkSignals(QString frame, int num_signals);

	void simulateIncomingFrames(const QStringList& frames);
	QVariant getResult();

	// Register the device arguments as a receiver of the incoming (simulated) frames.
	void addReceiver(device *d);

private:
	QSignalSpy spy;
	int dim_type;
	device *dev;
	ValuesNumber item_number;
	QList <device*> receivers;
};


template<class T> void DeviceTester::check(const QStringList &frames, const T &result)
{
	simulateIncomingFrames(frames);
	check(result);
}

template<class T> void DeviceTester::check(QString frame, const T &result)
{
	check(QStringList(frame), result);
}

template<class T> void DeviceTester::check(const T &result)
{
	QVariant r = getResult();
	QVERIFY2(r.canConvert<T>(), "Unable to convert the result in the proper type");
	QCOMPARE(r.value<T>(), result);
}



// The interface of the device checker, do not use directly
class DeviceCheckerInterface
{
public:
	virtual void compare(DeviceValues) = 0;
	virtual int getDimension() = 0;
};


// An internal class, used by the MultiDeviceTester class to store and perform
// the test with custom types.
template <class T> class DeviceChecker : public DeviceCheckerInterface
{
public:
	DeviceChecker(int dim, T val)
	{
		dimension = dim;
		value = val;
	}

	virtual void compare(DeviceValues values)
	{
		Q_ASSERT_X(values.contains(dimension), "DeviceChecker::compare",
			qPrintable(QString("DeviceValues does not contains the dimension %1").arg(dimension)));
		Q_ASSERT_X(values[dimension].canConvert<T>(), "DeviceChecker::compare",
			"Unable to convert the result to the target type");
		QCOMPARE(values[dimension].value<T>(), value);
	}

	virtual int getDimension() { return dimension; }
private:
	T value;
	int dimension;
};


/**
 * This class is very similar to the DeviceTester but instead of testing one or
 * more frames and a single dimension it tests if a frame produces more dimensions.
 */
class MultiDeviceTester
{
public:
	enum CheckType
	{
		ALL_VALUES,
		CONTAINS
	};

	MultiDeviceTester(device *d);
	~MultiDeviceTester();

	// Register the device arguments as a receiver of the frame.
	void addReceiver(device *d);

	// Add the couple <dimension, value> to the list of dimensions to check
	template <class T1, class T2> void operator<<(QPair<T1, T2> dim);
	template <class T1> void operator<<(QPair<T1, const char*> dim);

	// Perform the test, checking also (if the second arguments has ALL_VALUES
	// as value) that the number dimensions expected is the same of the actual dimensions.
	void check(QString frame, CheckType check_type = ALL_VALUES);

	// Check the signal emitted without sending frames.
	void check(CheckType check_type = ALL_VALUES);

private:
	device *dev;
	QList<DeviceCheckerInterface*> checkers;
	QSignalSpy spy;
	QList <device*> receivers;
};


template <class T1, class T2> void MultiDeviceTester::operator<<(QPair<T1, T2> dim)
{
	checkers << new DeviceChecker<T2>(dim.first, dim.second);
}

template <class T1> void MultiDeviceTester::operator<<(QPair<T1, const char*> dim)
{
	checkers << new DeviceChecker<QString>(dim.first, QString(dim.second));
}


// To use the const char* as argument we need to redefine a function similar to
// qMakePair and add a specialization (that convert the const char* in a QString)
// to the MultiDeviceTester class.
template <class T1, class T2> QPair<T1, T2> makePair(T1 t1, T2 t2) { return qMakePair(t1, t2); }



#endif // DEVICE_TESTER_H
