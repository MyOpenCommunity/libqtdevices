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


#include "statemachine.h"

#include <QtGlobal>
#include <QMetaMethod>


// helper class, disallows transitions from the states in the list

class SourceStateConstraint : public TransitionConstraint
{
public:
	SourceStateConstraint(const QList<int> &states) {}

	virtual ~SourceStateConstraint() {}

	virtual bool isAllowed(const StateMachine *machine, int source_state, int dest_state)
	{
		return !states.contains(source_state);
	}

private:
	QList<int> states;
};


// StateMachine implementation

StateMachine::StateMachine()
{
}

void StateMachine::start(int state)
{
	active_states.clear();
	active_states.append(state);
}

int StateMachine::currentState() const
{
	return active_states.back();
}

void StateMachine::addState(int state, const char *entered, const char *exited)
{
	State s;

	s.state = state;
	s.entered = entered;
	s.exited = exited;

	Q_ASSERT_X(!available_states.contains(state), "StateMachine::addState",
		   "Duplicate state");

	available_states[state] = s;

	// TODO: check that entered/exited are signals/slots with the correct signature,
	//       abort if they are not
}

void StateMachine::addTransitionConstraint(int dest_state, TransitionConstraint *constraint)
{
	State &s = available_states[dest_state];

	s.constraints.append(constraint);
}

void StateMachine::removeTransitions(const QList<int> &source_states, int dest_state)
{
	addTransitionConstraint(dest_state, new SourceStateConstraint(source_states));
}

bool StateMachine::isTransitionAllowed(int state) const
{
	const State &s = available_states[state];

	foreach (TransitionConstraint *c, s.constraints)
	{
		if (!c->isAllowed(this, currentState(), state))
			return false;
	}

	return true;
}

bool StateMachine::toState(int state)
{
	if (state == currentState())
		return true;

	if (!isTransitionAllowed(state))
		return false;

	int old_state = currentState();
	active_states.append(state);
	changeState(state, old_state);
	return true;
}

void StateMachine::exitCurrentState()
{
	int curr_state = active_states.takeLast();
	changeState(currentState(), curr_state);
}

void StateMachine::changeState(int new_state, int old_state)
{
	const State &s = available_states[new_state];
	const State &os = available_states[old_state];


	const QMetaObject *mo = metaObject();

	// TODO: resolve the method when addState() is called

	if (os.exited)
	{
		QMetaMethod meth = mo->method(mo->indexOfMethod(QMetaObject::normalizedSignature(os.exited + 1)));

		meth.invoke(this, Q_ARG(int, new_state), Q_ARG(int, old_state));
	}

	if (s.entered)
	{
		QMetaMethod meth = mo->method(mo->indexOfMethod(QMetaObject::normalizedSignature(s.entered + 1)));

		meth.invoke(this, Q_ARG(int, new_state), Q_ARG(int, old_state));
	}

	emit stateChanged(new_state, old_state);
}

