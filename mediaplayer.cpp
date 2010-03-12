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

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


static const char *MPLAYER_FILENAME = "/usr/bin/mplayer";
static const char *MPLAYER_FIFO = "/tmp/mplayer_fifo";

// Dirty kludge to allow mplayerExited access to class instance.
static MediaPlayer *_globalMediaPlayer;

static void mplayerExited(int signo, siginfo_t *info, void *)
{
	int status;
	waitpid(-1, &status, WNOHANG);

	if (_globalMediaPlayer)
		_globalMediaPlayer->sigChildReceived(info->si_pid, status);
}

MediaPlayer::MediaPlayer(QObject *parent) : QObject(parent)
{
	if (!QFile::exists(MPLAYER_FIFO))
		system(QByteArray("mkfifo ") + MPLAYER_FIFO);
	struct sigaction sa;

	memset( &sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGCHLD);
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sa.sa_sigaction = ::mplayerExited;
	sigaction(SIGCHLD, &sa, NULL);

	mplayer_pid = 0;
	paused = false;
	fullscreen = false;
	_isPlaying = false;
	_globalMediaPlayer = this;

	ctrlf = NULL;
	outf = NULL;
}

MediaPlayer::~MediaPlayer()
{
	_globalMediaPlayer = NULL;
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
		     << "-slave"
		     << "-input"
		     << QString("file=") + MPLAYER_FIFO;

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
	QList<QByteArray> byte_args;
	QVector<const char *> mplayer_args;

	for (int i = 0; i < args.size(); ++i)
	{
		byte_args.append(args[i].toLocal8Bit());
		mplayer_args.append(byte_args[i].constData());
	}
	mplayer_args.append(NULL);

	return runMPlayer(mplayer_args.constData(), write_output);
}

bool MediaPlayer::runMPlayer(const char * const mplayer_args[], bool write_output)
{
	int control_pipe[2];
	int output_pipe[2];

	if (pipe(control_pipe) < 0)
		return false;
	if (pipe(output_pipe) < 0)
		return false;

	mplayer_pid = fork();
	if (mplayer_pid == -1)
		return false;

	if (mplayer_pid == 0)
	{
		// CHILD
		// Close unused write end
		close(control_pipe[1]);
		close(output_pipe[0]);

		dup2(control_pipe[0], STDIN_FILENO);

		if (write_output)
			dup2(output_pipe[1], STDOUT_FILENO);
		else
		{
			int nullfd = open("/dev/null", O_WRONLY);
			if (nullfd != -1)
				dup2(nullfd, STDOUT_FILENO);
			else
				qDebug("[AUDIO] unable to open /dev/null");
		}

		execve(MPLAYER_FILENAME, const_cast<char * const *>(mplayer_args), environ);
	}
	else
	{
		// PARENT
		_isPlaying = true;
		paused = false;

		close(control_pipe[0]);
		close(output_pipe[1]);

		control_fd = control_pipe[1];
		output_fd  = output_pipe[0];

		// Make file descriptors NON BLOCKING
		fcntl(control_fd, F_SETFL, O_NONBLOCK);
		fcntl(output_fd, F_SETFL, O_NONBLOCK);

		ctrlf = fdopen(control_fd, "w");
		outf  = fdopen(output_fd, "r");
	}

	return true;
}

void MediaPlayer::pause()
{
	if (!paused)
	{
		execCmd("pause\n");
		paused = true;
	}
}

void MediaPlayer::resume()
{
	if (paused)
	{
		execCmd("pause\n");
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

void MediaPlayer::execCmd(QByteArray command)
{
	if (QFile::exists(MPLAYER_FIFO))
	{
		QFile f(MPLAYER_FIFO);
		f.open(QFile::WriteOnly);
		f.write(command);
	}
	else
		qDebug("[AUDIO] MediaPlayer::execCmd(): error, no control fifo found");
}

QString MediaPlayer::readOutput()
{
	char line[1024];
	QString result;

	if (outf)
	{
		while (fgets(line, sizeof(line), outf))
			result += line;
	}
	else
		qDebug("[AUDIO] MediaPlayer::readOutput(): mplayer not running");

	return result;
}


bool MediaPlayer::isInstanceRunning()
{
	return _isPlaying;
}


QMap<QString, QString> MediaPlayer::getVideoInfo()
{
	/// Define Search Data Map
	QMap<QString, QString> data_search;
	data_search["current_time"] = "A:\\s+(\\d+\\.\\d+)\\s+";

	return getMediaInfo(data_search);
}


QMap<QString, QString> MediaPlayer::getPlayingInfo()
{
	/// Define Search Data Map
	QMap<QString, QString> data_search;
	data_search["file_name"]    = "Playing [^\\n]*([^/\\n]+)\\.\\n";
	data_search["meta_title"]   = "Title: ([^\\n]*)\\n";
	data_search["meta_artist"]  = "Artist: ([^\\n]*)\\n";
	data_search["meta_album"]   = "Album: ([^\\n]*)\\n";
	data_search["meta_year"]    = "Year: ([^\\n]*)\\n";
	data_search["meta_comment"] = "Comment: ([^\\n]*)\\n";
	data_search["meta_genre"]   = "Genre: ([^\\n]*)\\n";
	data_search["total_time"]   = "[(](\\d+:\\d+\\.\\d+)[)] \\d+\\.\\d+";
	data_search["current_time"] = "A:\\s+\\d+\\.\\d+\\s+[(](\\d*:*\\d+\\.\\d+)[)]";

	return getMediaInfo(data_search);
}

QMap<QString, QString> MediaPlayer::getMediaInfo(const QMap<QString, QString> &data_search)
{
	/// READ ROW output from MPlayer
	QString row_data = readOutput();

	/// Create output Map
	QMap<QString, QString> info_data;

	/// Parse ROW data to get info
	QMap<QString, QString>::ConstIterator it;
	for (it = data_search.begin(); it != data_search.end(); ++it)
	{
		QRegExp rx(it.value());

		if (rx.indexIn(row_data) > -1)
			info_data[it.key()] = rx.cap(1);
	}

	return info_data;
}


void MediaPlayer::quit()
{
	if (mplayer_pid)
		kill(mplayer_pid, SIGINT);
}

void MediaPlayer::quitAndWait()
{
	quit();
	// wait for the player to terminate
	while (isInstanceRunning())
		;
}

void MediaPlayer::sigChildReceived(int dead_pid, int status)
{
	/// Check if the dead child is mplayer or not
	if (dead_pid == mplayer_pid)
	{
		mplayer_pid = 0;
		_isPlaying = false;
		ctrlf = NULL;
		outf = NULL;

		if (WIFEXITED(status))
		{
			int rc = WEXITSTATUS(status);
			qDebug("[AUDIO] mplayer exited, with code %d", rc);
			if (rc == 0) //end of song
			{
				emit mplayerDone();
				return;
				
			}
			else if(rc == 1) //signal received
			{
				emit mplayerKilled();
				return;
			}
		}
		else if (WIFSIGNALED(status))
		{
			qDebug("[AUDIO] mplayer terminated by signal %d", WTERMSIG(status));
			if (WTERMSIG(status) == SIGINT)
			{
				emit mplayerKilled();
				return;
			}
		}
		else
			qDebug("[AUDIO] mplayer aborted for unknown reason");

		emit mplayerAborted();
	}
}
