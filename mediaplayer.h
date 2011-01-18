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

// the max size allowed for videos.
#define MAX_VIDEO_WIDTH 320
#define MAX_VIDEO_HEIGHT 240


/*!
	\ingroup Multimedia
	\ingroup SoundDiffusion
	\brief Play audio/video files using MPlayer

	After calling play() to reproduce an audio file, it's necessary to periodically
	(every 2-3 seconds) call getPlayingInfo() to process MPlayer output.

	Since the playback is handled by an external process, using one of the control
	commands (play(), pause(), resume(), stop()) will not immediately affect the playback
	state; for the commands starting/restarting the playback, the mplayerStarted() / mplayerResumed()
	signals are emitted before the playback actually starts; likewise, for commands stopping/pausing
	the playback, the mplayerPaused() / mplayerStopped() signals are emitted after the playback actually
	starts.

	Another tricky part of the API is the handling of pause states: see pause(), isPaused(), isPlaying(),
	isReallyPaused() for an explanation.
 */
class MediaPlayer : public QObject
{
Q_OBJECT
public:
	/*!
		\brief Constructor.
	 */
	MediaPlayer(QObject *parent = 0);

	/*!
		\brief Start playing an audio file.

		If \a write_output is \c false, there is no need to call getPlayingInfo() periodically,
		and there is no way to get information for the playing audio track and no way to detect
		when the playback pauses.

		\see getPlayingInfo()
	 */
	bool play(QString track, bool write_output = true);

	/*!
		\brief Start playing a video file.

		If \a write_output is false, there is no need to call getVideoInfo() periodically,
		and there is no way to get information for the playing vidoe and no way to detect
		when the playback pauses.

		When \a start_time is greater than 0, will start the playback at approximately
		that time into the video track.

		The \a geometry is in screen coordinates.

		\see getVideoInfo()
		\see playVideoFullScreen()
	 */
	bool playVideo(QString track, QRect geometry, int start_time, bool write_output = true);

	/*!
		\brief Start playing a video file full screen.

		\see getVideoInfo()
		\see playVideo()
	 */
	bool playVideoFullScreen(QString track, int start_time, bool write_output = true);

	/*!
		\brief Pause playback.

		Note that this only sends the pause command to MPlayer; to know when
		the playback actually pauses, use the mplayerPaused() signal.

		\see mplayerPaused()
	 */
	void pause();

	/*!
		\brief Resume playback after a pause.

		Note that this only sends the resume command to MPlayer; the actual playback
		will start sometimes later.

		\see mplayerResumed()
	 */
	void resume();

	// TODO cleanly separate the logical player state from the physical MPlayer state
	// (for things like the next-while-paused that are currently handled in multiple
	// parts of the GUI code)

	/*!
		\brief Stop playback and quit MPlayer.

		Can also be called when there is no physical MPlayer running,
		but isPaused() is \c true.
	 */
	void stop();

	/*!
		\brief Stop playback and quit MPlayer.

		Similar to stop, but does not reset the paused state; must only be used
		when MPlayer is "logically" paused (for example if the user uses prev/next
		while the player is paused).

		\see stop()
	 */
	void quit();

	/*!
		\brief \c true if player is playing

		Note that this is a logical state: right after calling pause() (or resume())
		isPlaying() return value reflects the just-invoked method, but MPlayer might
		not have stopped (or restarted) playback yet.

		\see isPaused()
		\see isReallyPaused()
	 */
	bool isPlaying();

	/*!
		\brief Whether the player is paused

		Due to the asynchronous nature of the communication with MPlayer, isPaused()
		does not always coincide with current MPlayer state; isPaused()
		returns the state MPlayer will have after it processes all pending pause() / resume()
		commands; to detect when MPlayer really pauses, use the mplayerPaused() signal or
		isReallyPaused().
	 */
	bool isPaused() { return paused; }

	/*!
		\brief Whether the player is paused

		Returns physical player state.  Note that the value is updated when reading
		MPlayer output, so there is a small delay between MPlayer pausing and the return
		value becoming \c true.

		\see isPaused()
	 */
	bool isReallyPaused() { return really_paused; }

	/*!
		\brief Whether there is an active MPlayer instance.

		This is \c true both for a playing and a paused MPlayer.
	 */
	bool isInstanceRunning();

	/*!
		\brief Process MPlayer output for audio tracks

		Must be called regularly during playback to process MPlayer output,
		otherwise playback will stop; it also returns information about the
		playing track and update the playing/paused state.

		If the event loop is not running, \a msecs_timeout must be greater than 0,
		otherwise no output will ever be read.

		Available tags for all tracks:
		- current_time: (MM:SS)

		Available tags for audio files:
		- file_name: the file name
		- meta_title: track title, as written in ID3 tags
		- meta_artist: track author/performer, as written in ID3 tags
		- meta_album: track album, as written in ID3 tags
		- total_time: total track time, either from ID3 tags or guessed by the player

		Available tags ShoutCast URLs:
		- stream_title: title of this stream
		- stream_url: URL of the stream (as reported in the stream itself)

		\see getVideoInfo()
	 */
	QMap<QString, QString> getPlayingInfo(int msecs_timeout = 0);

	/*!
		\brief Process MPlayer output for audio tracks.

		The only available tag is:
		- current_time: (MM:SS)

		\see getPlayingInfo()
	 */
	QMap<QString, QString> getVideoInfo(int msecs_timeout = 0);

	/*!
		\brief Request audio track info

		Request the information returned by getPlayingInfo() without starting
		the playback.

		Emits playingInfoUpdated() when the information becomes available.
	 */
	void requestInitialPlayingInfo(const QString &track);

	/*!
		\brief Request video track info

		Request the information returned by getVideoInfo() without starting
		the playback.

		Emits playingInfoUpdated() when the information becomes available.
	 */
	void requestInitialVideoInfo(const QString &track);

	/*!
		\brief Seek forward/backwards

		Moves the playback backward/forward by the given offset.  Due to
		limitations of various audio/video formats, the actual seek time will
		rarely equal the offset.
	 */
	void seek(int seconds);

	/*!
		\brief Return true if the video has a resolution less than the max allowed.
	*/
	bool checkVideoResolution(QString track);

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
	/*!
		\brief Emitted before starting playback of a media file.
	 */
	void mplayerStarted();

	/*!
		\brief Emitted after media playback pauses.
	 */
	void mplayerPaused();

	/*!
		\brief Emitted before media playback resumes after a pause.
	 */
	void mplayerResumed();

	/*!
		\brief Playback completed successfully.
	 */
	void mplayerDone();

	/*!
		\brief Playback stopped by explicit request (GUI or other process) or by an error.
	 */
	void mplayerStopped();

	/*!
		\brief Information for a video/audio file.

		Emitted by requestInitialVideoInfo() and requestInitialPlayingInfo().
		\a info contains the same data returned by getPlayingInfo() and getVideoInfo().
	 */
	void playingInfoUpdated(const QMap<QString,QString> &info);
};


/*!
	\ingroup Core
	\brief Simple interface for .wav file playback
 */
class SoundPlayer : public QObject
{
Q_OBJECT
public:
	/*!
		\brief A global instance is available in bt_global::sound

		Instantiating a separate object is only necessary to detect when
		playback finished using soundFinished() (since the soundFinished()
		for the global instance might be triggered by any user of the global
		instance).
	 */
	SoundPlayer(QObject *parent=0);

	/*!
		\brief Play a .wav

		If a sound is playing it is stopped, but soundFinished() is not emitted.
	 */
	void play(const QString &path);

	/*!
		\brief Stops the currently-playing sound

		Emits soundFinished() when the playback actually stops.
	 */
	void stop();

signals:
	/*!
		This signal is not emitted when the currently-playing sound is stopped
		by a call to play().
	 */
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

