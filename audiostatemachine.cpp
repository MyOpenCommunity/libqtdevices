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


#include "audiostatemachine.h"
#include "hardware_functions.h" // initEchoCanceller


#include <QtGlobal>
#include <QMetaMethod>
#include <QtConcurrentRun>


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
	addState(IDLE,
		 SLOT(stateIdleEntered()),
		 SLOT(stateIdleExited()));
	addState(BEEP_ON,
		 SLOT(stateBeepOnEntered()),
		 SLOT(stateBeepOnExited()));
	addState(BEEP_OFF,
		 SLOT(stateBeepOffEntered()),
		 SLOT(stateBeepOffExited()));
	addState(PLAY_MEDIA_TO_SPEAKER,
		 SLOT(statePlayMediaToSpeakerEntered()),
		 SLOT(statePlayMediaToSpeakerExited()));
	addState(PLAY_FROM_DIFSON_TO_SPEAKER,
		 SLOT(statePlayFromDifsonToSpeakerEntered()),
		 SLOT(statePlayFromDifsonToSpeakerExited()));
	addState(PLAY_MEDIA_TO_DIFSON,
		 SLOT(statePlayMediaToDifsonEntered()),
		 SLOT(statePlayMediaToDifsonExited()));
	addState(PLAY_RINGTONE,
		 SLOT(statePlayRingtoneEntered()),
		 SLOT(statePlayRingtoneExited()));
	addState(SCS_VIDEO_CALL,
		 SLOT(stateScsVideoCallEntered()),
		 SLOT(stateScsVideoCallExited()));
	addState(SCS_INTERCOM_CALL,
		 SLOT(stateScsIntercomCallEntered()),
		 SLOT(stateScsIntercomCallExited()));
	addState(IP_VIDEO_CALL,
		 SLOT(stateIpVideoCallEntered()),
		 SLOT(stateIpVideoCallExited()));
	addState(IP_INTERCOM_CALL,
		 SLOT(stateIpIntercomCallEntered()),
		 SLOT(stateIpIntercomCallExited()));
	addState(ALARM_TO_SPEAKER,
		 SLOT(stateAlarmToSpeakerEntered()),
		 SLOT(stateAlarmToSpeakerExited()));
	addState(ALARM_TO_DIFSON,
		 SLOT(stateAlarmToDifsonEntered()),
		 SLOT(stateAlarmToDifsonExited()));
	addState(SCREENSAVER_WITH_PLAY,
		 SLOT(stateScreensaverWithPlayEntered()),
		 SLOT(stateScreensaverWithPlayExited()));

	// by default, all state transitions are possible, if this is not
	// correct, use removeTransitions() to make some transitions impossible

	// go to the start state: on startup IDLE is a transitional state, after a while it moves to BEEP_ON or BEEP_OFF
	start(IDLE);
}

void AudioStateMachine::start(int state)
{
	QtConcurrent::run(initEchoCanceller);
	StateMachine::start(state);
}

void AudioStateMachine::stateIdleEntered()
{
	// do something when entering the idle state
}

void AudioStateMachine::stateIdleExited()
{
	// do something when leaving the idle state
}

void AudioStateMachine::stateBeepOnEntered()
{

}

void AudioStateMachine::stateBeepOnExited()
{

}

void AudioStateMachine::stateBeepOffEntered()
{

}

void AudioStateMachine::stateBeepOffExited()
{

}

void AudioStateMachine::statePlayMediaToSpeakerEntered()
{

}

void AudioStateMachine::statePlayMediaToSpeakerExited()
{

}

void AudioStateMachine::statePlayFromDifsonToSpeakerEntered()
{

}

void AudioStateMachine::statePlayFromDifsonToSpeakerExited()
{

}

void AudioStateMachine::statePlayMediaToDifsonEntered()
{

}

void AudioStateMachine::statePlayMediaToDifsonExited()
{

}

void AudioStateMachine::statePlayRingtoneEntered()
{

}

void AudioStateMachine::statePlayRingtoneExited()
{

}

void AudioStateMachine::stateScsVideoCallEntered()
{

}

void AudioStateMachine::stateScsVideoCallExited()
{

}

void AudioStateMachine::stateScsIntercomCallEntered()
{

}

void AudioStateMachine::stateScsIntercomCallExited()
{

}

void AudioStateMachine::stateIpVideoCallEntered()
{

}

void AudioStateMachine::stateIpVideoCallExited()
{

}

void AudioStateMachine::stateIpIntercomCallEntered()
{

}

void AudioStateMachine::stateIpIntercomCallExited()
{

}

void AudioStateMachine::stateAlarmToSpeakerEntered()
{

}

void AudioStateMachine::stateAlarmToSpeakerExited()
{

}

void AudioStateMachine::stateAlarmToDifsonEntered()
{

}

void AudioStateMachine::stateAlarmToDifsonExited()
{

}

void AudioStateMachine::stateScreensaverWithPlayEntered()
{

}

void AudioStateMachine::stateScreensaverWithPlayExited()
{

}

