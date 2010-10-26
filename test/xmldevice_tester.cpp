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

#include "xmldevice_tester.h"

#include "xmldevice.h"


XmlDeviceTester::XmlDeviceTester(XmlDevice *d, int resp_type)
	: spy(d, SIGNAL(responseReceived(XmlResponse)))
{
	dev = d;
	response_type = resp_type;
}

void XmlDeviceTester::simulateIncomingResponse(const QString &response)
{
	dev->handleData(response);
}

QVariant XmlDeviceTester::getResult()
{
	Q_ASSERT_X(spy.count() > 0, "XmlDeviceTester::getResult", "XmlDeviceTester: No signal emitted!");
	Q_ASSERT_X(spy.last().count() > 0, "XmlDeviceTester::getResult", "XmlDeviceTester: No arguments for the last signal emitted!");
	QVariant signal_arg = spy.last().at(0); // get the first argument from last signal
	if (signal_arg.canConvert<XmlResponse>())
	{
		XmlResponse response = signal_arg.value<XmlResponse>();
		if (response.contains(response_type))
		{
			Q_ASSERT_X(response.size() == 1, "DeviceTester::getResult", "DeviceValues must contain only one item");

			return response[response_type];
		}
	}
	Q_ASSERT_X(false, "XmlDeviceTester::getResult", "XmlDeviceTester: error on parsing the signal content.");
	return QVariant(); // only to avoid warning
}
