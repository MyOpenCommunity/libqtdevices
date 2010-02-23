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


#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QMap>
#include <QObject>

#include <stdio.h>

class MediaPlayer : public QObject
{
Q_OBJECT
public:
	MediaPlayer(QObject *parent = 0);
	~MediaPlayer();

	/// Starts MPlayer playing a single track
	bool play(QString track, bool write_output = true);

	/// Pause playing of current song
	void pause();

	/// Resume playing of current song
	void resume();

	/// Stop and quit mplayer
	void quit();

	/// Wheather the player is on pause
	bool isPaused() { return paused; }

	/// Retrun true if music is playing
	bool isInstanceRunning();

	/// Read ASYNCHRONOUSLY the output from the PIPE and parse it to get INFO
	QMap<QString, QString> getPlayingInfo();

	/// Need to be public because called by signal handler
	void sigChildReceived(int dead_pid, int status);

private:
	/// mplayer PID
	int mplayer_pid;

	/// File Descriptors
	int control_fd;
	int output_fd;

	/// File Handlers
	FILE *ctrlf;
	FILE *outf;

	/// Send a string to the mplayer process to execute.
	void execCmd(QString command);
	/// Read output from mplayer process.
	QString readOutput();

	bool _isPlaying;
	bool paused;

signals:
	/// mplayer child process quit gracefully and done it's work.
	void mplayerDone();

	/// mplayer child process was killed by a signal
	void mplayerKilled();

	/// mplayer exited with error or aborted
	void mplayerAborted();
};

#endif

