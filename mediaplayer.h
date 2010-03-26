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
#include <QProcess>

#include <stdio.h>

class QRect;

class MediaPlayer : public QObject
{
Q_OBJECT
public:
	MediaPlayer(QObject *parent = 0);

	/// Starts MPlayer playing a single track
	bool play(QString track, bool write_output = true);

	/// Starts MPlayer playing a video
	bool playVideo(QString track, QRect geometry, int start_time, bool write_output = true);
	bool playVideoFullScreen(QString track, int start_time, bool write_output = true);

	/// Pause playing of current song
	void pause();

	/// Resume playing of current song
	void resume();

	/// Stop and quit mplayer
	void quit();

	/// Like quit, but wait for mplayer to terminate
	void quitAndWait();

	/// Wheather the player is on pause
	bool isPaused() { return paused; }

	/// Retrun true if music is playing
	bool isInstanceRunning();

	/// Read ASYNCHRONOUSLY the output from the PIPE and parse it to get INFO
	QMap<QString, QString> getPlayingInfo();

	QMap<QString, QString> getVideoInfo();

	/// Need to be public because called by signal handler
	void sigChildReceived(int dead_pid, int status);

	/// moves the playback backward/forward by the given offset
	void seek(int seconds);

private:
	QProcess mplayer_proc;

	/// Send a string to the mplayer process to execute.
	void execCmd(const QByteArray &command);

	bool runMPlayer(const QList<QString> &args, bool write_output);
	QList<QString> getStandardArgs();
	QList<QString> getVideoArgs(int seek_time);
	QList<QString> getAudioArgs(int seek_time);

	QMap<QString, QString> getMediaInfo(const QMap<QString, QString> &data_search);

	bool paused;

private slots:
	void mplayerFinished(int exit_code, QProcess::ExitStatus exit_status);
	void mplayerError(QProcess::ProcessError error);

signals:
	/// mplayer child process quit gracefully and done it's work.
	void mplayerDone();

	/// mplayer child process was killed by a signal
	void mplayerKilled();

	/// mplayer exited with error or aborted
	void mplayerAborted();
};

#endif

