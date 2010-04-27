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
#include "hardware_functions.h" // DEV_E2

#include <QtConcurrentRun>
#include <QProcess>

#include <fcntl.h> // open
#include <unistd.h> // usleep

#define E2_BASE_CONF_ZARLINK 11694
#define ZARLINK_KEY 0x63


namespace
{
	bool silentExecute(const QString &program, QStringList args = QStringList())
	{
		args << "> /dev/null" << "2>&1";
		return QProcess::execute(program, args);
	}

	// Init the echo canceller, updating (if needed) the configuration. This function
	// MUST be called in a separate thread, in order to avoid the freeze of the ui.
	void initEchoCanceller()
	{
		char init = 0;
		bool need_reset = false;

		int eeprom = open(DEV_E2, O_RDWR | O_SYNC, 0666);
		lseek(eeprom, E2_BASE_CONF_ZARLINK, SEEK_SET);
		read(eeprom, &init, 1);

		if (init != ZARLINK_KEY) // different versions, update the zarlink configuration
		{
			for (int i = 0; i < 5; ++i)
			{
				if (!silentExecute("/home/bticino/bin/zarlink 0400 0001 CONF /home/bticino/cfg/zle38004.cr"))
				{
					init = ZARLINK_KEY;
					lseek(eeprom, E2_BASE_CONF_ZARLINK, SEEK_SET);
					write(eeprom, &init, 1);
					need_reset = true;
					break;
				}
				usleep(500000);
			}
		}

		silentExecute(QString("echo %1 > /home/bticino/cfg/vers_conf_zarlink").arg(init));
		if (need_reset)
		{
			silentExecute("echo 0 > /proc/sys/dev/btweb/reset_ZL1");
			usleep(100000);
			silentExecute("echo 1 > /proc/sys/dev/btweb/reset_ZL1");
		}
	}

	void activateVCTAudio()
	{
		QProcess::startDetached("/bin/in_scsbb_on");
	}

	void disactivateVCTAudio()
	{
		QProcess::startDetached("/bin/in_scsbb_off");
	}
}


using namespace AudioStates;

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
	activateVCTAudio();
}

void AudioStateMachine::stateScsVideoCallExited()
{
	disactivateVCTAudio();
}

void AudioStateMachine::stateScsIntercomCallEntered()
{
	activateVCTAudio();
}

void AudioStateMachine::stateScsIntercomCallExited()
{
	disactivateVCTAudio();
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

AudioStateMachine *bt_global::audio_states = 0;

