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
#ifndef XMLDEVICE_TESTER_H
#define XMLDEVICE_TESTER_H

#include <QtTest>
#include <QSignalSpy>

class XmlDevice;


class XmlDeviceTester
{
public:
	XmlDeviceTester(XmlDevice *d, int resp_type);

	template<class T> void check(const QString &response, const T &result);

	void simulateIncomingResponse(const QString &response);

	QVariant getResult();

private:
	QSignalSpy spy;
	int response_type;
	XmlDevice *dev;
};

template<class T> void XmlDeviceTester::check(const QString &response, const T &result)
{
	simulateIncomingResponse(response);

	QVariant r = getResult();
	QVERIFY2(r.canConvert<T>(), "Unable to convert the result in the proper type");
	QCOMPARE(r.value<T>(), result);
}

#endif
