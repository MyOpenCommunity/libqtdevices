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


#include "device_tester.h"

#include <device.h>
#include <openmsg.h>

#include <QStringList>
#include <QVariant>
#include <QMetaType>
#include <QtTest/QtTest>

#include <assert.h>

// To put/extract in QVariant
Q_DECLARE_METATYPE(DeviceValues)


DeviceTester::DeviceTester(device *d, int type, ValuesNumber it) :
	spy(d, SIGNAL(status_changed(const DeviceValues&)))
{
	dim_type = type;
	dev = d;
	item_number = it;
}

void DeviceTester::sendFrames(const QStringList& frames)
{
	spy.clear();
	for (int i = 0; i < frames.size(); ++i)
	{
		OpenMsg msg;
		msg.CreateMsgOpen(frames[i].toAscii().data(), frames[i].length());
		dev->manageFrame(msg);
	}
}

void DeviceTester::checkSignals(QString frame, int num_signals)
{
	sendFrames(QStringList(frame));
	QCOMPARE(spy.count(), num_signals);
}

void DeviceTester::checkSignals(const QStringList& frames, int num_signals)
{
	sendFrames(frames);
	QCOMPARE(spy.count(), num_signals);
}

QVariant DeviceTester::getResult(const QStringList& frames)
{
	sendFrames(frames);
	Q_ASSERT_X(spy.count() > 0, "DeviceTester::getResult", "DeviceTester: No signal emitted!");
	Q_ASSERT_X(spy.last().count() > 0, "DeviceTester::getResult", "DeviceTester: No arguments for the last signal emitted!");
	QVariant signal_arg = spy.last().at(0); // get the first argument from last signal
	if (signal_arg.canConvert<DeviceValues>())
	{
		DeviceValues values_list = signal_arg.value<DeviceValues>();
		if (values_list.contains(dim_type))
		{
			if (item_number == ONE_VALUE)
				Q_ASSERT_X(values_list.size() == 1, "DeviceTester::getResult",
					"DeviceValues must contain only one item");
			return values_list[dim_type];
		}
	}
	return QVariant();
}

void DeviceTester::check(QString frame, const char *result)
{
	check(QStringList(frame), QString(result));
}

