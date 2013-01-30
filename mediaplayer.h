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


// Given the current time (as a string, in the formats ss, mm:ss or h:mm:ss)
// and the total time returns an unique string formatted
// accordingly with the total time:
// mm:ss / mm:ss if the total hours is less than 1;
// [hh:]mm:ss / hh:mm:ss if the total hours is more than 1.
QString formatTime(const QString &current_time, const QString &total_time);

// Given the current time (as a string, in one of teh formats returned by MPlayer,
// discards fractional seconds and returns a QTime object
QTime parseMPlayerTime(const QString &time);

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
	enum OutputMode
	{
		OutputStdout = 1,
		OutputStderr = 2,
		OutputAll    = OutputStdout|OutputStderr
	};

	/*!
		\brief Constructor.
	 */
	MediaPlayer(QObject *parent = 0);

	~MediaPlayer();

	/*!
		\brief Start playing an audio file.

		If \a write_output is \c false, there is no need to call getPlayingInfo() periodically,
		and there is no way to get information for the playing audio track and no way to detect
		when the playback pauses.

		\see getPlayingInfo()
	 */
	bool play(QString track, float start_time, OutputMode write_output = OutputAll);

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
	bool playVideo(QString track, QRect geometry, float start_time, OutputMode write_output = OutputAll);

	/*!
		\brief Start playing a video file full screen.

		\see getVideoInfo()
		\see playVideo()
	 */
	bool playVideoFullScreen(QString track, float start_time, OutputMode write_output = OutputAll);

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

#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
	/*!
		\brief Set the global template for command line arguments

		The special values "<FILE_NAME>" and "<SEEK_TIME>" are replaced with file name and seek time (in seconds).
	*/
	static void setGlobalCommandLineArguments(QString executable, QStringList audio, QStringList video);

	/*!
		\brief Set the template for command line arguments

		The special values "<FILE_NAME>" and "<SEEK_TIME>" are replaced with file name and seek time (in seconds).
	*/
	void setCommandLineArguments(QStringList audio, QStringList video);
#endif

private:
	bool active, is_video;

	/// Send a string to the mplayer process to execute.
	void execCmd(const QByteArray &command);

	// called when the mplayer process really stops playback
	void actuallyPaused();

#ifdef LAYOUT_TS_10
	// update the direct video/audio access state
	void updateDirectAccessState(bool state);
#endif

	bool runMPlayer(const QList<QString> &args, OutputMode write_output);
	QList<QString> getStandardArgs();
	QList<QString> getVideoArgs(float seek_time);
	QList<QString> getAudioArgs(float seek_time);

	QMap<QString, QString> getMediaInfo(const QMap<QString, QString> &data_search, int msecs_timeout = 0);

	QFutureWatcher<QMap<QString,QString> > *info_watcher;

	bool paused, really_paused;

	static QString global_player_executable;
#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
	static QStringList global_audio_cmdline, global_video_cmdline;
	QStringList audio_cmdline, video_cmdline;
#endif
	QProcess *mplayer_proc;

private slots:
	void mplayerFinished(int exit_code, QProcess::ExitStatus exit_status);
	void mplayerError(QProcess::ProcessError error);
	void infoReceived();
	void readStandardError();

#ifdef LAYOUT_TS_10
	void playbackStarted();
#endif

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

	/*!
		\brief Emitted when there is new player output to parse
	*/
	void outputAvailable();
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

#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
	/*!
		\brief Set the template for command line arguments

		The special value "<FILE_NAME>" is replaced with file name.
	*/
	static void setGlobalCommandLineArguments(QString executable, QStringList arguments);
#endif

	/// Return active state
	bool isActive() const;

signals:
	/*!
		This signal is not emitted when the currently-playing sound is stopped
		by a call to play().
	 */
	void soundStarted();

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
#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
	static QString player_executable;
	static QStringList audio_cmdline;
#endif

	void start();
};


namespace bt_global { extern SoundPlayer *sound; }


#endif

