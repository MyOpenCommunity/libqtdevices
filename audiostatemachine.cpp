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
#include "generic_functions.h" // smartExecute, silentExecute
#include "main.h" // bt_global::config

#include <QtConcurrentRun>
#include <QProcess>
#include <QTimer>
#include <QTime>
#include <QFile>

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
		smartExecute("/bin/in_scsbb_on");
	}

	void disactivateVCTAudio()
	{
		smartExecute("/bin/in_scsbb_off");
	}

	void activateLocalSource()
	{
		smartExecute("/bin/rca2_on");
	}

	void deactivateLocalSource()
	{
		smartExecute("/bin/rca2_off");
	}

	void activateLocalAmplifier()
	{
		QFile ampli_device("/proc/sys/dev/btweb/en_ampli");

		qDebug() << "Enabling local amplifier";
		ampli_device.open(QFile::WriteOnly);
		ampli_device.write("1\n");
		ampli_device.flush();
		ampli_device.close();
	}

	void deactivateLocalAmplifier()
	{
		QFile ampli_device("/proc/sys/dev/btweb/en_ampli");

		qDebug() << "Disabling local amplifier";
		ampli_device.open(QFile::WriteOnly);
		ampli_device.write("0\n");
		ampli_device.flush();
		ampli_device.close();
	}

	void changeVolumePath(Volumes::Type type, int value)
	{
		Q_ASSERT_X(value >= VOLUME_MIN && value <= VOLUME_MAX, "changeVolumePath",
			qPrintable(QString("Volume value %1 out of range for audio path %2!").arg(value).arg(type)));

		// Volumes for the script set_volume starts from 1, so we add it.
		smartExecute("/home/bticino/bin/set_volume",
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

	bool isVideoCallState(int state)
	{
		return state == AudioStates::IP_INTERCOM_CALL || state == AudioStates::IP_VIDEO_CALL ||
		       state == AudioStates::SCS_INTERCOM_CALL || state == AudioStates::SCS_VIDEO_CALL ||
		       state == AudioStates::PLAY_VDE_RINGTONE || state == AudioStates::MUTE;
	}

	bool isAlarmState(int state)
	{
		return state == AudioStates::ALARM_TO_SPEAKER;
	}
}


using namespace AudioStates;


AudioStateMachine::AudioStateMachine()
{
	// To avoid useless write in the eeprom memory, we use a timer to 'compress'.
	volumes_timer = new QTimer(this);
	volumes_timer->setInterval(VOLUME_TIMER_SECS * 1000);
	connect(volumes_timer, SIGNAL(timeout()), SLOT(saveVolumes()));
	connect(this, SIGNAL(directAudioAccessStopped()), SLOT(completeStateChange()));

	transition_guard = new QTimer(this);
	transition_guard->setInterval(1000);
	transition_guard->setSingleShot(true);
	connect(transition_guard, SIGNAL(timeout()), SLOT(forceStateChange()));

	current_audio_path = -1;
	direct_audio_access = false;
	pending_old_state = pending_new_state = -1;
	is_source = !(*bt_global::config)[SOURCE_ADDRESS].isEmpty();
	is_amplifier = !(*bt_global::config)[AMPLIFIER_ADDRESS].isEmpty();
	local_source_status = local_amplifier_status = media_player_status = media_player_temporary_pause = false;

	addState(IDLE,
		 SLOT(stateIdleEntered()),
		 SLOT(stateIdleExited()));
	addState(BEEP_ON,
		 SLOT(stateBeepOnEntered()),
		 SLOT(stateBeepOnExited()));
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
	addState(PLAY_VDE_RINGTONE,
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
	addState(SCREENSAVER,
		 SLOT(stateScreensaverEntered()),
		 SLOT(stateScreensaverExited()));

	// by default, all state transitions are possible, if this is not
	// correct, use removeTransitions() to make some transitions impossible

	// go to the start state
	start(IDLE);
}

void AudioStateMachine::start(int state)
{
	QtConcurrent::run(initEchoCanceller);
	initVolumes();
	disactivateVCTAudio();

	// turn off the local source/amplifier at startup
	deactivateLocalAmplifier();
	changeVolumePath(Volumes::MM_AMPLIFIER, 0);
	deactivateLocalSource();
	changeVolumePath(Volumes::MM_SOURCE, 0);

	StateMachine::start(state);
}

bool AudioStateMachine::toState(int state)
{
	int index = stateCount();

	// there are these "interesting" special cases: video call states have precedence
	// over alarm states, which have precedence over everything else; if we try
	// to transition from a high priority state to a low priority state, we note
	// this fact by inserting the state in the state stack below the high priority states
	if (!isVideoCallState(state))
		while (isVideoCallState(stateAt(index - 1)))
			index -= 1;
	if (!isAlarmState(state) && !isVideoCallState(state))
		while (isAlarmState(stateAt(index - 1)))
			index -= 1;
	// screensaver must be added just above the beep (if present)
	if (state == AudioStates::SCREENSAVER)
		index = (stateCount() > 1 && stateAt(1) == AudioStates::BEEP_ON ? 2 : 1);
	// beep is always the lowest state (just above IDLE)
	if (state == AudioStates::BEEP_ON)
		index = 1;

	if (index == stateCount())
		return StateMachine::toState(state);

	insertState(index, state);

	return true;
}

void AudioStateMachine::changeState(int new_state, int old_state)
{
	emit stateAboutToChange(old_state);

	pending_old_state = old_state;
	pending_new_state = new_state;

	if (isDirectAudioAccess())
	{
		// completeStateChange will be called after the playing process completes
		qDebug() << "Delaying audio state transition";

		// safety catch
		transition_guard->start();
	}
	else
	{
		qDebug() << "Direct audio state transition";

		completeStateChange();
	}
}

void AudioStateMachine::forceStateChange()
{
	// check if there is any pending transition
	if (pending_new_state == -1 || pending_old_state == -1)
		return;

	qDebug() << "Forcing a delayed state change";
	completeStateChange();
}

void AudioStateMachine::completeStateChange()
{
	// check if there is any pending transition
	if (pending_new_state == -1 || pending_old_state == -1)
		return;

	callStateCallbacks(pending_new_state, pending_old_state);

	emit stateChanged(pending_new_state, pending_old_state);

	pending_old_state = pending_new_state = -1;
}

void AudioStateMachine::saveVolumes()
{
	::saveVolumes();
	volumes_timer->stop();
}

// local source/amplifier handling for sound diffusion

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
	if (currentState() == AudioStates::PLAY_DIFSON)
	{
		if (status)
		{
			activateLocalAmplifier();
			changeVolumePath(Volumes::MM_AMPLIFIER);
		}
		else
		{
			deactivateLocalAmplifier();
			changeVolumePath(Volumes::MM_AMPLIFIER, 0);
		}
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
		changeVolumePath(Volumes::MM_AMPLIFIER);
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
		{
			activateLocalSource();
			changeVolumePath(Volumes::MM_SOURCE, DEFAULT_VOLUME);
		}
		else
		{
			deactivateLocalSource();
			changeVolumePath(Volumes::MM_SOURCE, 0);
		}
	}
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
	media_player_temporary_pause = paused;
}

bool AudioStateMachine::isSoundDiffusionActive()
{
	return getLocalAmplifierStatus() || getLocalSourceStatus() || media_player_status;
}

// misc methods

void AudioStateMachine::setDirectAudioAccess(bool status)
{
	if (status == direct_audio_access)
		return;
	direct_audio_access = status;
	if (direct_audio_access)
		emit directAudioAccessStarted();
	else
		emit directAudioAccessStopped();
}

bool AudioStateMachine::isDirectAudioAccess()
{
       return direct_audio_access;
}

void AudioStateMachine::setVolume(int value)
{
	Q_ASSERT_X(value >= VOLUME_MIN && value <= VOLUME_MAX, "AudioStateMachine::setVolume",
		qPrintable(QString("Volume value %1 out of range for audio path %2!").arg(value).arg(current_audio_path)));

	Q_ASSERT_X(current_audio_path != -1, "AudioStateMachine::setVolume", "setVolume called without set an audio path!");
	volumes[current_audio_path] = value;
	changeVolumePath(static_cast<Volumes::Type>(current_audio_path));
	volumes_timer->start();
}

int AudioStateMachine::getVolume()
{
	Q_ASSERT_X(current_audio_path != -1, "AudioStateMachine::getVolume",
		"You must set the current audio path before getting the current volume value!");

	return volumes.at(current_audio_path);
}

// state callbacks

void AudioStateMachine::stateIdleEntered()
{
	qDebug() << "AudioStateMachine::stateIdleEntered";

	deactivateLocalAmplifier();
}

void AudioStateMachine::stateIdleExited()
{
	qDebug() << "AudioStateMachine::stateIdleExited";

	activateLocalAmplifier();
}

void AudioStateMachine::stateBeepOnEntered()
{
	qDebug() << "AudioStateMachine::stateBeepOnEntered";

	current_audio_path = Volumes::BEEP;
	changeVolumePath(Volumes::BEEP);
}

void AudioStateMachine::stateBeepOnExited()
{
	qDebug() << "AudioStateMachine::stateBeepOnExited";
}

void AudioStateMachine::statePlayMediaToSpeakerEntered()
{
	qDebug() << "AudioStateMachine::statePlayMediaToSpeakerEntered";

	current_audio_path = Volumes::MM_LOCALE;
	changeVolumePath(Volumes::MM_LOCALE);
}

void AudioStateMachine::statePlayMediaToSpeakerExited()
{
	qDebug() << "AudioStateMachine::statePlayMediaToSpeakerExited";
}

void AudioStateMachine::statePlayDifsonEntered()
{
	qDebug() << "AudioStateMachine::statePlayDifsonEntered";

	if (local_amplifier_status)
	{
		activateLocalAmplifier();
		current_audio_path = Volumes::MM_AMPLIFIER;
		changeVolumePath(Volumes::MM_AMPLIFIER);
	}
	else
		deactivateLocalAmplifier();

	if (local_source_status)
	{
		activateLocalSource();
		current_audio_path = Volumes::MM_SOURCE;
		changeVolumePath(Volumes::MM_SOURCE);
	}
}

void AudioStateMachine::statePlayDifsonExited()
{
	qDebug() << "AudioStateMachine::statePlayDifsonExited";

	if (local_amplifier_status)
		changeVolumePath(Volumes::MM_AMPLIFIER, 0);
	// always leave local amplifier active on exit
	activateLocalAmplifier();

	if (local_source_status)
	{
		deactivateLocalSource();
		changeVolumePath(Volumes::MM_SOURCE, 0);
	}
}

void AudioStateMachine::statePlayRingtoneEntered()
{
	qDebug() << "AudioStateMachine::statePlayRingtoneEntered";

	// TODO: move this check in the changeVolumePath!
	if (current_audio_path != Volumes::RINGTONES)
	{
		current_audio_path = Volumes::RINGTONES;
		changeVolumePath(Volumes::RINGTONES);
	}
}

void AudioStateMachine::statePlayRingtoneExited()
{
	qDebug() << "AudioStateMachine::statePlayRingtoneExited";
}

void AudioStateMachine::stateScsVideoCallEntered()
{
	qDebug() << "AudioStateMachine::stateScsVideoCallEntered";

	activateVCTAudio();
	current_audio_path = Volumes::VIDEODOOR;
	changeVolumePath(Volumes::VIDEODOOR);
}

void AudioStateMachine::stateScsVideoCallExited()
{
	qDebug() << "AudioStateMachine::stateScsVideoCallExited";

	disactivateVCTAudio();
}

void AudioStateMachine::stateScsIntercomCallEntered()
{
	qDebug() << "AudioStateMachine::stateScsIntercomCallEntered";

	activateVCTAudio();
	current_audio_path = Volumes::INTERCOM;
	changeVolumePath(Volumes::INTERCOM);
}

void AudioStateMachine::stateScsIntercomCallExited()
{
	qDebug() << "AudioStateMachine::stateScsIntercomCallExited";

	disactivateVCTAudio();
}

void AudioStateMachine::stateMuteEntered()
{
	qDebug() << "AudioStateMachine::stateMuteEntered";

	current_audio_path = Volumes::MICROPHONE;
	setVolume(0);
}

void AudioStateMachine::stateMuteExited()
{
	qDebug() << "AudioStateMachine::stateMuteExited";

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
	qDebug() << "AudioStateMachine::stateAlarmToSpeakerEntered";

	current_audio_path = Volumes::RINGTONES;
	changeVolumePath(Volumes::RINGTONES);
}

void AudioStateMachine::stateAlarmToSpeakerExited()
{
	qDebug() << "AudioStateMachine::stateAlarmToSpeakerExited";
}

void AudioStateMachine::stateScreensaverEntered()
{
	qDebug() << "AudioStateMachine::stateScreensaverEntered";

	deactivateLocalAmplifier();
}

void AudioStateMachine::stateScreensaverExited()
{
	qDebug() << "AudioStateMachine::stateScreensaverExited";

	activateLocalAmplifier();
}

AudioStateMachine *bt_global::audio_states = 0;
