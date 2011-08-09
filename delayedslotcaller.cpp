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
#include <QTimerEvent>
#include <QTimer>
#include <QDebug>


DelayedSlotCaller::DelayedSlotCaller(bool single_shot, ArgumentsCheck check_arguments)
{
	timer_id = -1;
	is_single_shot = single_shot;
	target = 0;
	check = check_arguments;
}

DelayedSlotCaller::~DelayedSlotCaller()
{
	reset();
}

void DelayedSlotCaller::reset()
{
	for (int i = 0; i < arguments.size(); ++i)
		delete arguments[i];

	arguments.clear();
	timer_id = -1;
	target = 0;
}

void DelayedSlotCaller::setSlot(QObject *receiver, const char *slot, int msecs)
{
	Q_ASSERT_X(timer_id == -1, "DelayedSlotCaller::setSlot",
		qPrintable(QString("Call with an already defined slot %1").arg(method_to_call.signature())));
	const QMetaObject *mo = receiver->metaObject();

	QByteArray signature = QMetaObject::normalizedSignature(slot + 1);
	int index = mo->indexOfMethod(signature);
	if (index == -1)
	{
		qWarning() << "DelayedSlotCaller::setSlot -> no such slot" << signature;
		return;
	}

	method_to_call = mo->method(index);
	target = receiver;
	timer_id = startTimer(msecs);
}

void DelayedSlotCaller::addArgument(const char *arg)
{
	Q_ASSERT_X(arguments.size() < 9, "SlotCaller::addArgument", "Arguments list cannot exceed 10 elements");

	const QList<QByteArray> &types = method_to_call.parameterTypes();

	if (types[arguments.size()]== "QString")
		arguments << new ArgContainer<QString>(QString(arg));
	else
		arguments << new ArgContainer<const char*>(arg);
}

void DelayedSlotCaller::abort()
{
	if (timer_id != 1)
	{
		killTimer(timer_id);
		reset();
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
	if (index >= arguments.size())
		return QGenericArgument();

	return arguments[index]->getArgument();
}

void DelayedSlotCaller::callSlot()
{
	if (check == CHECK)
	{
		const QList<QByteArray> &types = method_to_call.parameterTypes();

		if (arguments.size() != types.size())
		{
			qWarning() << "DelayedSlotCaller::callSlot -> the arguments number does not"
					<< "match the signature of the slot" << method_to_call.signature();
			return;
		}

		for (int i = 0; i < types.size(); ++i)
		{
			// QVariant wraps various types in a "strange" way: a QVariant(QVariant(type))
			// has the same typeName of QVariant(type), so we exclude QVariant from the test.
			if (arguments[i]->getTypeName() != types[i] && types[i] != "QVariant")
			{
				qWarning() << "DelayedSlotCaller::callSlot -> the argument number"
					<< i + 1 << "of type" << QString(arguments[i]->getTypeName())
					<< "does not match the signature of the slot" << method_to_call.signature();

				return;
			}
		}
	}

	method_to_call.invoke(target, arg(0), arg(1), arg(2), arg(3), arg(4), arg(5), arg(6), arg(7), arg(8), arg(9));
}


