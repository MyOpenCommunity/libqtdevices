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

using namespace AudioStates;


AudioStateMachine::AudioStateMachine()
{
	addState(IDLE);
	addState(BEEP_ON);
	addState(BEEP_OFF);
	addState(PLAY_MEDIA_TO_SPEAKER);
	addState(PLAY_FROM_DIFSON_TO_SPEAKER);
	addState(PLAY_MEDIA_TO_DIFSON);
	addState(PLAY_RINGTONE);
	addState(SCS_VIDEO_CALL);
	addState(SCS_INTERCOM_CALL);
	addState(IP_VIDEO_CALL);
	addState(IP_INTERCOM_CALL);
	addState(ALARM_TO_SPEAKER);
	addState(ALARM_TO_DIFSON);
	addState(SCREENSAVER_WITH_PLAY);

	start(IDLE);
}

void AudioStateMachine::start(int state)
{
	StateMachine::start(state);
}


AudioStateMachine *bt_global::audio_states = 0;


