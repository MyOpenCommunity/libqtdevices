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
#include "hardware_functions.h" // maxWidth, maxHeight, getAudioCmdLine
#include "displaycontrol.h"
#include "audiostatemachine.h"

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

#ifdef BT_HARDWARE_BTOUCH
static const char *MPLAYER_FILENAME = "/usr/bin/mplayer";
#elif BT_HARDWARE_X11
static const char *MPLAYER_FILENAME = "mplayer";
#else // BT_HARDWARE_TOUCHX
static const char *MPLAYER_FILENAME = "/home/bticino/cfg/extra/10/mplayer";
#endif


namespace
{
	QMap<QString, QString> getAudioDataSearchMap()
	{
		/// Define Search Data Map
		QMap<QString, QString> data_search;
		data_search["file_name"]    = "Playing [^\\n]*([^/\\n]+)\\.\\n";
		data_search["meta_title"]   = "Title: ([^\\n]*)\\n";
		data_search["meta_artist"]  = "Artist: ([^\\n]*)\\n";
		data_search["meta_album"]   = "Album: ([^\\n]*)\\n";
		data_search["total_time"]   = "of\\s+\\d+\\.\\d+\\s+[(](\\d+:\\d+\\.\\d+)[)]";
		data_search["current_time"] = "A:\\s+\\d+\\.\\d+\\s+[(](\\d*:*\\d+\\.\\d+)[)]";
		// shoutcast info
		// MPlayer does not quote "'" inside titles, so parsing the shoutcast info
		// with a regex is tricky; the regexes below can be improved
		data_search["stream_title"] = "\\bStreamTitle='([^;]+)';";
		data_search["stream_url"] = "\\bStreamUrl='([^;]+)';";

		return data_search;
	}

	QMap<QString,QString> getVideoDataSearchMap()
	{
		/// Define Search Data Map
		QMap<QString, QString> data_search;
		data_search["current_time"] = "A:\\s+(\\d+\\.\\d+)\\s+";

		return data_search;
	}

	QMap<QString,QString> parsePlayerOutput(const QString &raw_data, QMap<QString,QString> data_search)
	{
		/// Create output Map
		QMap<QString, QString> info_data;

		/// Parse RAW data to get info
		QMap<QString, QString>::ConstIterator it;
		for (it = data_search.begin(); it != data_search.end(); ++it)
		{
			QRegExp rx(it.value());

			if (rx.indexIn(raw_data) > -1)
				info_data[it.key()] = rx.cap(1);
		}

		return info_data;
	}

	QMap<QString, QString> startFakePlayer(const QString &track, QMap<QString,QString> data_search)
	{
		qDebug() << "Started fake player";
		QList<QString> args;
		args << "-nolirc" << "-ao" << "null" << "-vo" << "null" << track;
		QProcess fake_player;
		fake_player.start(MPLAYER_FILENAME, args);
		fake_player.waitForStarted(300);

		QMap<QString, QString> info_data;
		QString raw_data;
		bool wait = true;
		while (wait)
		{
			fake_player.waitForReadyRead(300);
			raw_data.append(fake_player.readAll());

			info_data = parsePlayerOutput(raw_data, data_search);

			// Wait until the current time info because it's the last, always present
			// info of the mplayer output.
			if (!info_data["current_time"].isEmpty())
				wait = false;
		}
		fake_player.terminate();
		fake_player.waitForFinished(300);

		return info_data;
	}
}


QProcess MediaPlayer::mplayer_proc;


MediaPlayer::MediaPlayer(QObject *parent) : QObject(parent)
{
	active = false;
	is_video = false;
	paused = false;
	info_watcher = 0;
	connect(&mplayer_proc, SIGNAL(readyReadStandardError()), SLOT(readStandardError()));
	connect(&mplayer_proc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(mplayerFinished(int, QProcess::ExitStatus)));
	connect(&mplayer_proc, SIGNAL(error(QProcess::ProcessError)), SLOT(mplayerError(QProcess::ProcessError)));
	connect(this, SIGNAL(mplayerResumed()), SLOT(playbackStarted()));
	connect(this, SIGNAL(mplayerStarted()), SLOT(playbackStarted()));
}

void MediaPlayer::readStandardError()
{
	QByteArray error = mplayer_proc.readAllStandardError();
	if (!error.isEmpty())
		qWarning() << "MediaPlayer::error: " << error;
}

bool MediaPlayer::playVideo(QString track, QRect geometry, int start_time, bool write_output)
{
	QList<QString> mplayer_args = getStandardArgs();
	mplayer_args.append(getVideoArgs(start_time));

	mplayer_args << "-vf" << QString("scale=%1:%2").arg(geometry.width()).arg(geometry.height())
		     << "-geometry" << QString("%1:%2").arg(geometry.left()).arg(geometry.top())
		     << track;
	is_video = true;

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::playVideoFullScreen(QString track, int start_time, bool write_output)
{
	QList<QString> mplayer_args = getStandardArgs();
	mplayer_args.append(getVideoArgs(start_time));

	mplayer_args << "-fs"
		     << track;
	is_video = true;

	return runMPlayer(mplayer_args, write_output);
}

QList<QString> MediaPlayer::getStandardArgs()
{
	return QList<QString>() << "-nolirc" << "-slave";
}

QList<QString> MediaPlayer::getVideoArgs(int seek_time)
{
	return QList<QString>() << "-screenw" << QString::number(maxWidth())
			<< "-screenh" << QString::number(maxHeight())
			<< "-ac" << "mad," << "-af" << "channels=2,resample=48000"
			<< "-ao" << "oss:/dev/dsp1"
			<< "-ss" << QString::number(seek_time);
}

QList<QString> MediaPlayer::getAudioArgs(int seek_time)
{
#ifdef BT_HARDWARE_X11
	return QList<QString>();
#else
	return QList<QString>() << "-ac" << "mad," << "-af" << "channels=2,resample=48000"
			<< "-ao" << "oss:/dev/dsp1"
			<< "-ss" << QString::number(seek_time);
#endif
}

bool MediaPlayer::play(QString track, bool write_output)
{
	QList<QString> mplayer_args = getStandardArgs();
	mplayer_args.append(getAudioArgs(0));

	if ((track.endsWith(".m3u", Qt::CaseInsensitive)) || (track.endsWith(".asx", Qt::CaseInsensitive)))
	{
		mplayer_args << "-playlist";
		mplayer_args << track;
	}
	else
		mplayer_args << track;
	is_video = false;

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::runMPlayer(const QList<QString> &args, bool write_output)
{
	if (mplayer_proc.state() != QProcess::NotRunning)
	{
		mplayer_proc.terminate();
		mplayer_proc.waitForFinished();
	}

	active = true;

	if (!write_output)
		mplayer_proc.setStandardOutputFile("/dev/null");

	mplayer_proc.start(MPLAYER_FILENAME, args);
	paused = false;

	bool started = mplayer_proc.waitForStarted(300);
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
	emit mplayerPaused();
	updateDirectAccessState(false);
}

void MediaPlayer::resume()
{
	if (!active)
		return;

	if (paused)
	{
		execCmd("pause");
		paused = false;
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
	if (mplayer_proc.write(command + "\n") < -1)
		qDebug() << "Error MediaPlayer::execCmd():" << mplayer_proc.errorString();
}

bool MediaPlayer::isInstanceRunning()
{
	return (active && mplayer_proc.state() == QProcess::Running);
}

QMap<QString, QString> MediaPlayer::getVideoInfo()
{
	/// Define Search Data Map
	QMap<QString, QString> data_search = getVideoDataSearchMap();

	return getMediaInfo(data_search);
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

	QFuture<QMap<QString,QString> > future = QtConcurrent::run(startFakePlayer, track, getAudioDataSearchMap());
	info_watcher->setFuture(future);
}

void MediaPlayer::requestInitialVideoInfo(const QString &track)
{
	QMap<QString, QString> info;
	info.insert("current_time", "00:00");

	emit playingInfoUpdated(info);
}

QMap<QString, QString> MediaPlayer::getPlayingInfo()
{
	QMap<QString, QString> data_search = getAudioDataSearchMap();

	return getMediaInfo(data_search);
}

QMap<QString, QString> MediaPlayer::getMediaInfo(const QMap<QString, QString> &data_search)
{
	if (!active)
		return QMap<QString, QString>();

	/// READ RAW output from MPlayer
	QString raw_data = mplayer_proc.readAll();

	/// Create output Map
	QMap<QString, QString> info_data = parsePlayerOutput(raw_data, data_search);

	QRegExp paused("=====  PAUSE  =====");
	if (paused.indexIn(raw_data) > -1)
		actuallyPaused();

	return info_data;
}

void MediaPlayer::quit()
{
	if (!active)
		return;

	if (mplayer_proc.state() == QProcess::Running)
	{
		mplayer_proc.terminate();
		qDebug("MediaPlayer::quit() waiting for mplayer to quit...");
		if (!mplayer_proc.waitForFinished(300))
			qWarning() << "Couldn't terminate mplayer";
		paused = false;
	}
}

void MediaPlayer::mplayerFinished(int exit_code, QProcess::ExitStatus exit_status)
{
	if (!active)
		return;
	active = false;
	updateDirectAccessState(false);

	if (exit_status == QProcess::CrashExit)
	{
		emit mplayerAborted();
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
			emit mplayerKilled();
			return;
		}
	}
}

void MediaPlayer::mplayerError(QProcess::ProcessError error)
{
	int idx = mplayer_proc.metaObject()->indexOfEnumerator("ProcessError");
	QMetaEnum e = mplayer_proc.metaObject()->enumerator(idx);
	qDebug() << "[AUDIO] mplayer_proc raised an error: " << e.key(error);
}

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

void MediaPlayer::infoReceived()
{
	if (info_watcher)
	{
		QMap<QString, QString> info = info_watcher->result();
		qDebug() << "Received track info:" << info;
		emit playingInfoUpdated(info);

		info_watcher->deleteLater();
		info_watcher = 0;
	}
}


Q_GLOBAL_STATIC(QProcess, sox_process);

SoundPlayer::SoundPlayer(QObject *parent) : QObject(parent)
{
	connect(sox_process(), SIGNAL(readyReadStandardError()), SLOT(readStandardError()));
	connect(sox_process(), SIGNAL(error(QProcess::ProcessError)), SLOT(error()));
	connect(sox_process(), SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(processFinished()));
	connect(bt_global::audio_states, SIGNAL(stateAboutToChange(int)), SLOT(audioStateAboutToChange()));
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
	if (!active)
		return;

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
		// The order is important! Releasing the audio resource should be the last
		// instruction (because on that event the audio state machine performs
		// its state change).
		bt_global::audio_states->setDirectAudioAccess(false);
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
		bt_global::audio_states->setDirectAudioAccess(true);
		start();
	}
}

void SoundPlayer::start()
{
	// if we are here and the process is already active, there were two pending
	// requests to play; wait for our turn
	if (sox_process()->state() != QProcess::NotRunning)
		return;
	active = true;

	QPair<QString, QStringList> cmdline = getAudioCmdLine(to_play);
	sox_process()->start(cmdline.first, cmdline.second);
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

// The global definition of sound player pointer
SoundPlayer *bt_global::sound = 0;
