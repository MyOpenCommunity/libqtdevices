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
#include "main.h" // bt_global::config

using namespace AudioStates;


AudioStateMachine::AudioStateMachine()
{
	addState(IDLE);
	addState(BEEP_ON);
	addState(PLAY_MEDIA_TO_SPEAKER);
	addState(PLAY_DIFSON);
	addState(PLAY_RINGTONE);
	addState(SCS_VIDEO_CALL);
	addState(SCS_INTERCOM_CALL);
	addState(IP_VIDEO_CALL);
	addState(IP_INTERCOM_CALL);
	addState(ALARM_TO_SPEAKER);
	addState(SCREENSAVER);

	start(IDLE);
}

void AudioStateMachine::start(int state)
{
	is_source = !(*bt_global::config)[SOURCE_ADDRESS].isEmpty();
	is_amplifier = !(*bt_global::config)[AMPLIFIER_ADDRESS].isEmpty();
	StateMachine::start(state);
}

bool AudioStateMachine::toState(int state)
{
	return StateMachine::toState(state);
}

void AudioStateMachine::setVolume(int)
{

}

int AudioStateMachine::getVolume()
{
	return DEFAULT_VOLUME;
}

bool AudioStateMachine::isSource()
{
	return is_source;
}

bool AudioStateMachine::isAmplifier()
{
	return is_amplifier;
}

void AudioStateMachine::setLocalAmplifierStatus(bool status)
{
	local_amplifier_status = status;
}

bool AudioStateMachine::getLocalAmplifierStatus()
{
	return local_amplifier_status;
}

void AudioStateMachine::setLocalAmplifierVolume(int volume)
{
}

int AudioStateMachine::getLocalAmplifierVolume()
{
	return DEFAULT_VOLUME;
}

void AudioStateMachine::setLocalSourceStatus(bool status)
{
	local_source_status = status;
}

bool AudioStateMachine::getLocalSourceStatus()
{
	return local_source_status;
}

void AudioStateMachine::setMediaPlayerActive(bool active)
{
	media_player_status = active;
}

void AudioStateMachine::setMediaPlayerTemporaryPause(bool paused)
{

}

bool AudioStateMachine::isSoundDiffusionActive()
{
	return getLocalAmplifierStatus() || getLocalSourceStatus() || media_player_status;
}

void AudioStateMachine::setDirectAudioAccess(bool status)
{
}

bool AudioStateMachine::isDirectAudioAccess()
{
       return false;
}

AudioStateMachine *bt_global::audio_states = 0;


