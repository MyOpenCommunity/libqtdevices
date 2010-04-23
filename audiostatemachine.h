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

#include "statemachine.h"


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

#if !defined(BT_HARDWARE_X11)
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
#endif
};


namespace bt_global { extern AudioStateMachine *audio_states; }

#endif // AUDIOSTATEMACHINE_H
