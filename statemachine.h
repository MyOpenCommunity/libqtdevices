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


/*!
	\ingroup Core
	\brief Generic state machine

	The state is handled as a stack; new states are pushed on top of the state stack.
	When a new state is entered (or exited) the corresponding state change callback
	is called.

	For all state changes between different states, stateAboutToChange() is emitted
	before a state transition and stateChanged() after the transition completes.
	When pushing twice the same state on the stack, stateReentered() is emitted and
	no callbacks are run.  In all cases, stateTransition() is emitted after the state change.
 */
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

	/*!
		\brief Start the state machine.

		Clears the state stack and adds the given state; does not call any
		state callbacks
	 */
	virtual void start(int state);

	/*!
		\brief Returns the current state.
	 */
	int currentState() const;

	/*!
		\brief Chechs whether it's possible to go from the current state to \a new_state.
	 */
	bool isTransitionAllowed(int new_state) const;

	/*!
		\brief Changes to the given state.

		Adds the given state to the state stack if the state transition is allowed,
		returns false otherwise.
	 */
	virtual bool toState(int state);

	/*!
		\brief Removes a state from the state stack.

		If the current state is \a state, performs a full state transition to
		the state below it, otherwise the state is removed from the state stack
		but no other actions are performed.
	 */
	void removeState(int state);

	/*!
		\brief Add a new state to the state machine.

		Usually called in the constructor, adds another state to the state machine.
		The optional entered and exited parameters are signals/slots that will be
		called when the state is entered or exited.
	 */
	void addState(int state, const char *entered = NULL, const char *exited = NULL);

	/*!
		Disallow any transition from the states in \a source_states list to
		\a dest_state.
	 */
	void removeTransitions(const QList<int> &source_states, int dest_state);

	/*!
		A more general form for removeTransitions(): the given constraint will be
		checked before performing a transition to \a dest_state.
	 */
	void addTransitionConstraint(int dest_state, TransitionConstraint *constraint);

	/*!
		\brief True it the state stack contains \a state.
	 */
	bool contains(int state);

signals:
	/*!
		\brief Emitted before the transition between different states.
	 */
	void stateAboutToChange(int old_state);

	/*!
		\brief Emitted after the transition between different states.

	 */
	void stateChanged(int new_state, int old_state);

	/*!
		\brief Emitted when a state is reentered (it is pushed twice on the stack).

		\a new_state is guaranteed to be equal to \a old_state (it is passed twice to
		be compatible with stateChanged()).
	 */
	void stateReentered(int new_state, int old_state);

	/*!
		\brief Emitted for every state transition, regardless the two states are equal or not.
	 */
	void stateTransition(int new_state, int old_state);

protected:
	/*!
		\brief Number of states currently on the state stack.
	 */
	int stateCount();

	/*!
		Returns the state at the given position in the state stack (0 is the bottom of the stack).
	 */
	int stateAt(int index);

	/*!
		\brief Insert a state in the middle of the state stack.

		Inserts the state in the middle of the stack, without performing any
		checks or emitting any signal; can't be used to push a state at the top
		of the stack.
	 */
	void insertState(int index, int state);

	/*!
		\brief Called to perform the transition between different stacks.
	 */
	virtual void changeState(int new_state, int old_state);

	/*!
		\brief Call state transition callbacks when changing from \a old_state to \a new_state.
	 */
	void callStateCallbacks(int new_state, int old_state);

private:
	// the map of available states
	QHash<int, State> available_states;

	// the list of currently active states
	QList<int> active_states;

	// exits from the current state and moves to the previous one.
	void exitCurrentState();
};


#endif // STATEMACHINE_H
