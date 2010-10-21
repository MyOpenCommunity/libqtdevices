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

/*!
	\brief States for AudioStateMachine
 */
namespace AudioStates
{
	enum
	{
		IDLE,                  /*!< Base state, always on the stack (power off local amplifier). */
		BEEP_ON,               /*!< Beep active (local amplifier on). */
		MUTE,                  /*!< Videocall mute state. */
		PLAY_MEDIA_TO_SPEAKER, /*!< Reproduce a media file using local speaker (no sound diffusion). */
		PLAY_DIFSON,           /*!< Act as a sound diffusion source and/or amplifier. */
		PLAY_RINGTONE,         /*!< Play a ringtone locally (used by ringtone settings page). */
		PLAY_VDE_RINGTONE,     /*!< Play a ringtone during a videocall/intercom call. */
		PLAY_FLOORCALL,        /*!< Play a ringtone during a fllor call. */
		SCS_VIDEO_CALL,        /*!< SCS videocall active (use answered the call). */
		SCS_INTERCOM_CALL,     /*!< SCS intercom active (use answered the call). */
		IP_VIDEO_CALL,         /*!< IP videocall active (use answered the call). */
		IP_INTERCOM_CALL,      /*!< IP intercom active (use answered the call). */
		ALARM_TO_SPEAKER,      /*!< Alarm clock with ringtone. */
		SCREENSAVER,           /*!< Screensaver/screen off is active (power off local amplifier). */
	};
}


class QTimer;


/*!
	\ingroup SoundDiffusion
	\brief State manager for audio output paths.

	When a state is entered the state machine runs all the programs required
	to set up the output audio path and notifies interested parties when a state is about to
	change and when the state transition is complete.

	The audio state machine is organized as a stack of states; however states have different
	priorities so when a state is added to the stack (calling the toState() method) it might not
	be put at the top of the stack and hence will not become active until all higher-priority
	states have been removed from the stack.

	Some audio states (AudioStates::PLAY_MEDIA_TO_SPEAKER, AudioStates::PLAY_DIFSON) are tricky because they
	are entered when either the local source/amplifier is active or the media player is reproducing, and they
	also depend on whether the BTouch is configured as a sound diffusion source/amplifier.  These two stated are
	added/removed to the state stack after calls to setLocalAmplifierStatus(), setMediaPlayerActive() and setLocalSourceStatus().

	An additional complication is that not all state transition are synchronous: it's always good practice to call toState()
	to start the transition and complete the work after receiving the stateChanged() signal.

	When a program is accessing the	audio output device, it's necessary to wait for direct audio access to stop before
	performing the state transition; it's the responsibility of classes that access the audio device to pause/stop playback
	when receiving the stateAboutToChange() signal and to call setDirectAudioAccess() when starting/stopping/pausing playback.
	It's vital that setDirectAudioAccess() is called with a \c false argument only after playback actually stops.

	\section AudioStateMachine-newstate Adding a new state

	To add a new state:
	\li add a new value to the enum below
	\li add the enter/exit handlers to the 'private slots' section
	\li call addState() in the constructor to register the state in the state machine
	\li modify toState() if it is necessary to add the state in a specific place in the state stack
 */
class AudioStateMachine : public StateMachine
{
Q_OBJECT
public:
	AudioStateMachine();

	/*! \brief Called by the contructor. */
	virtual void start(int state);

	/*!
		This override might insert the state in the middle of the state stack instead
		of pushing it at the top; also, since state changes might happen asynchronously,
		use stateAboutToChange()/stateChanged() to be notified when the transition completes.
	 */
	virtual bool toState(int state);

	/*!
		\brief Set the physical volume for the current audio state.
	 */
	void setVolume(int value);

	/*!
		\brief Read the physical volume for the current audio state.
	 */
	int getVolume();

	/*!
		\brief Whether the BTouch is configured to act as a sound diffusion source.
	 */
	bool isSource();

	/*!
		\brief Whether the BTouch is configured to act as a sound diffusion amplifier.
	 */
	bool isAmplifier();

	/*!
		\brief Set the local source status.

		Changing local source status might add/remove AudioStates::PLAY_DIFSON state to the
		state stack.  Must never be called unless isSource() is \c true.
	 */
	void setLocalSourceStatus(bool status);

	/*!
		\brief Whether the local source is currently active.
	 */
	bool getLocalSourceStatus();

	/*!
		\brief Local amplifier volume value.
	 */
	int getLocalAmplifierVolume();

	/*!
		\brief Set local amplifier volume value (1-31).
	 */
	void setLocalAmplifierVolume(int value);

	/*!
		\brief Whether local amplifier is active.

		This function might return \c true when the physical amplifier is off
		because the
	 */
	bool getLocalAmplifierStatus();

	/*!
		\brief Set local amplifier active status.

		Changing local amplifier status might add/remove AudioStates::PLAY_DIFSON
		state to the state stack.  Must never be called unless isAmplifier() is \c true.
	 */
	void setLocalAmplifierStatus(bool status);

	/*!
		\brief Set amplifier to temporary off mode.

		Switch off the local amplifier without changing sound diffusion state.
		Used when handling the "temporary off" command for the local amplifier.

		\see VirtualAmplifierDevice
	 */
	void setLocalAmplifierTemporaryOff(bool off);

	/*!
		\brief Set local media player active status.

		Changing local source status might add/remove AudioStates::PLAY_DIFSON or
		AudioStates::PLAY_MEDIA_TO_SPEAKER state to the state stack
		(depending on isSource() value).

		Must be called when the player is started/stopped/paused.
	 */
	void setMediaPlayerActive(bool active);

	/*!
		Must be called when the media player is paused because of an audio
		state machine change, and will be restarted when the audio state machine
		reenters the correct state (setMediaPlayerActive() is \c false both for
		normal pause and for temporary pause).
	 */
	void setMediaPlayerTemporaryPause(bool paused);

	/*!
		Returns whether AudioStates::PLAY_MEDIA_TO_SPEAKER or AudioStates::PLAY_DIFSON should
		be present in the state stack.
	 */
	bool isSoundDiffusionActive();

	/*!
		\brief Update direct audio access state

		Call to notify watchers that some program (mplayer, sox, ...) is writing directly
		to the audio output device; there can only be one such program active at a given time.

		Emits directAudioAccessStarted() / directAudioAccessStopped().

		setDirectAudioAccess(true) must be called before audio playback actually starts and
		setDirectAudioAccess(false) must be called after playback stops (not after sending the stop
		command to the player).
	 */
	void setDirectAudioAccess(bool status);

	/*!
		\brief Returns whether some program is accessing the audio output.
	 */
	bool isDirectAudioAccess();

signals:
	/*!
		\brief Emitted when a program starts writing to the output device.
	 */
	void directAudioAccessStarted();

	/*!
		\brief Emitted after a program stops writing to the output device.
	 */
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
	bool local_source_status, local_amplifier_status, local_amplifier_temporary_off;
	bool media_player_status, media_player_temporary_pause;
	bool direct_audio_access;
};


namespace bt_global { extern AudioStateMachine *audio_states; }

#endif // AUDIOSTATEMACHINE_H
