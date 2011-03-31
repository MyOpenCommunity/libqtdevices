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

#include "delayedslotcaller.h"

#include <QMetaObject>
#include <QMetaMethod>
#include <QTimerEvent>
#include <QTimer>
#include <QDebug>


DelayedSlotCaller::DelayedSlotCaller(QObject *parent, bool single_shot) : QObject(parent)
{
	timer_id = -1;
	is_single_shot = single_shot;
}

void DelayedSlotCaller::setSlot(QObject *receiver, const char *slot, int msecs)
{
	Q_ASSERT_X(timer_id == -1, "DelayedSlotCaller::setSlot", qPrintable(QString("Call with an already defined slot %1").arg(slot_to_call)));
	const QMetaObject *mo = receiver->metaObject();

	QByteArray signature = QMetaObject::normalizedSignature(slot + 1);
	if (mo->indexOfMethod(signature) == -1)
	{
		qWarning() << "DelayedSlotCaller::setSlot -> no such slot" << signature;
		return;
	}

	QMetaMethod method = mo->method(mo->indexOfMethod(signature));

	const QList<QByteArray> &types = method.parameterTypes();

	QList<QByteArray> invalid_types;
	invalid_types << "const char*";
	for (int i = 0; i < types.size(); ++i)
	{
		if (invalid_types.contains(types[i]))
		{
			qWarning() << "DelayedSlotCaller::setSlot -> the target slot" << signature
				<< " cannot contains" << types[i];
			return;
		}
	}

	target = receiver;
	slot_to_call = slot;
	timer_id = startTimer(msecs);
}

void DelayedSlotCaller::addArgument(const char *arg)
{
	Q_ASSERT_X(arguments.size() < 9, "SlotCaller::addArgument", "Arguments list cannot exceed 10 elements");
	// The documentation of QVariant class says that if we call its constructor with
	// a const char* as argument it performs a deep copy of the string. However,
	// this seems to be not true. Another problem is that the type() returned by
	// the QVariant instance is not const char* but QString.
	arguments << QString(arg);
}

void DelayedSlotCaller::abort()
{
	if (timer_id != 1)
	{
		killTimer(timer_id);
		timer_id = -1;
		arguments.clear();
	}
}

void DelayedSlotCaller::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == timer_id)
	{
		callSlot();
		emit called();
		if (is_single_shot)
			killTimer(timer_id);
	}
}

QGenericArgument DelayedSlotCaller::arg(int index)
{
	// NOTE: the documentation says that we should not use directly the QGenericArgument class.
	// However, because we don't know the type of the arguments (we know it only as a string)
	// we cannot use the Q_ARG macro.
	if (index >= arguments.size())
		return QGenericArgument();

	// Because the QGenericArgument store only the pointers to the typename and to the constData we
	// need to maintain in life the real 'data' pointed, so we use a reference.
	const QVariant &v = arguments[index];

	return QGenericArgument(v.typeName(), v.constData());
}

void DelayedSlotCaller::callSlot()
{
	const QMetaObject *mo = target->metaObject();

	QByteArray signature = QMetaObject::normalizedSignature(slot_to_call + 1);
	QMetaMethod method = mo->method(mo->indexOfMethod(signature));

	const QList<QByteArray> &types = method.parameterTypes();

	for (int i = 0; i < types.size(); ++i)
	{
		// QVariant wraps various types in a "strange" way: a QVariant(QVariant(type))
		// has the same typeName of QVariant(type), so we exclude QVariant from the test.
		if (QByteArray(arguments[i].typeName()) != types[i] && types[i] != "QVariant")
		{
			qWarning() << "DelayedSlotCaller::callSlot -> the argument number"
				<< i + 1 << "of type" << QString(arguments[i].typeName())
				<< "does not match the signature of the slot" << signature;

			return;
		}
	}

	method.invoke(target, arg(0), arg(1), arg(2), arg(3), arg(4), arg(5), arg(6), arg(7), arg(8), arg(9));
}


