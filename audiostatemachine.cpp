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
#include <QTimer>

#include <fcntl.h> // open
#include <unistd.h> // usleep, read, write


// The address used to save information in the e2 memory.
#define E2_BASE_CONF_ZARLINK 11694
#define E2_BASE_INIT 11671
#define E2_BASE_VOLUMES 11685

// The keys used to update the e2, the eeprom memory where is stored the zarlink
// configuration and the volumes.
#define ZARLINK_KEY 0x63
#define VOLUME_KEY 0x5b

#define VOLUME_TIMER_SECS 5

#define MM_SOURCE_VOLUME 3

namespace Volumes
{
	enum Type
	{
		VIDEODOOR = 0,
		INTERCOM,
		MM_LOCALE,
		BEEP,
		RINGTONES,
		FILE,
		VCTIP,
		MICROPHONE,
		MM_SOURCE,
		MM_AMPLIFIER,
		COUNT // an invalid element, used only to calculate the size
	};
}


namespace
{
	// The global container for volumes.
	QByteArray volumes(Volumes::COUNT, DEFAULT_VOLUME);

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
		if (eeprom == -1)
		{
			qWarning() << "Unable to open E2 device";
			return;
		}
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

	void initVolumes()
	{
		char init = 0;
		int eeprom = open(DEV_E2, O_RDWR | O_SYNC, 0666);
		if (eeprom == -1)
		{
			qWarning() << "Unable to open E2 device";
			return;
		}
		lseek(eeprom, E2_BASE_INIT, SEEK_SET);
		read(eeprom, &init, 1);

		if (init != VOLUME_KEY) // reset volumes and update the e2
		{
			init = VOLUME_KEY;

			lseek(eeprom, E2_BASE_INIT, SEEK_SET);
			write(eeprom, &init, 1);

			lseek(eeprom, E2_BASE_VOLUMES, SEEK_SET);
			write(eeprom, volumes.data(), Volumes::COUNT);
		}
		else
		{
			lseek(eeprom, E2_BASE_VOLUMES, SEEK_SET);
			read(eeprom, volumes.data(), Volumes::COUNT);

			// We check if all the values read from the hardware is in the admitted range.
			for (int i = 0;  i < Volumes::COUNT; ++i)
			{
				int volume_read = volumes.at(i);
				if (volume_read <= VOLUME_MIN || volume_read >= VOLUME_MAX)
					volumes[i] = DEFAULT_VOLUME;
			}
		}
	}

	void activateVCTAudio()
	{
		QProcess::execute("/bin/in_scsbb_on");
	}

	void disactivateVCTAudio()
	{
		QProcess::execute("/bin/in_scsbb_off");
	}

	void changeVolumePath(Volumes::Type type, int value)
	{
		Q_ASSERT_X(value >= VOLUME_MIN && value <= VOLUME_MAX, "changeVolumePath",
			qPrintable(QString("Volume value %1 out of range for audio path %2!").arg(value).arg(type)));

		qDebug() << "/home/bticino/bin/set_volume" << type + 1 << value;
		// Volumes for the script set_volume starts from 1, so we add it.
		QProcess::execute("/home/bticino/bin/set_volume",
				  QStringList() << QString::number(type + 1) << QString::number(value));
	}

	void changeVolumePath(Volumes::Type type)
	{
		changeVolumePath(type, volumes.at(type));
	}

	void saveVolumes()
	{
		int eeprom = open(DEV_E2, O_RDWR | O_SYNC, 0666);
		if (eeprom == -1)
		{
			qWarning() << "Unable to open E2 device";
			return;
		}
		lseek(eeprom, E2_BASE_VOLUMES, SEEK_SET);
		write(eeprom, volumes.data(), Volumes::COUNT);
	}
}


using namespace AudioStates;


AudioStateMachine::AudioStateMachine()
{
	// To avoid useless write in the eeprom memory, we use a timer to 'compress'.
	volumes_timer = new QTimer(this);
	volumes_timer->setInterval(VOLUME_TIMER_SECS * 1000);
	connect(volumes_timer, SIGNAL(timeout()), SLOT(saveVolumes()));

	current_audio_path = -1;

	addState(IDLE,
		 SLOT(stateIdleEntered()),
		 SLOT(stateIdleExited()));
	addState(BEEP_ON,
		 SLOT(stateBeepOnEntered()),
		 SLOT(stateBeepOnExited()));
	addState(BEEP_OFF,
		 SLOT(stateBeepOffEntered()),
		 SLOT(stateBeepOffExited()));
	addState(MUTE,
		 SLOT(stateMuteEntered()),
		 SLOT(stateMuteExited()));
	addState(PLAY_MEDIA_TO_SPEAKER,
		 SLOT(statePlayMediaToSpeakerEntered()),
		 SLOT(statePlayMediaToSpeakerExited()));
	addState(PLAY_DIFSON,
		 SLOT(statePlayDifsonEntered()),
		 SLOT(statePlayDifsonExited()));
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
	addState(SCREENSAVER_WITH_PLAY,
		 SLOT(stateScreensaverWithPlayEntered()),
		 SLOT(stateScreensaverWithPlayExited()));
	addState(SCREENSAVER_WITHOUT_PLAY,
		 SLOT(stateScreensaverWithoutPlayEntered()),
		 SLOT(stateScreensaverWithoutPlayExited()));

	// by default, all state transitions are possible, if this is not
	// correct, use removeTransitions() to make some transitions impossible

	// go to the start state: on startup IDLE is a transitional state, after a while it moves to BEEP_ON or BEEP_OFF
	start(IDLE);
}

void AudioStateMachine::start(int state)
{
	QtConcurrent::run(initEchoCanceller);
	local_source_status = local_amplifier_status = false;
	initVolumes();
	StateMachine::start(state);
}

void AudioStateMachine::saveVolumes()
{
	::saveVolumes();
	volumes_timer->stop();
}

void AudioStateMachine::setLocalAmplifierStatus(bool status)
{
	local_amplifier_status = status;
	if (currentState() == AudioStates::PLAY_DIFSON)
	{
		if (status)
			changeVolumePath(Volumes::MM_AMPLIFIER, volumes[Volumes::MM_AMPLIFIER]);
		else
			changeVolumePath(Volumes::MM_AMPLIFIER, 0);
	}
}

bool AudioStateMachine::getLocalAmplifierStatus()
{
	return local_amplifier_status;
}

void AudioStateMachine::setLocalAmplifierVolume(int volume)
{
	volumes[Volumes::MM_AMPLIFIER] = volume;
	if (local_amplifier_status && currentState() == AudioStates::PLAY_DIFSON)
		changeVolumePath(Volumes::MM_AMPLIFIER, volumes[Volumes::MM_AMPLIFIER]);
}

int AudioStateMachine::getLocalAmplifierVolume()
{
	return volumes[Volumes::MM_AMPLIFIER];
}

void AudioStateMachine::setLocalSourceStatus(bool status)
{
	local_source_status = status;
	if (currentState() == AudioStates::PLAY_DIFSON)
	{
		if (status)
			changeVolumePath(Volumes::MM_SOURCE, MM_SOURCE_VOLUME);
		else
			changeVolumePath(Volumes::MM_SOURCE, 0);
	}
}

bool AudioStateMachine::getLocalSourceStatus()
{
	return local_source_status;
}

bool AudioStateMachine::isSoundDiffusionActive()
{
	return getLocalAmplifierStatus() || getLocalSourceStatus();
}

void AudioStateMachine::setVolume(int value)
{
	Q_ASSERT_X(value >= VOLUME_MIN && value <= VOLUME_MAX, "AudioStateMachine::setVolume",
		qPrintable(QString("Volume value %1 out of range for audio path %2!").arg(value).arg(bt_global::audio_states->current_audio_path)));

	int audio_path = bt_global::audio_states->current_audio_path;

	Q_ASSERT_X(audio_path != -1, "AudioStateMachine::setVolume", "setVolume called without set an audio path!");
	volumes[audio_path] = value;
	changeVolumePath(static_cast<Volumes::Type>(audio_path));
	volumes_timer->start();
}

int AudioStateMachine::getVolume()
{
	Q_ASSERT_X(current_audio_path != -1, "AudioStateMachine::getVolume",
		"You must set the current audio path before getting the current volume value!");

	return volumes.at(current_audio_path);
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
	current_audio_path = Volumes::BEEP;
	changeVolumePath(Volumes::BEEP);
}

void AudioStateMachine::stateBeepOnExited()
{

}

void AudioStateMachine::stateBeepOffEntered()
{
	// TODO turn off the amplifier
}

void AudioStateMachine::stateBeepOffExited()
{
	// TODO turn back on the amplfier
}

void AudioStateMachine::statePlayMediaToSpeakerEntered()
{
	qDebug() << "AudioStateMachine" << "play media to speaker state";

	current_audio_path = Volumes::MM_LOCALE;
	changeVolumePath(Volumes::MM_LOCALE);
}

void AudioStateMachine::statePlayMediaToSpeakerExited()
{

}

void AudioStateMachine::statePlayDifsonEntered()
{
	qDebug() << "AudioStateMachine" << "play sound diffusion state";

	if (local_amplifier_status)
	{
		current_audio_path = Volumes::MM_AMPLIFIER;
		changeVolumePath(Volumes::MM_AMPLIFIER);
	}

	if (local_source_status)
	{
		current_audio_path = Volumes::MM_SOURCE;
		changeVolumePath(Volumes::MM_SOURCE);
	}
}

void AudioStateMachine::statePlayDifsonExited()
{
	changeVolumePath(Volumes::MM_AMPLIFIER, 0);
	changeVolumePath(Volumes::MM_SOURCE, 0);
}

void AudioStateMachine::statePlayRingtoneEntered()
{
	current_audio_path = Volumes::RINGTONES;
	changeVolumePath(Volumes::RINGTONES);
}

void AudioStateMachine::statePlayRingtoneExited()
{

}

void AudioStateMachine::stateScsVideoCallEntered()
{
	activateVCTAudio();
	current_audio_path = Volumes::VIDEODOOR;
	changeVolumePath(Volumes::VIDEODOOR);
}

void AudioStateMachine::stateScsVideoCallExited()
{
	disactivateVCTAudio();
}

void AudioStateMachine::stateScsIntercomCallEntered()
{
	activateVCTAudio();
	current_audio_path = Volumes::INTERCOM;
	changeVolumePath(Volumes::INTERCOM);
}

void AudioStateMachine::stateScsIntercomCallExited()
{
	disactivateVCTAudio();
}

void AudioStateMachine::stateMuteEntered()
{
	current_audio_path = Volumes::MICROPHONE;
	setVolume(0);
}

void AudioStateMachine::stateMuteExited()
{
	setVolume(1);
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
	current_audio_path = Volumes::RINGTONES;
	changeVolumePath(Volumes::RINGTONES);
}

void AudioStateMachine::stateAlarmToSpeakerExited()
{

}

void AudioStateMachine::stateScreensaverWithPlayEntered()
{

}

void AudioStateMachine::stateScreensaverWithPlayExited()
{

}

void AudioStateMachine::stateScreensaverWithoutPlayEntered()
{

}

void AudioStateMachine::stateScreensaverWithoutPlayExited()
{

}

AudioStateMachine *bt_global::audio_states = 0;

