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


#include "mediaplayerpage.h"
#include "mediaplayer.h"
#include "multimedia_buttons.h"
#include "mount_watcher.h"
#include "pagestack.h"
#include "audiostatemachine.h"


MediaPlayerPage::MediaPlayerPage() :
	refresh_data(this)
{
	player = new MediaPlayer(this);
	resume_on_state_change = false;

	// terminate player when unmounted
	connect(&MountWatcher::getWatcher(), SIGNAL(directoryUnmounted(const QString &, MountType)),
		SLOT(unmounted(const QString &)));

	// pause local playback when receiving a VCT call
	if (!bt_global::audio_states->isSource())
		connect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), SLOT(audioStateChanged(int,int)));

	// handle audio state machine state changes
	connect(player, SIGNAL(mplayerStarted()), SLOT(playbackStarted()));
	connect(player, SIGNAL(mplayerResumed()), SLOT(playbackStarted()));
	connect(player, SIGNAL(mplayerDone()), SLOT(playbackStopped()));
	connect(player, SIGNAL(mplayerKilled()), SLOT(playbackStopped()));
	connect(player, SIGNAL(mplayerAborted()), SLOT(playbackStopped()));
	connect(player, SIGNAL(mplayerPaused()), SLOT(playbackStopped()));
}

void MediaPlayerPage::showPage()
{
	bt_global::page_stack.showUserPage(this);

	Page::showPage();
}

bool MediaPlayerPage::isPlayerInstanceRunning() const
{
	return player->isInstanceRunning();
}

bool MediaPlayerPage::isPlayerPaused() const
{
	return player->isPaused();
}

void MediaPlayerPage::connectMultimediaButtons(MultimediaPlayerButtons *buttons)
{
	// handle clicks on buttons
	connect(buttons, SIGNAL(previous()), SLOT(previous()));
	connect(buttons, SIGNAL(next()), SLOT(next()));
	connect(buttons, SIGNAL(stop()), SLOT(stop()));
	connect(buttons, SIGNAL(play()), SLOT(resume()));
	connect(buttons, SIGNAL(pause()), SLOT(pause()));
	connect(buttons, SIGNAL(seekForward()), SLOT(seekForward()));
	connect(buttons, SIGNAL(seekBack()), SLOT(seekBack()));

	// update the icon of the play button
	connect(player, SIGNAL(mplayerStarted()), buttons, SLOT(started()));
	connect(player, SIGNAL(mplayerResumed()), buttons, SLOT(started()));
	connect(player, SIGNAL(mplayerDone()), buttons, SLOT(stopped()));
	connect(player, SIGNAL(mplayerKilled()), buttons, SLOT(stopped()));
	connect(player, SIGNAL(mplayerAborted()), buttons, SLOT(stopped()));
	connect(player, SIGNAL(mplayerPaused()), buttons, SLOT(stopped()));

	// handle mplayer termination
	connect(player, SIGNAL(mplayerDone()), SLOT(next()));
	connect(player, SIGNAL(mplayerAborted()), SLOT(videoPlaybackTerminated()));
	connect(player, SIGNAL(mplayerKilled()), SLOT(videoPlaybackTerminated()));
}

void MediaPlayerPage::stop()
{
	player->quit();
	emit Closed();
}

void MediaPlayerPage::pause()
{
	player->pause();
}

void MediaPlayerPage::resume()
{
	if (player->isInstanceRunning())
	{
		player->resume();
		refresh_data.start(MPLAYER_POLLING);
	}
	else
		displayMedia(current_file);
}

void MediaPlayerPage::previous()
{
	player->quitAndWait();
	current_file -= 1;
	if (current_file < 0)
		current_file = total_files - 1;

	displayMedia(current_file);
}

void MediaPlayerPage::next()
{
	player->quitAndWait();
	current_file += 1;
	if (current_file >= total_files)
		current_file = 0;

	displayMedia(current_file);
}

void MediaPlayerPage::seekForward()
{
	player->seek(10);
	refresh_data.start(MPLAYER_POLLING);
}

void MediaPlayerPage::seekBack()
{
	player->seek(-10);
	refresh_data.start(MPLAYER_POLLING);
}

void MediaPlayerPage::videoPlaybackTerminated()
{
	refresh_data.stop();
}

void MediaPlayerPage::unmounted(const QString &dir)
{
	if (player->isInstanceRunning() && file_list[current_file].startsWith(dir))
		stop();
}

void MediaPlayerPage::audioStateChanged(int new_state, int old_state)
{
	if (new_state == AudioStates::PLAY_MEDIA_TO_SPEAKER && resume_on_state_change && player->isPaused())
		resume();
	else if (old_state == AudioStates::PLAY_MEDIA_TO_SPEAKER && player->isInstanceRunning() && !player->isPaused())
	{
		resume_on_state_change = true;
		pause();
	}
}

void MediaPlayerPage::playbackStarted()
{
	// when the player resumes after an higher priority state (alarm, vct) there is no
	// need to reenter the play state
	if (!bt_global::audio_states->isSource() && !resume_on_state_change)
		bt_global::audio_states->toState(AudioStates::PLAY_MEDIA_TO_SPEAKER);
	resume_on_state_change = false;
}

void MediaPlayerPage::playbackStopped()
{
	// leave the play state on the stack when the player is paused beacuse of a higher priority state
	if (!bt_global::audio_states->isSource() && !resume_on_state_change && bt_global::audio_states->contains(AudioStates::PLAY_MEDIA_TO_SPEAKER))
		bt_global::audio_states->removeState(AudioStates::PLAY_MEDIA_TO_SPEAKER);
	refresh_data.stop();
}
