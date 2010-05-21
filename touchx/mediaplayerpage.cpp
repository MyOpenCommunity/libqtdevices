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


MediaPlayerPage::MediaPlayerPage() :
	refresh_data(this)
{
	player = new MediaPlayer(this);

	// terminate player when unmounted
	connect(&MountWatcher::getWatcher(), SIGNAL(directoryUnmounted(const QString &, MountType)),
		SLOT(unmounted(const QString &)));
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
	connect(this, SIGNAL(started()), buttons, SLOT(started()));
	connect(this, SIGNAL(stopped()), buttons, SLOT(stopped()));

	// handle mplayer termination
	connect(player, SIGNAL(mplayerDone()), SLOT(next()));
	connect(player, SIGNAL(mplayerAborted()), SLOT(playbackTerminated()));
	connect(player, SIGNAL(mplayerKilled()), SLOT(playbackTerminated()));
}

void MediaPlayerPage::stop()
{
	player->quit();
	emit Closed();
}

void MediaPlayerPage::pause()
{
	player->pause();
	refresh_data.stop();
	emit paused();
	emit stopped();
}

void MediaPlayerPage::resume()
{
	if (player->isInstanceRunning())
	{
		player->resume();
		refresh_data.start(MPLAYER_POLLING);
		emit started();
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
}

void MediaPlayerPage::seekBack()
{
	player->seek(-10);
}

void MediaPlayerPage::playbackTerminated()
{
	emit stopped();
	emit terminated();
	refresh_data.stop();
}

void MediaPlayerPage::unmounted(const QString &dir)
{
	if (player->isInstanceRunning() && file_list[current_file].startsWith(dir))
		stop();
}
