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


class QStringList;
class QVariant;
class device;


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

#endif // DEVICE_TESTER_H
