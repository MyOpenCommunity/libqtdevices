#include "audiostatemachine.h"

#include <QtGlobal>
#include <QMetaMethod>


// helper class, disallows transitions from the states in the list

class SourceStateConstraint
	: public TransitionConstraint
{
public:
	SourceStateConstraint(const QList<int> &states)
	{
	}

	virtual bool isAllowed(const StateMachine *machine,
			       int source_state, int dest_state)
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

	const State &s = available_states[state];
	const State &os = available_states[currentState()];

	// TODO needs to be used like a stack
	active_states[0] = state;

	const QMetaObject *mo = metaObject();

	// TODO: resolve the method when addState() is called

	if (os.exited)
	{
		QMetaMethod meth = mo->method(mo->indexOfMethod(QMetaObject::normalizedSignature(os.exited + 1)));

		meth.invoke(this, Q_ARG(int, state), Q_ARG(int, os.state));
	}

	if (s.entered)
	{
		QMetaMethod meth = mo->method(mo->indexOfMethod(QMetaObject::normalizedSignature(s.entered + 1)));

		meth.invoke(this, Q_ARG(int, state), Q_ARG(int, os.state));
	}

	emit stateChanged(state, os.state);

	return true;
}


// AudioStateMachine implementation

AudioStateMachine::AudioStateMachine()
{
	addState(ALARM_CLOCK,
		 SLOT(stateAlarmClockEntered()),
		 SLOT(stateAlarmClockExited()));
	addState(VIDEO_CALL,
		 SLOT(stateVideoCallEntered()));

	// by default, all state transitions are possible, if this is not
	// correct, use removeTransitions() to make some transitions impossible

	// go to the start state
	start(ALARM_CLOCK);
}

void AudioStateMachine::stateAlarmClockEntered(int state, int old_state)
{
	// do something when entering the alarm clock state
}

void AudioStateMachine::stateAlarmClockExited(int state, int old_state)
{
	// do something when leaving the alarm clock state
}

void AudioStateMachine::stateVideoCallEntered()
{
	// do something when entering the video call state
}
