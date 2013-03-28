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


#include "mediaplayer.h"
#ifndef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
#include "hardware_functions.h" // maxWidth, maxHeight, getAudioCmdLine
#endif
#include "displaycontrol.h"
#ifdef LAYOUT_TS_10
#include "audiostatemachine.h"
#endif

#include <QRegExp>
#include <QDebug>
#include <QRect>
#include <QFile>
#include <QVector>
#include <QList>
#include <QMetaEnum>
#include <QtConcurrentRun>
#include <QFutureWatcher>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>

#if defined(BT_HARDWARE_PXA255)
static const char *MPLAYER_FILENAME = "/usr/bin/mplayer";
static const char *MPLAYER_AUDIO_DEVICE = "oss:/dev/dsp1";
#elif defined(BT_HARDWARE_DM365)
static const char *MPLAYER_FILENAME = "/usr/bin/mplayer";
static const char *MPLAYER_AUDIO_DEVICE = "alsa:device=hw=1";
#elif defined(BT_HARDWARE_X11)
static const char *MPLAYER_FILENAME = "mplayer";
#elif defined(BT_HARDWARE_PXA270)
static const char *MPLAYER_FILENAME = "/home/bticino/cfg/extra/10/mplayer";
static const char *MPLAYER_AUDIO_DEVICE = "oss:/dev/dsp1";
#endif


// The timeout for extracting info using media player.
#define MPLAYER_INFO_TIMEOUT_SECS 5


namespace
{
	QMap<QString, QString> getAudioDataSearchMap()
	{
		// Define Search Data Map
		QMap<QString, QString> data_search;
		data_search["file_name"]    = "Playing [^\\n]*([^/\\n]+)\\.\\n";
		data_search["meta_title"]   = "Title: ([^\\n]*)\\n";
		data_search["meta_artist"]  = "Artist: ([^\\n]*)\\n";
		data_search["meta_album"]   = "Album: ([^\\n]*)\\n";
		data_search["total_time"]   = "of\\s+\\d+\\.\\d+\\s+[(]\\s*((?:\\d+:)*\\d+\\.\\d+)[)]";
		data_search["current_time"] = "A:\\s+\\d+\\.\\d+\\s+[(]\\s*((?:\\d+:)*\\d+\\.\\d+)[)]";
		data_search["current_time_only"] = "A:\\s+(\\d+)";
		// shoutcast info
		// MPlayer does not quote "'" inside titles, so parsing the shoutcast info
		// with a regex is tricky; the regexes below can be improved
		data_search["stream_title"] = "\\bStreamTitle='([^;]+)';";
		data_search["stream_url"] = "\\bStreamUrl='([^;]+)';";

		return data_search;
	}

	QMap<QString,QString> getVideoDataSearchMap()
	{
		// Define Search Data Map
		QMap<QString, QString> data_search;
		data_search["current_time"] = "A:\\s+(\\d+\\.\\d+)\\s+";

		return data_search;
	}

	QMap<QString,QString> parsePlayerOutput(const QString &raw_data, QMap<QString,QString> data_search)
	{
		// Create output Map
		QMap<QString, QString> info_data;

		// Parse RAW data to get info
		for (QMap<QString, QString>::const_iterator it = data_search.begin(); it != data_search.end(); ++it)
		{
			QRegExp rx(it.value());

			if (rx.lastIndexIn(raw_data) > -1)
				info_data[it.key()] = rx.cap(1);
		}

		return info_data;
	}

	QMap<QString, QString> extractMPlayerInfo(QString executable, QList<QString> mplayer_args, QMap<QString,QString> data_search,
		QSet<QString> required_keys)
	{
		QTime time_guard;
		time_guard.start();

		QProcess fake_player;
		fake_player.start(executable, mplayer_args);
		fake_player.waitForStarted(300);

		QMap<QString, QString> info_data;
		QString raw_data;


		while (true)
		{
			fake_player.waitForReadyRead(300);
			raw_data.append(fake_player.readAll());

			info_data = parsePlayerOutput(raw_data, data_search);

			bool find_all = true;
			foreach (const QString &key, required_keys)
				if (!info_data.contains(key))
					find_all = false;

			if (find_all || time_guard.elapsed() > MPLAYER_INFO_TIMEOUT_SECS * 1000)
				break;

		}
		fake_player.terminate();
		fake_player.waitForFinished(300);

		return info_data;
	}

	QMap<QString, QString> startFakePlayer(const QString &executable, const QString &track, QMap<QString,QString> data_search)
	{
		QList<QString> args;
		args << "-nolirc" << "-ao" << "null" << "-vo" << "null" << track;
		// We want to wait until the current time info because it's the last, always present
		// info of the mplayer output.
		return extractMPlayerInfo(executable, args, data_search, QSet<QString>() << "current_time");
	}

	QString addDot(const QString &time)
	{
		QString res = time;
		int dot = res.indexOf('.');

		// add zero decimal part if not present
		if (dot < 0)
			res += ".0";
		return res;
	}

	QStringList replaceArguments(QStringList tmpl, QString file_name, float seconds)
	{
		for (int i = 0; i < tmpl.size(); ++i)
		{
			if (tmpl[i] == "<FILE_NAME>")
				tmpl[i] = file_name;
			else if (tmpl[i] == "<SEEK_TIME>")
				tmpl[i] = QString::number(seconds);
		}

		return tmpl;
	}
}

QTime parseMPlayerTime(const QString &time)
{
	// time format may be h:mm:ss (and derivatives) or ss (may be greater then 59)
	// firstly, tries to convert as integer
	bool ok;
	int v = time.toInt(&ok);

	QTime res;
	if (ok)
	{
		// int case
		res = res.addSecs(v);
	}
	else
	{
		QString s = addDot(time);

		// not an int, tries to read h:mm:ss format
		res = QTime::fromString(s, "h:mm:ss.z");
		if (!res.isValid())
			res = QTime::fromString(s, "ss.z");
		if (!res.isValid())
			res = QTime::fromString(s, "mm:ss.z");
	}

	return res;
}

QString formatTime(const QString &current_time, const QString &total_time)
{
	QTime current = parseMPlayerTime(current_time);
	QTime total = parseMPlayerTime(total_time);
	QString curr, tot;

	if (total.hour() == 0)
	{
		curr = current.toString("mm:ss");
		tot = total.toString("mm:ss");
	}
	else
	{
		tot = total.toString("hh:mm:ss");
		if (current.hour() > 0)
			curr = current.toString(current.hour() > 0 ? "hh:mm:ss" : "mm:ss");
	}
	return curr + " / " + tot;
}


#ifndef MEDIAPLAYER_MULTIPLE_PLAYERS
// there can only be a single MPlayer instance running, because only one
// process at a time can access /dev/dsp.  At any given time, at most one MediaPlayer
// instance "owns" the MPlayer process (the one with the active flag set).
//
// When an instance is not active, calling one of the play*() methods will stop
// the current playback, if any, and start the requested file.
// All other methods are no-ops.
Q_GLOBAL_STATIC(QProcess, mplayer_process)
#endif

#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
QString MediaPlayer::global_player_executable;
QStringList MediaPlayer::global_audio_cmdline;
QStringList MediaPlayer::global_video_cmdline;
#else
QString MediaPlayer::global_player_executable = MPLAYER_FILENAME;
#endif

MediaPlayer::MediaPlayer(QObject *parent) : QObject(parent)
{
	active = false;
	is_video = false;
	paused = false;
	really_paused = false;
	info_watcher = 0;
#ifndef MEDIAPLAYER_MULTIPLE_PLAYERS
	mplayer_proc = mplayer_process();
#else
	mplayer_proc = new QProcess(this);
#endif
	connect(mplayer_proc, SIGNAL(readyReadStandardError()), SLOT(readStandardError()));
	connect(mplayer_proc, SIGNAL(readyReadStandardOutput()), SIGNAL(outputAvailable()));
	connect(mplayer_proc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(mplayerFinished(int, QProcess::ExitStatus)));
	connect(mplayer_proc, SIGNAL(error(QProcess::ProcessError)), SLOT(mplayerError(QProcess::ProcessError)));
#ifdef LAYOUT_TS_10
	connect(this, SIGNAL(mplayerResumed()), SLOT(playbackStarted()));
	connect(this, SIGNAL(mplayerStarted()), SLOT(playbackStarted()));
#endif
#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
	audio_cmdline = global_audio_cmdline;
	video_cmdline = global_video_cmdline;
#endif
}

MediaPlayer::~MediaPlayer()
{
	quit();
}

void MediaPlayer::readStandardError()
{
	QByteArray error = mplayer_proc->readAllStandardError();
	if (!error.isEmpty())
		qWarning() << "MediaPlayer::error: " << error;
}

bool MediaPlayer::checkVideoResolution(QString track)
{
	QList<QString> args;
	args << "-identify" << "-vo" << "/dev/null" << "-frames" << "1" << track;
	QMap<QString, QString> data_search;
	data_search["resolution"] = "VIDEO:\\s+[^\\n\\r]+\\s+(\\d+x\\d+)";
	QMap<QString, QString> info = extractMPlayerInfo(global_player_executable, args, data_search, QSet<QString>() << "resolution");

	if (!info.contains("resolution"))
	{
		qWarning() << "Unknown resolution";
		return false;
	}

	QStringList res = info["resolution"].split('x');
	int width = res[0].toInt();
	int height = res[1].toInt();

	return width <= MAX_VIDEO_WIDTH && height <= MAX_VIDEO_HEIGHT;
}

bool MediaPlayer::playVideo(QString track, QRect geometry, float start_time, OutputMode write_output)
{
	QList<QString> mplayer_args = getVideoArgs(start_time);

	mplayer_args << "-vf" << QString("scale=%1:%2").arg(geometry.width()).arg(geometry.height())
		     << "-geometry" << QString("%1:%2").arg(geometry.left()).arg(geometry.top())
		     << track;
	is_video = true;

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::playVideoFullScreen(QString track, float start_time, OutputMode write_output)
{
	QList<QString> mplayer_args = getVideoArgs(start_time);
	mplayer_args << "-fs" << track;
	is_video = true;

	return runMPlayer(mplayer_args, write_output);
}
#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
void MediaPlayer::setGlobalCommandLineArguments(QString executable, QStringList audio, QStringList video)
{
	global_player_executable = executable;
	global_audio_cmdline = audio;
	global_video_cmdline = video;
}

void MediaPlayer::setCommandLineArguments(QStringList audio, QStringList video)
{
	audio_cmdline = audio;
	video_cmdline = video;
}
#endif
QList<QString> MediaPlayer::getStandardArgs()
{
	return QList<QString>() << "-nolirc" << "-slave";
}

QList<QString> MediaPlayer::getVideoArgs(float seek_time)
{
#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
	return getStandardArgs() + replaceArguments(video_cmdline, QString(), seek_time);
#else
	return getStandardArgs() << "-screenw" << QString::number(maxWidth())
			<< "-screenh" << QString::number(maxHeight())
			<< "-ac" << "mad," << "-af" << "channels=2,resample=48000"
			<< "-ao" << "oss:/dev/dsp1"
			<< "-ss" << QString::number(seek_time);
#endif
}

QList<QString> MediaPlayer::getAudioArgs(float seek_time)
{
#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
	return getStandardArgs() + replaceArguments(audio_cmdline, QString(), seek_time);
#elif defined(BT_HARDWARE_X11)
	return getStandardArgs();
#else
	return getStandardArgs() << "-ao" << MPLAYER_AUDIO_DEVICE
			<< "-ss" << QString::number(seek_time);
#endif
}

bool MediaPlayer::play(QString track, float start_time, OutputMode write_output)
{
	QList<QString> mplayer_args = getAudioArgs(start_time);

	if ((track.endsWith(".m3u", Qt::CaseInsensitive)) || (track.endsWith(".asx", Qt::CaseInsensitive)) || (track.contains(".pls", Qt::CaseInsensitive)))
	{
		mplayer_args << "-playlist";
		mplayer_args << track;
	}
	else
		mplayer_args << track;
	is_video = false;

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::runMPlayer(const QList<QString> &args, OutputMode write_output)
{
	if (mplayer_proc->state() != QProcess::NotRunning)
	{
		mplayer_proc->kill();
		mplayer_proc->waitForFinished();
	}

	active = true;

	if (!(write_output & OutputStdout))
		mplayer_proc->setStandardOutputFile("/dev/null");
	if (!(write_output & OutputStderr))
		mplayer_proc->setStandardErrorFile("/dev/null");

	qDebug() << "About to start mplayer exec (" << global_player_executable << ") with args: " << args;
	mplayer_proc->start(global_player_executable, args);
	paused = really_paused = false;

	bool started = mplayer_proc->waitForStarted(300);
	if (started)
		emit mplayerStarted();

	return started;
}

void MediaPlayer::pause()
{
	if (!active)
		return;

	if (paused)
		return;
	// we set the paused flag here to avoid two consecutive calls to pause()
	// leaving the player in playback state; see also the comment about isPaused
	paused = true;
	execCmd("pause");
}

void MediaPlayer::actuallyPaused()
{
	// scenario:
	// - call to pause()
	// - call to resume()
	// - mplayer processes the pause command and pauses
	// - mplayer processes the resume command and resumes right after the pause
	//
	// this condition is here to avoid emitting the mplayerPaused() after the
	// mplayerResumed() emitted by resume(); another (more complicated) option would
	// be to queue the command in resume and only execute it here after mplayer actually
	// pauses
	if (paused)
	{
		really_paused = true;
		emit mplayerPaused();
#ifdef LAYOUT_TS_10
		updateDirectAccessState(false);
#endif
	}
}

void MediaPlayer::resume()
{
	if (!active)
		return;

	if (paused)
	{
		// When the user clicks two times on the seek forward (or back) button
		// mplayer print the pause string, so we need to cleanup its output buffer.
		mplayer_proc->readAll();
		execCmd("pause");
		paused = really_paused = false;
		emit mplayerResumed();
	}
}

void MediaPlayer::seek(int seconds)
{
	if (!active)
		return;

	QByteArray cmd("pausing_keep seek ");
	cmd += QByteArray::number(seconds);
	execCmd(cmd);
}

void MediaPlayer::execCmd(const QByteArray &command)
{
	if (mplayer_proc->write(command + "\n") < -1)
		qDebug() << "Error MediaPlayer::execCmd():" << mplayer_proc->errorString();
}

bool MediaPlayer::isPlaying()
{
	return isInstanceRunning() && !isPaused();
}

bool MediaPlayer::isInstanceRunning()
{
	return (active && mplayer_proc->state() == QProcess::Running);
}

QMap<QString, QString> MediaPlayer::getVideoInfo(int msecs_timeout)
{
	/// Define Search Data Map
	QMap<QString, QString> data_search = getVideoDataSearchMap();

	return getMediaInfo(data_search, msecs_timeout);
}

void MediaPlayer::requestInitialPlayingInfo(const QString &track)
{
	if (info_watcher)
	{
		info_watcher->deleteLater();
		info_watcher = 0;
	}
	info_watcher = new QFutureWatcher<QMap<QString, QString> >(this);
	connect(info_watcher, SIGNAL(finished()), SLOT(infoReceived()));

	QFuture<QMap<QString,QString> > future = QtConcurrent::run(startFakePlayer, global_player_executable, track, getAudioDataSearchMap());
	info_watcher->setFuture(future);
}

void MediaPlayer::requestInitialVideoInfo(const QString &track)
{
	Q_UNUSED(track);

	QMap<QString, QString> info;
	info.insert("current_time", "00:00");

	emit playingInfoUpdated(info);
}

QMap<QString, QString> MediaPlayer::getPlayingInfo(int msecs_timeout)
{
	QMap<QString, QString> data_search = getAudioDataSearchMap();

	return getMediaInfo(data_search, msecs_timeout);
}

QMap<QString, QString> MediaPlayer::getMediaInfo(const QMap<QString, QString> &data_search, int msecs_timeout)
{
	if (!active)
		return QMap<QString, QString>();

	if (msecs_timeout)
		mplayer_proc->waitForReadyRead(msecs_timeout);

	/// READ RAW output from MPlayer
	QString raw_data = QString::fromUtf8(mplayer_proc->readAll().constData());

	/// Create output Map
	QMap<QString, QString> info_data = parsePlayerOutput(raw_data, data_search);

	QRegExp paused_rx("=====  PAUSE  =====");
	if (paused_rx.indexIn(raw_data) > -1)
		actuallyPaused();

	return info_data;
}

void MediaPlayer::quit()
{
	if (!active)
		return;

	if (mplayer_proc->state() == QProcess::Running)
	{
		mplayer_proc->terminate();
		qDebug("MediaPlayer::quit() waiting for mplayer to quit...");
		if (!mplayer_proc->waitForFinished(300))
			qWarning() << "Couldn't terminate mplayer";
	}
}

void MediaPlayer::stop()
{
	// simulate player termination when the player is logically paused
	if (!active && paused)
	{
		paused = false;
		emit mplayerStopped();
	}

	if (!active)
		return;

	paused = false;
	quit();
}

void MediaPlayer::mplayerFinished(int exit_code, QProcess::ExitStatus exit_status)
{
	if (!active)
		return;
	active = false;
#ifdef LAYOUT_TS_10
	updateDirectAccessState(false);
#endif

	if (exit_status == QProcess::CrashExit)
	{
		emit mplayerStopped();
		return;
	}
	else
	{
		qDebug("[AUDIO] mplayer exited, with code %d", exit_code);
		if (exit_code == 0) //end of song
		{
			emit mplayerDone();
			return;
		}
		else if(exit_code == 1) //signal received
		{
			emit mplayerStopped();
			return;
		}
	}
}

void MediaPlayer::mplayerError(QProcess::ProcessError error)
{
	int idx = mplayer_proc->metaObject()->indexOfEnumerator("ProcessError");
	QMetaEnum e = mplayer_proc->metaObject()->enumerator(idx);
	qDebug() << "[AUDIO] mplayer_proc raised an error: " << "'" << e.key(error) << "'";
}

#ifdef LAYOUT_TS_10
void MediaPlayer::playbackStarted()
{

	if (!active)
		return;
	updateDirectAccessState(true);
}

void MediaPlayer::updateDirectAccessState(bool state)
{
	if (is_video)
		bt_global::display->setDirectScreenAccess(state);
	bt_global::audio_states->setMediaPlayerActive(state);
	bt_global::audio_states->setDirectAudioAccess(state);
}
#endif

void MediaPlayer::infoReceived()
{
	if (info_watcher)
	{
		QMap<QString, QString> info = info_watcher->result();
		emit playingInfoUpdated(info);

		info_watcher->deleteLater();
		info_watcher = 0;
	}
}


Q_GLOBAL_STATIC(QProcess, sox_process)

#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
QString SoundPlayer::player_executable;
QStringList SoundPlayer::audio_cmdline;
#endif

SoundPlayer::SoundPlayer(QObject *parent) : QObject(parent)
{
	connect(sox_process(), SIGNAL(readyReadStandardError()), SLOT(readStandardError()));
	connect(sox_process(), SIGNAL(error(QProcess::ProcessError)), SLOT(error()));
	connect(sox_process(), SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(processFinished()));
#ifdef LAYOUT_TS_10
	connect(bt_global::audio_states, SIGNAL(stateAboutToChange(int)), SLOT(audioStateAboutToChange()));
#endif
	active = false;
}

void SoundPlayer::readStandardError()
{
	QByteArray error = sox_process()->readAllStandardError();
	if (!error.isEmpty())
		qWarning() << "SoundPlayer::error: " << error;
}

void SoundPlayer::audioStateAboutToChange()
{
	stop();
}

void SoundPlayer::processFinished()
{
	if (!to_play.isNull())
		start();
	else if (active)
	{
		active = false;
		emit soundFinished();
#ifdef LAYOUT_TS_10
		// The order is important! Releasing the audio resource should be the last
		// instruction (because on that event the audio state machine performs
		// its state change).
		bt_global::audio_states->setDirectAudioAccess(false);
#endif
	}
}

void SoundPlayer::play(const QString &path)
{
	to_play = path;
	if (sox_process()->state() != QProcess::NotRunning)
	{
		// We want to wait the end of the running process before starting the new one
		sox_process()->terminate();
	}
	else
	{
#ifdef LAYOUT_TS_10
		bt_global::audio_states->setDirectAudioAccess(true);
#endif
		start();
		emit soundStarted();
	}
}

bool SoundPlayer::isActive() const
{
	return active;
}

void SoundPlayer::start()
{
	// if we are here and the process is already active, there were two pending
	// requests to play; wait for our turn
	if (sox_process()->state() != QProcess::NotRunning)
		return;
	active = true;

#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
	sox_process()->start(player_executable, replaceArguments(audio_cmdline, to_play, 0));
#else
	QPair<QString, QStringList> cmdline = getAudioCmdLine(to_play);
	sox_process()->start(cmdline.first, cmdline.second);
#endif
	to_play = QString();
}

void SoundPlayer::error()
{
	if (!active)
		return;

	qWarning() << "SoundPlayer::error" << sox_process()->errorString();
}

void SoundPlayer::stop()
{
	if (!active)
		return;

	sox_process()->terminate();
}
#ifdef MEDIAPLAYER_DISABLE_HARDWARE_FUNCTIONS
void SoundPlayer::setGlobalCommandLineArguments(QString executable, QStringList arguments)
{
	player_executable = executable;
	audio_cmdline = arguments;
}
#endif
// The global definition of sound player pointer
SoundPlayer *bt_global::sound = 0;
