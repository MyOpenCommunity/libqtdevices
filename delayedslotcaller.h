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

#ifndef DELAYED_SLOT_CALLER_H
#define DELAYED_SLOT_CALLER_H

#include <QObject>
#include <QList>
#include <QVariant>

class QGenericArgument;

/*!
	\ingroup Core
	\brief Calls a slot with or without arguments after a delay.

	This class can be used to overtake the limitation of a timer which can call
	only slot without arguments.
	After build the instance, set the slot to call and the delay calling the
	method setSlot() and add the arguments with the addArgument().
	The execution of the slot can be stopped calling the abort() method. The signal
	called() is emitted every time that the slot has been called.

	Limitations:
	- the slot to call cannot contains an argument of type "const char*"
	- the slot can have a maximum of 10 arguments.

	\note The slot can contains custom types, that have to be register with the
	Q_DECLARE_METATYPE macro. Unfortunately, the macro doesn't work with template
	types that have more than one template arguments. To overcome this limitation,
	you can define a typedef of the type and use it in both the method signature
	and in the addArgument() call.
*/
class DelayedSlotCaller : public QObject
{
Q_OBJECT
public:
	DelayedSlotCaller(bool single_shot = true);

	/*!
		\brief Set the \a slot to call after \a msec milliseconds in the \a receiver object.
	*/
	void setSlot(QObject *receiver, const char *slot, int msecs);

	/*!
		\brief Add \a arg to the list of the arguments passed to the slot called.
	*/
	template <class T> void addArgument(const T &arg);

	void addArgument(const char* arg);

	/*!
		\brief Stop the execution of the slot.
	*/
	void abort();

signals:
	/*!
		\brief Notify that the target slot has been called.
	*/
	void called();

protected:
	void timerEvent(QTimerEvent *event);

private slots:
	void callSlot();

private:
	const char *slot_to_call;
	QObject *target;
	int timer_id;
	bool is_single_shot;

	QList<QVariant> arguments;

	QGenericArgument arg(int index);
};


template <class T>  void DelayedSlotCaller::addArgument(const T &arg)
{
	Q_ASSERT_X(arguments.size() < 9, "SlotCaller::addArgument", "Arguments list cannot exceed 10 elements");
	arguments << QVariant::fromValue(arg);
}

#endif
