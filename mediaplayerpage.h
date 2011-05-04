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


#ifndef MEDIAPLAYERPAGE_H
#define MEDIAPLAYERPAGE_H

#include "page.h"
#include "generic_functions.h" // EntryInfoList

#include <QTimer>

class MediaPlayer;
class MultimediaPlayerButtons;


// The interface for the manager of a list of 'files'.
class ListManager : public QObject
{
Q_OBJECT
public:
	virtual QString currentFilePath() = 0;

	virtual void nextFile() = 0;
	virtual void previousFile() = 0;

	virtual int currentIndex() = 0;
	virtual int totalFiles() = 0;

	virtual EntryInfo::Metadata currentMeta() = 0;

signals:
	void currentFileChanged();
};


// Implements the ListManager interface for files or virtual radio ip item.
// All the items in a directory/album are loaded at the beginning, without regarding
// the number of item displayed.
class FileListManager : public ListManager
{
public:
	FileListManager();
	virtual QString currentFilePath();

	virtual void nextFile();
	virtual void previousFile();

	virtual int currentIndex();
	virtual int totalFiles();

	virtual EntryInfo::Metadata currentMeta();

	// FileListManager specific methods
	void setCurrentIndex(int i);
	void setList(const EntryInfoList &files);

private:
	int index, total_files;
	EntryInfoList files_list;
};



/*!
	\ingroup Multimedia
	\brief Base class for pages with media player functionality

	Defines standard handling for multimedia player buttons and manages a
	(circular) playlist.

	If a subclass starts/stops the media player without using the methods provided
	by %MediaPlayerPage, it must take care of starting/stopping the refresh_data timer
	and emitting the started() / stopped() signals.
 */
class MediaPlayerPage : public Page
{
Q_OBJECT
public:
	static const int MPLAYER_POLLING = 500;

	MediaPlayerPage();

	bool isPlayerInstanceRunning() const;
	bool isPlayerPaused() const;

protected:
	// adds default behaviour for multimedia buttons
	void connectMultimediaButtons(MultimediaPlayerButtons *buttons);

	// reproduce the i-th media object inside file_list
	//
	// must emit started() and start the refresh_data timer
	virtual void startPlayback() = 0;

public slots:
	// standard player functionality
	//
	// note that previous()/next() wrap around at the start/end of the playlist
	//
	// as a convenience, stop() emits Closed()
	virtual void stop();
	virtual void resume();
	virtual void pause();
	virtual void previous();
	virtual void next();
	virtual void seekForward();
	virtual void seekBack();

	// called for abnormal MPlayer termination
	virtual void videoPlaybackTerminated();

protected:
	// media objects handled by the page

	MediaPlayer *player;

	// can be connected
	QTimer refresh_data;

	// set to true when the player is paused due to a audio state change (es. vct call)
	// or (for video player) due to a page change (es. alarm)
	bool temporary_pause;

	ListManager *list_manager;

private slots:
	void unmounted(const QString &path);

	void playbackStarted();
	void playbackStopped();
	void audioStateChanged(int new_state, int old_state);
	void audioStateAboutToChange(int old_state);
};

#endif // MEDIAPLAYERPAGE_H
