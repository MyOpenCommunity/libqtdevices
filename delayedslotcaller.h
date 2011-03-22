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
	The execution of the slot can be stopped calling the abort() method.

	Limitations:
	- the slot to call cannot contains an argument of type "const char*"
	- the slot can have a maximum of 10 arguments.
*/
class DelayedSlotCaller : public QObject
{
Q_OBJECT
public:
	DelayedSlotCaller(QObject *parent = 0, bool single_shot = true);

	void setSlot(QObject *receiver, const char *slot, int msecs);
	void addArgument(const QVariant &arg);
	void addArgument(const char* arg);
	void abort();

protected:
	void timerEvent(QTimerEvent *event);

private slots:
	void callSlot();

private:
	const char *slot_to_call;
	QObject *target;
	QList<QVariant> arguments;
	int timer_id;
	bool is_single_shot;

	QGenericArgument arg(int index);
};


#endif
