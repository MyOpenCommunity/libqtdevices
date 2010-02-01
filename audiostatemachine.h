#ifndef AUDIOSTATEMACHINE_H
#define AUDIOSTATEMACHINE_H

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


class StateMachine
	: public QObject
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
	void start(int state);

	// returns the current state
	int currentState() const;

	// returns true if the transition from the current state to
	// new_state is a valid one
	bool isTransitionAllowed(int new_state) const;

	// moves the state machine to the given state; returns false if the
	// transition is disallowed by some constraint
	bool toState(int state);

	void exitCurrentState();

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

private:
	// the map of available states
	QHash<int, State> available_states;

	// the list of currently active states
	QList<int> active_states;
};


// state machine for audio output
//
// to add a new state:
// - add a new value to the enum below
// - add the enter/exit handlers to the 'private slots' section
// - call addState() in the constructor to register the state in the
//   state machine
class AudioStateMachine
	: public StateMachine
{
Q_OBJECT
public:
	enum
	{
		ALARM_CLOCK,
		VIDEO_CALL,
	};

	AudioStateMachine();

private slots:
	// declare state handlers here
	void stateAlarmClockEntered(int state, int old_state);
	void stateAlarmClockExited(int state, int old_state);
	void stateVideoCallEntered();
};

#endif // AUDIOSTATEMACHINE_H
