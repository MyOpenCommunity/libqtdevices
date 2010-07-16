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

#define VOLUME_MIN 0
#define VOLUME_MAX 8
#define DEFAULT_VOLUME 3

namespace AudioStates
{
	/*
	 * BEEP_ON, PLAY_RINGTONE, PLAY_VDE_RINGTONE, PLAY_FLOORCALL, ALARM_TO_SPEAKER
	 *     have the same output path, but maybe different volumes
	 * IDLE: power off the local amplifier
	 * SCREENSAVER: like IDLE, only entered when there is a BEEP_ON state on the stack
	 * PLAY_FROM_DIFSON_TO_SPEAKER, PLAY_MEDIA_TO_DIFSON can be active at the same time
	 *     ALARM_TO_DIFSON is the same as the two states above
	 */
	enum
	{
		IDLE,
		BEEP_ON,
		MUTE,
		PLAY_MEDIA_TO_SPEAKER,
		PLAY_DIFSON,
		PLAY_RINGTONE,
		PLAY_VDE_RINGTONE,
		PLAY_FLOORCALL,
		SCS_VIDEO_CALL,
		SCS_INTERCOM_CALL,
		IP_VIDEO_CALL,
		IP_INTERCOM_CALL,
		ALARM_TO_SPEAKER,
		SCREENSAVER,
	};
}


class QTimer;


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
	AudioStateMachine();
	virtual void start(int state);

	// this override might insert the state in the middle of the state stack instea
	// of pushing it at the top; also, since the state change happens asynchronously,
	// you need to use the stateAboutToChange()/stateChanged() to be notified
	// when the transition completes
	virtual bool toState(int state);

	// Set and get the volume of the current state
	void setVolume(int value);
	int getVolume();

	// wether this touch can act as a sound diffusion source/amplifier
	bool isSource();
	bool isAmplifier();

	// the source does not have a volume yet
	void setLocalSourceStatus(bool status);
	bool getLocalSourceStatus();

	// the amplifier volume/status
	// note that the volume might be non-zero and the status true
	// even when the amplifier is off, if the amplifier is off
	// because the state machine is not PLAY_DIFSON
	int getLocalAmplifierVolume();
	void setLocalAmplifierVolume(int value);
	bool getLocalAmplifierStatus();
	void setLocalAmplifierStatus(bool status);
	void setMediaPlayerActive(bool active);
	void setMediaPlayerTemporaryPause(bool paused);

	bool isSoundDiffusionActive();

	// Call to notify watchers that some program (mplayer, sox, ...) is writing directly
	// to the dsp; there can only be one such program active at a given time
	void setDirectAudioAccess(bool status);
	bool isDirectAudioAccess();

signals:
	// notify that some program (mplayer, sox, ...) is writing directly to the dsp
	void directAudioAccessStarted();
	void directAudioAccessStopped();

#if !defined(BT_HARDWARE_X11)

protected:
	void changeState(int new_state, int old_state);

private:
	void manageMediaPlaybackStates();

private slots:
	// declare state handlers here
	// Events: Startup, Beep disabled
	void stateIdleEntered();
	void stateIdleExited();
	// Events: Beep enabled
	void stateBeepOnEntered();
	void stateBeepOnExited();
	// Events: Mute the microphone
	void stateMuteEntered();
	void stateMuteExited();
	// Events: Play media (mp3, wave. radio IP...) on local speaker
	void statePlayMediaToSpeakerEntered();
	void statePlayMediaToSpeakerExited();
	// Events: Play Diffusion Sound system (source, amplifier or both)
	void statePlayDifsonEntered();
	void statePlayDifsonExited();
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
	// Events: Screensaver activated, not playing anything
	void stateScreensaverEntered();
	void stateScreensaverExited();

	void saveVolumes();
	void completeStateChange();
	void forceStateChange();

private:
	QTimer *volumes_timer, *transition_guard;
	int current_audio_path, pending_old_state, pending_new_state;
#endif

private:
	bool is_source, is_amplifier;
	bool local_source_status, local_amplifier_status, media_player_status, media_player_temporary_pause;
	bool direct_audio_access;
};


namespace bt_global { extern AudioStateMachine *audio_states; }

#endif // AUDIOSTATEMACHINE_H
