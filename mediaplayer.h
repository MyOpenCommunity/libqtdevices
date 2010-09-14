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
#include <QFutureWatcher>

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

	/// Stop and quit mplayer (waiting for mplayer to terminate)
	void stop();

	/// Similar to stop, but does not reset the paused state; must only be used
	/// when mplayer is "logically" paused (for example if the user uses prev/next
	/// while the player is paused)
	void quit();

	/// Return true if player is currently playing
	bool isPlaying();

	/// Wheather the player is on pause; due to the asynchronous nature of the communication
	/// with MPlayer, isPaused() does not always coincide with current MPlayer state; isPaused()
	/// returns the state MPlayer will have after it processes all pending commands; to detect when
	/// MPlayer really pauses, use the mplayerPaused() signal.
	bool isPaused() { return paused; }

	/// Return true is player is really paused (it has completed processing of the pause command)
	bool isReallyPaused() { return really_paused; }

	/// Return true if music is playing or in pause
	bool isInstanceRunning();

	/// Read the output from the PIPE and parse it to get INFO; if msecs_timeout
	/// is greater than 0, waits for input to become available; if this metod is called
	/// when the event loop is not running, you must use msecs_timeout > 0 or no output
	/// will be ever read
	QMap<QString, QString> getPlayingInfo(int msecs_timeout = 0);

	QMap<QString, QString> getVideoInfo(int msecs_timeout = 0);

	void requestInitialPlayingInfo(const QString &track);
	void requestInitialVideoInfo(const QString &track);

	/// Need to be public because called by signal handler
	void sigChildReceived(int dead_pid, int status);

	/// moves the playback backward/forward by the given offset
	void seek(int seconds);

private:
	// there can only be a single MPlayer instance running, because only one
	// process at a time can access /dev/dsp.  At any given time, at most one MediaPlayer
	// instance "owns" the MPlayer process (the one with the active flag set).
	//
	// When an instance is not active, calling one of the play*() methods will stop
	// the current playback, if any, and start the requested file.
	// All other methods are no-ops.
	bool active, is_video;
	static QProcess mplayer_proc;

	/// Send a string to the mplayer process to execute.
	void execCmd(const QByteArray &command);

	// called when the mplayer process really stops playback
	void actuallyPaused();

	// update the direct video/audio access state
	void updateDirectAccessState(bool state);

	bool runMPlayer(const QList<QString> &args, bool write_output);
	QList<QString> getStandardArgs();
	QList<QString> getVideoArgs(int seek_time);
	QList<QString> getAudioArgs(int seek_time);

	QMap<QString, QString> getMediaInfo(const QMap<QString, QString> &data_search, int msecs_timeout = 0);

	QFutureWatcher<QMap<QString,QString> > *info_watcher;

	bool paused, really_paused;

private slots:
	void mplayerFinished(int exit_code, QProcess::ExitStatus exit_status);
	void mplayerError(QProcess::ProcessError error);
	void playbackStarted();
	void infoReceived();
	void readStandardError();

signals:
	/// mplayer started the reproduction of the media
	void mplayerStarted();
	/// mplayer paused the reproduction of the media
	void mplayerPaused();
	/// mplayer resumed the reproduction of the media
	void mplayerResumed();

	/// mplayer child process quit gracefully and done it's work.
	void mplayerDone();

	/// mplayer child process was killed by a signal
	void mplayerKilled();

	/// mplayer exited with error or aborted
	void mplayerAborted();

	void playingInfoUpdated(const QMap<QString,QString> &info);
};


/**
 * This class exposes a simple interface to play/stop audio files.
 */
class SoundPlayer : public QObject
{
Q_OBJECT
public:
	SoundPlayer(QObject *parent=0);

	void play(const QString &path);
	void stop();

signals:
	void soundFinished();

private slots:
	void error();
	void processFinished();
	void audioStateAboutToChange();

	void readStandardError();

private:
	bool active;
	QString to_play;
	void start();
};


namespace bt_global { extern SoundPlayer *sound; }


#endif

