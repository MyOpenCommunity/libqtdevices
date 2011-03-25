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

#include <assert.h>

// To put/extract in QVariant
Q_DECLARE_METATYPE(DeviceValues)


DeviceTester::DeviceTester(device *d, int type, ValuesNumber it) :
	spy(d, SIGNAL(valueReceived(const DeviceValues&)))
{
	dim_type = type;
	dev = d;
	item_number = it;
}

void DeviceTester::simulateIncomingFrames(const QStringList& frames)
{
	spy.clear();
	for (int i = 0; i < frames.size(); ++i)
	{
		OpenMsg msg;
		msg.CreateMsgOpen(frames[i].toAscii().data(), frames[i].length());
		dev->manageFrame(msg);
		foreach (device *dev, receivers)
			dev->manageFrame(msg);
	}
}

void DeviceTester::addReceiver(device *d)
{
	receivers.append(d);
}

void DeviceTester::checkSignals(QString frame, int num_signals)
{
	simulateIncomingFrames(QStringList(frame));
	QCOMPARE(spy.count(), num_signals);
}

void DeviceTester::checkSignals(const QStringList& frames, int num_signals)
{
	simulateIncomingFrames(frames);
	QCOMPARE(spy.count(), num_signals);
}

QVariant DeviceTester::getResult()
{
	Q_ASSERT_X(spy.count() > 0, "DeviceTester::getResult", "No signal emitted!");
	Q_ASSERT_X(spy.last().count() > 0, "DeviceTester::getResult", "No arguments for the last signal emitted!");
	QVariant signal_arg = spy.last().at(0);
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
	Q_ASSERT_X(false, "DeviceTester::getResult", "Error on parsing the signal content.");
	return QVariant(); // only to avoid warning
}

void DeviceTester::check(QString frame, const char *result)
{
	check(QStringList(frame), QString(result));
}



MultiDeviceTester::MultiDeviceTester(device *d) : spy(d, SIGNAL(valueReceived(const DeviceValues&)))
{
	dev = d;
}

MultiDeviceTester::~MultiDeviceTester()
{
	for (int i = 0; i < checkers.size(); ++i)
		delete checkers[i];
}

void MultiDeviceTester::addReceiver(device *d)
{
	receivers.append(d);
}

void MultiDeviceTester::check(QString frame, CheckType check_type)
{
	OpenMsg msg;
	msg.CreateMsgOpen(frame.toAscii().data(), frame.length());
	dev->manageFrame(msg);
	foreach (device *dev, receivers)
		dev->manageFrame(msg);

	Q_ASSERT_X(spy.count() > 0, "MultiDeviceTester::check", "No signal emitted!");
	Q_ASSERT_X(spy.last().count() > 0, "MultiDeviceTester::check", "No arguments for the last signal emitted!");
	QVariant signal_arg = spy.last().at(0);
	Q_ASSERT_X(signal_arg.canConvert<DeviceValues>(), "MultiDeviceTester::check", "Error on parsing the signal content.");

	DeviceValues v = signal_arg.value<DeviceValues>();
	for (int i = 0; i < checkers.size(); ++i)
		checkers[i]->compare(v);

	if (check_type == ALL_VALUES && v.size() != checkers.size())
	{
		for (int i = 0; i < checkers.size(); ++i)
			if (v.contains(checkers[i]->getDimension()))
				v.remove(checkers[i]->getDimension());

		QStringList remainders;
		QHashIterator<int, QVariant> it(v);
		while (it.hasNext())
		{
			it.next();
			remainders.append(QString::number(it.key()));
		}

		Q_ASSERT_X(false, "MultiDeviceTester::check", qPrintable(QString("Unexpected dimensions received: %1").arg(remainders.join(","))));
	}
}
