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
class AudioStateMachine : public StateMachine
{
Q_OBJECT
public:
	enum
	{
		IDLE,
		BEEP_ON,
		BEEP_OFF,
		PLAY_MEDIA_TO_SPEAKER,
		PLAY_FROM_DIFSON_TO_SPEAKER,
		PLAY_MEDIA_TO_DIFSON,
		PLAY_RINGTONE,
		SCS_VIDEO_CALL,
		SCS_INTERCOM_CALL,
		IP_VIDEO_CALL,
		IP_INTERCOM_CALL,
		ALARM_TO_SPEAKER,
		ALARM_TO_DIFSON,
		SCREENSAVER_WITH_PLAY,
		//SCREENSAVER_WITHOUT_PLAY --> IDLE
	};

	AudioStateMachine();

	virtual void start(int state);

private slots:
	// declare state handlers here
	// Events: Startup, Screensaver without password, Screen OFF
	void stateIdleEntered();
	void stateIdleExited();
	// Events: Beep enabled
	void stateBeepOnEntered();
	void stateBeepOnExited();
	// Events: Beep disabled
	void stateBeepOffEntered();
	void stateBeepOffExited();
	// Events: Play media (mp3, wave. radio IP...) on local speaker
	void statePlayMediaToSpeakerEntered();
	void statePlayMediaToSpeakerExited();
	// Events: Play Diffusion Sound system on local speaker
	void statePlayFromDifsonToSpeakerEntered();
	void statePlayFromDifsonToSpeakerExited();
	// Events: Play media (mp3, wave. radio IP...) on diffusione sound system - set Touch as multimedia source
	void statePlayMediaToDifsonEntered();
	void statePlayMediaToDifsonExited();
	// Events: Play ringtone on local speaker, incoming video call or intercom
	void statePlayRingtoneEntered();
	void statePlayRingtoneExited();
	// Events: Answer on an incoming scs video call
	void stateScsVideoCallEntered();
	void stateScsVideoCallExited();
	// Events: Answer on an incoming scs intercom call
	void stateScsIntercomCallEntered();
	void stateScsIntercomCallExited();
	// Events: Answer on an incoming IP video call
	void stateIpVideoCallEntered();
	void stateIpVideoCallExited();
	// Events: Answer on an incoming IP intercom call
	void stateIpIntercomCallEntered();
	void stateIpIntercomCallExited();
	// Events: Play Alarm on local speaker
	void stateAlarmToSpeakerEntered();
	void stateAlarmToSpeakerExited();
	// Events: Play Alarm on sound diffusion system
	// Maybe obsolete, can be used one of: statePlayMediaToSpeakerEntered, statePlayFromDifsonToSpeakerEntered, statePlayMediaToDifsonEntered
	void stateAlarmToDifsonEntered();
	void stateAlarmToDifsonExited();
	// Events: Activate Screensaver during an interaction with diffusion sound system
	void stateScreensaverWithPlayEntered();
	void stateScreensaverWithPlayExited();
};


namespace bt_global { extern AudioStateMachine *audio_states; }

#endif // AUDIOSTATEMACHINE_H
