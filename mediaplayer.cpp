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
#include "hardware_functions.h" // maxWidth, maxHeight

#include <QRegExp>
#include <QDebug>
#include <QRect>
#include <QFile>
#include <QVector>
#include <QList>
#include <QMetaEnum>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


static const char *MPLAYER_FILENAME = "/usr/bin/mplayer";

MediaPlayer::MediaPlayer(QObject *parent) : QObject(parent)
{
	paused = false;
	fullscreen = false;
	connect(&mplayer_proc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(mplayerFinished(int, QProcess::ExitStatus)));
	connect(&mplayer_proc, SIGNAL(error(QProcess::ProcessError)), SLOT(mplayerError(QProcess::ProcessError)));
}

bool MediaPlayer::playVideo(QString track, QRect geometry, int start_time, bool write_output)
{
	QFile sk("/tmp/start_seek");

	sk.open(QFile::WriteOnly);
	sk.write(QString("seek %1 2\n").arg(start_time).toAscii());
	sk.close();

	QList<QString> mplayer_args;

	mplayer_args << MPLAYER_FILENAME
		     << "-vf" << QString("scale=%1:%2").arg(geometry.width()).arg(geometry.height())
		     <<	"-geometry" << QString("%1:%2").arg(geometry.left()).arg(geometry.top())
		     << "-screenw" << QString::number(maxWidth())
		     << "-screenh" << QString::number(maxHeight())
		     << "-ac" << "mad," << "-af" << "channels=2,resample=48000"
		     << "-ao" << "oss:/dev/dsp1"
		     << "-input" << "file=/tmp/start_seek"
		     << track;

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::playVideoFullScreen(QString track, int start_time, bool write_output)
{
	QFile sk("/tmp/start_seek");

	sk.open(QFile::WriteOnly);
	sk.write(QString("seek %1 2\n").arg(start_time).toAscii());
	sk.close();

	QList<QString> mplayer_args;

	mplayer_args << MPLAYER_FILENAME
		     << "-screenw" << QString::number(maxWidth())
		     << "-screenh" << QString::number(maxHeight())
		     << "-ac" << "mad," << "-af" << "channels=2,resample=48000"
		     << "-ao" << "oss:/dev/dsp1"
		     << "-input" << "file=/tmp/start_seek"
		     << "-fs"
		     << track;

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::play(QString track, bool write_output)
{
	QList<QString> mplayer_args;
	mplayer_args << MPLAYER_FILENAME
		     << "-nolirc"
		     << "-slave";

	QByteArray t = track.toLocal8Bit();
	if ((track.endsWith(".m3u", Qt::CaseInsensitive)) || (track.endsWith(".asx", Qt::CaseInsensitive)))
	{
		mplayer_args << "-playlist";
		mplayer_args << t.constData();
	}
	else
		mplayer_args << t.constData();

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::runMPlayer(const QList<QString> &args, bool write_output)
{
	if (!write_output)
		mplayer_proc.setStandardOutputFile("/dev/null");

	mplayer_proc.start(MPLAYER_FILENAME, args);
	paused = false;
	return mplayer_proc.waitForStarted(300);
}

void MediaPlayer::pause()
{
	if (!paused)
	{
		execCmd("pause");
		paused = true;
	}
}

void MediaPlayer::resume()
{
	if (paused)
	{
		execCmd("pause");
		paused = false;
	}
}

void MediaPlayer::seek(int seconds)
{
	// at some point we should probably switch to slave MPlayer
	Q_ASSERT_X(seconds == 10 || seconds == -10, "MediaPlayer::seek", "Can only seek 10 seconds");

	// the keybindings can be remapped in /.mplayer/input.conf
	if (seconds < 0)
		execCmd("\x1b[D"); // left arrow
	else
		execCmd("\x1b[C"); // right arrow
}

void MediaPlayer::setFullscreen(bool fs)
{
	if (fs == fullscreen)
		return;
	execCmd("f");
	fullscreen = fs;
}

void MediaPlayer::execCmd(const QByteArray &command)
{
	if (mplayer_proc.write(command + "\n") < -1)
		qDebug() << "Error MediaPlayer::execCmd():" << mplayer_proc.errorString();
}

bool MediaPlayer::isInstanceRunning()
{
	return (mplayer_proc.state() == QProcess::Running);
}

QMap<QString, QString> MediaPlayer::getVideoInfo()
{
	/*
	/// Define Search Data Map
	QMap<QString, QString> data_search;
	data_search["current_time"] = "A:\\s+(\\d+\\.\\d+)\\s+";

	return getMediaInfo(data_search);
	*/
	// TODO: reimplement
	return QMap<QString, QString>();
}

QMap<QString, QString> MediaPlayer::getPlayingInfo()
{
	/// Define Search Data Map
	QMap<QString, QString> data_search;
	data_search["file_name"]    = "Playing [^\\n]*([^/\\n]+)\\.\\n";
	data_search["meta_title"]   = "Title: ([^\\n]*)\\n";
	data_search["meta_artist"]  = "Artist: ([^\\n]*)\\n";
	data_search["meta_album"]   = "Album: ([^\\n]*)\\n";
	data_search["total_time"]   = "[(](\\d+:\\d+\\.\\d+)[)]";
	data_search["current_time"] = "A:\\s+\\d+\\.\\d+\\s+[(](\\d*:*\\d+\\.\\d+)[)]";

	return getMediaInfo(data_search);
}

QMap<QString, QString> MediaPlayer::getMediaInfo(const QMap<QString, QString> &data_search)
{
	/// READ RAW output from MPlayer
	QString raw_data = mplayer_proc.readAll();

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

void MediaPlayer::quit()
{
	if (mplayer_proc.state() == QProcess::Running)
	{
		mplayer_proc.terminate();
		qDebug("MediaPlayer::quit() waiting for mplayer to quit...");
		if (!mplayer_proc.waitForFinished(300))
			qWarning() << "Couldn't terminate mplayer";
	}
}

void MediaPlayer::quitAndWait()
{
	quit();
	mplayer_proc.waitForFinished();
}

void MediaPlayer::mplayerFinished(int exit_code, QProcess::ExitStatus exit_status)
{
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
