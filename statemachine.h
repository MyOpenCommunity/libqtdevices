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


#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <QObject>
#include <QList>
#include <QHash>


class StateMachine;


// allows to make the available transitions conditional on some external
// conditions
class TransitionConstraint
{
public:
	// called by isTransitiionAllowed(); return true to allow the transition
	// to be performed
	virtual bool isAllowed(const StateMachine *machine,
				   int source_state, int dest_state) = 0;
};


class StateMachine : public QObject
{
	// private
	struct State
	{
		int state;
		// signals are emitted after the state has been updated
		const char *entered;
		const char *exited;
		QList<TransitionConstraint *> constraints;
	};

Q_OBJECT
public:
	StateMachine();

	// must be called to initialize the state machine; does not call any
	// state callbacks
	virtual void start(int state);

	// returns the current state
	int currentState() const;

	// returns true if the transition from the current state to
	// new_state is a valid one
	bool isTransitionAllowed(int new_state) const;

	// moves the state machine to the given state; returns false if the
	// transition is disallowed by some constraint
	virtual bool toState(int state);

	// exits from the current state and moves to the previous one.
	void exitCurrentState();

	// removes the given state from the state stack; if the current state is
	// the given state, it is equivalent to exitCurrentState, otherwise the state
	// is removed from the state stack but no other actions are performed
	void removeState(int state);

	// adds a new state to the state machine; the optional entered and exited
	// parameters are signals/slots that will be called when the state is entered
	// or exited
	void addState(int state, const char *entered = NULL, const char *exited = NULL);

	// disallow any transition from the states in the source_states list to
	// dest_state
	void removeTransitions(const QList<int> &source_states, int dest_state);

	// a more general form for removeTransitions(): the given constraint will be
	// checked before performing a transition to dest_state
	void addTransitionConstraint(int dest_state, TransitionConstraint *constraint);

signals:
	// emitted when the state transition is complete, after the entered/exited
	// callbacks have been called
	void stateChanged(int new_state, int old_state);

protected:
	// returns the number of states
	int stateCount();

	// returns the state at the given position (0 is the bottom of the stack)
	int stateAt(int index);

	// inserts the state in the middle of the stack, without performing any
	// checks or emitting any signal; can't be used to push a state at the top
	// of the stack
	void insertState(int index, int state);

private:
	// the map of available states
	QHash<int, State> available_states;

	// the list of currently active states
	QList<int> active_states;

	void changeState(int new_state, int old_state);
};


#endif // STATEMACHINE_H
