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

#include "test_delayedslotcaller.h"
#include "test_functions.h"

#include <delayedslotcaller.h>

#include <QDebug>
#include <QtGlobal>
#include <QTest>
#include <QString>

void SlotTester::slotNoArg()
{
	history << QString("SlotTester::slotNoArg");
}

void SlotTester::slotInt(int i)
{
	history << QString("SlotTester::slotInt") << i;
}

void SlotTester::slotQString(QString s)
{
	history << QString("SlotTester::slotQString") << s;
}

void SlotTester::slotCustomType(CustomType c)
{
	history << QString("SlotTester::slotCustomType") << QVariant::fromValue(c);
}

void SlotTester::slotQList(QList<int> l)
{
	history << QString("SlotTester::slotQList") << QVariant::fromValue(l);
}

void SlotTester::slotQHash(Hash h)
{
	history << QString("SlotTester::slotQHash") << QVariant::fromValue(h);
}

void SlotTester::slotQVariant(QVariant v)
{
	history << QString("SlotTester::slotQVariant") << v;
}

void SlotTester::slotMultipleArgs(int i, QStringList l, Hash h, int r)
{
	history << QString("SlotTester::slotMultipleArgs") << i << l;
	history << QVariant::fromValue(h) << r;
}


TestDelayedSlotCaller::TestDelayedSlotCaller()
{
	delay = 200;
}

void TestDelayedSlotCaller::testSlotNoArg()
{
	DelayedSlotCaller d;
	SlotTester tester;
	d.setSlot(&tester, SLOT(slotNoArg()), delay);
	testSleep(delay);
	QCOMPARE(tester.history.at(0).toString(), QString("SlotTester::slotNoArg"));
}

void TestDelayedSlotCaller::testDelay()
{
	int small_interval = 10;
	DelayedSlotCaller d;
	SlotTester tester;
	d.setSlot(&tester, SLOT(slotNoArg()), delay);
	testSleep(delay - small_interval);
	QCOMPARE(tester.history.size(), 0);
	testSleep(small_interval);
	QCOMPARE(tester.history.at(0).toString(), QString("SlotTester::slotNoArg"));
}

void TestDelayedSlotCaller::testSlotInt()
{
	DelayedSlotCaller d;
	SlotTester tester;
	d.setSlot(&tester, SLOT(slotInt(int)), delay);
	d.addArgument(5);
	testSleep(delay);
	QCOMPARE(tester.history.at(0).toString(), QString("SlotTester::slotInt"));
	QCOMPARE(tester.history.at(1).toInt(), 5);
}

void TestDelayedSlotCaller::testSlotQString()
{
	DelayedSlotCaller d;
	SlotTester tester;
	d.setSlot(&tester, SLOT(slotQString(QString)), delay);
	d.addArgument("a common string");
	testSleep(delay);
	QCOMPARE(tester.history.at(0).toString(), QString("SlotTester::slotQString"));
	QCOMPARE(tester.history.at(1).toString(), QString("a common string"));
}

void TestDelayedSlotCaller::testSlotCustomType()
{
	DelayedSlotCaller d;
	SlotTester tester;
	d.setSlot(&tester, SLOT(slotCustomType(CustomType)), delay);
	CustomType c;
	c.i = 10;
	c.l << "first" << "second" << "third";
	d.addArgument(c);
	testSleep(delay);
	QCOMPARE(tester.history.at(0).toString(), QString("SlotTester::slotCustomType"));
	QVERIFY(tester.history.at(1).value<CustomType>() == c);
}

void TestDelayedSlotCaller::testSlotQList()
{
	DelayedSlotCaller d;
	SlotTester tester;
	d.setSlot(&tester, SLOT(slotQList(QList<int>)), delay);
	QList<int> args;
	args << 5 << 2 << 3 << 9 << -3 << 0;
	d.addArgument(args);
	testSleep(delay);
	QCOMPARE(tester.history.at(0).toString(), QString("SlotTester::slotQList"));
	QVERIFY(tester.history.at(1).value<QList<int> >() == args);
}

void TestDelayedSlotCaller::testSlotQHash()
{
	DelayedSlotCaller d;
	SlotTester tester;
	d.setSlot(&tester, SLOT(slotQHash(Hash)), delay);

	Hash args;
	args[1] = "one";
	args[2] = "two";
	args[3] = "three";

	d.addArgument(args);
	testSleep(delay);
	QCOMPARE(tester.history.at(0).toString(), QString("SlotTester::slotQHash"));
	QVERIFY(tester.history.at(1).value<Hash>() == args);
}

void TestDelayedSlotCaller::testSlotQVariant()
{
	DelayedSlotCaller d;
	SlotTester tester;
	d.setSlot(&tester, SLOT(slotQVariant(QVariant)), delay);
	QVariant v(100);
	d.addArgument(v);
	testSleep(delay);
	QCOMPARE(tester.history.at(0).toString(), QString("SlotTester::slotQVariant"));
	QVERIFY(tester.history.at(1) == v);
}

void TestDelayedSlotCaller::testSlotMultipleArgs()
{
	DelayedSlotCaller d;
	SlotTester tester;
	d.setSlot(&tester, SLOT(slotMultipleArgs(int,QStringList,Hash,int)), delay);
	d.addArgument(0);
	QStringList l;
	l << "hello" << "world";
	d.addArgument(l);
	Hash h;
	h[1] = "first";
	h[2] = "second";
	d.addArgument(h);
	d.addArgument(-3);
	testSleep(delay);
	QCOMPARE(tester.history.at(0).toString(), QString("SlotTester::slotMultipleArgs"));
	QVERIFY(tester.history.at(1).toInt() == 0);
	QVERIFY(tester.history.at(2).toStringList() == l);
	QVERIFY(tester.history.at(3).value<Hash>() == h);
	QVERIFY(tester.history.at(4).toInt() == -3);
}
