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

#ifndef TEST_DELAYEDSLOTCALLER_H
#define TEST_DELAYEDSLOTCALLER_H

#include <QObject>
#include <QVariant>
#include <QList>
#include <QStringList>


struct CustomType
{
	int i;
	QStringList l;
	bool operator==(const CustomType &other)
	{
		return i == other.i && l == other.l;
	}
};

Q_DECLARE_METATYPE(CustomType);


Q_DECLARE_METATYPE(QList<int>);

typedef QHash<int,QString> Hash;
Q_DECLARE_METATYPE(Hash);


class SlotTester : public QObject
{
Q_OBJECT
public slots:
	void slotNoArg();
	void slotInt(int i);
	void slotQString(QString s);
	void slotCustomType(CustomType c);
	void slotQList(QList<int> l);
	void slotQHash(Hash h);
	void slotQVariant(QVariant v);
	void slotConstChar(const char *c);
	void slotMultipleArgs(int i, QStringList l, Hash h, int r);
	void slotMultipleCalls();

public:
	QList<QVariant> history;
};



class TestDelayedSlotCaller : public QObject
{
Q_OBJECT
public:
	TestDelayedSlotCaller();

private slots:
	void testSlotNoArg();
	void testDelay();
	void testSlotInt();
	void testSlotQString();
	void testSlotCustomType();
	void testSlotQList();
	void testSlotQHash();
	void testSlotQVariant();
	void testSlotConstChar();
	void testSlotMultipleArgs();
	void testSlotMultipleCalls();

private:
	int delay;
};


#endif
