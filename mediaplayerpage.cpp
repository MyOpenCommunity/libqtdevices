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
#include "audiostatemachine.h"

#ifdef LAYOUT_TS_10
#include "mount_watcher.h" // bt_global::mount_watcher
#endif

FileListManager::FileListManager()
{
	index = -1;
	total_files = -1;
}

void FileListManager::setList(const EntryInfoList &files)
{
	files_list = files;
	total_files = files.size();
	index = 0;
}

QString FileListManager::currentFilePath()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	return files_list[index].path;
}

void FileListManager::nextFile()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	++index;
	if (index >= total_files)
		index = 0;
	emit currentFileChanged();
}

void FileListManager::previousFile()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	--index;
	if (index < 0)
		index = total_files - 1;
	emit currentFileChanged();
}

int FileListManager::currentIndex()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	return index;
}

void FileListManager::setCurrentIndex(int i)
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	Q_ASSERT_X(index >= 0 && index < total_files, "FileListManager::setCurrentIndex", "index out of range");
	index = i;
}

int FileListManager::totalFiles()
{
	Q_ASSERT_X(index != -1 && total_files != -1, "FileListManager", "file list not initialized");
	return total_files;
}

EntryInfo::Metadata FileListManager::currentMeta()
{
	return EntryInfo::Metadata();
}


MediaPlayerPage::MediaPlayerPage() : refresh_data(this)
{
	player = new MediaPlayer(this);
	temporary_pause = false;

#ifdef LAYOUT_TS_10
	// terminate player when unmounted
	connect(bt_global::mount_watcher, SIGNAL(directoryUnmounted(QString,MountType)), SLOT(unmounted(QString)));
#endif
	// pause local playback when receiving a VCT call
	connect(bt_global::audio_states, SIGNAL(stateAboutToChange(int)), SLOT(audioStateAboutToChange(int)));
	connect(bt_global::audio_states, SIGNAL(stateChanged(int,int)), SLOT(audioStateChanged(int,int)));

	// handle audio state machine state changes
	connect(player, SIGNAL(mplayerStarted()), SLOT(playbackStarted()));
	connect(player, SIGNAL(mplayerResumed()), SLOT(playbackStarted()));
	connect(player, SIGNAL(mplayerDone()), SLOT(playbackStopped()));
	connect(player, SIGNAL(mplayerStopped()), SLOT(playbackStopped()));
	connect(player, SIGNAL(mplayerPaused()), SLOT(playbackStopped()));
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
	connect(player, SIGNAL(mplayerStopped()), buttons, SLOT(stopped()));
	connect(player, SIGNAL(mplayerPaused()), buttons, SLOT(stopped()));
}

void MediaPlayerPage::stop()
{
	player->stop();
	emit Closed();
}

void MediaPlayerPage::pause()
{
	player->pause();
}

void MediaPlayerPage::resume()
{
	if (player->isInstanceRunning())
		player->resume();
	else
		startPlayback();

}

void MediaPlayerPage::previous()
{
	player->quit();
	list_manager->previousFile();
}

void MediaPlayerPage::next()
{
	player->quit();
	list_manager->nextFile();
}

void MediaPlayerPage::seekForward()
{
	player->seek(10);
}

void MediaPlayerPage::seekBack()
{
	player->seek(-10);
}

void MediaPlayerPage::videoPlaybackTerminated()
{
	refresh_data.stop();
}

void MediaPlayerPage::unmounted(const QString &dir)
{
	if (player->isInstanceRunning() && list_manager->currentFilePath().startsWith(dir))
		stop();
}

void MediaPlayerPage::audioStateChanged(int new_state, int old_state)
{
	if ((new_state == AudioStates::PLAY_MEDIA_TO_SPEAKER || new_state == AudioStates::PLAY_DIFSON) && temporary_pause && player->isPaused())
		resume();
}

void MediaPlayerPage::audioStateAboutToChange(int old_state)
{
	// when not a sound diffusion source, do not force the player off when going from sound-diffusion to local playback
	if (old_state == AudioStates::PLAY_DIFSON && !bt_global::audio_states->isSource())
		return;

	if ((old_state == AudioStates::PLAY_MEDIA_TO_SPEAKER || old_state == AudioStates::PLAY_DIFSON) && player->isPlaying())
	{
		temporary_pause = true;
		bt_global::audio_states->setMediaPlayerTemporaryPause(true);
		pause();
	}
}

void MediaPlayerPage::playbackStarted()
{
	temporary_pause = false;
	refresh_data.start();
	bt_global::audio_states->setMediaPlayerTemporaryPause(false);
}

void MediaPlayerPage::playbackStopped()
{
	refresh_data.stop();
}
