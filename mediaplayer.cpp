#include "mediaplayer.h"
#include "hardware_functions.h" // maxWidth, maxHeight

#include <QRegExp>
#include <QDebug>
#include <QRect>
#include <QFile>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


static const char *MPLAYER_FILENAME = "/usr/bin/mplayer";

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

	const char *mplayer_args[19];

	QByteArray t = track.toLocal8Bit();
	QByteArray pos = QString("%1:%2").arg(geometry.left()).arg(geometry.top()).toAscii();
	QByteArray size = QString("scale=%1:%2").arg(geometry.width()).arg(geometry.height()).toAscii();
	QByteArray width = QString::number(maxWidth()).toAscii();
	QByteArray height = QString::number(maxHeight()).toAscii();

	mplayer_args[0] = MPLAYER_FILENAME;
	mplayer_args[1] = "-vf";
	mplayer_args[2] = size.constData();
	mplayer_args[3] = "-geometry";
	mplayer_args[4] = pos.constData();
	mplayer_args[5] = "-screenw";
	mplayer_args[6] = width.constData();
	mplayer_args[7] = "-screenh";
	mplayer_args[8] = height.constData();
	mplayer_args[9] = "-ac";
	mplayer_args[10] = "mad,";
	mplayer_args[11] = "-af";
	mplayer_args[12] = "channels=2,resample=48000";
	mplayer_args[13] = "-ao";
	mplayer_args[14] = "oss:/dev/dsp1";
	mplayer_args[15] = "-input";
	mplayer_args[16] = "file=/tmp/start_seek";
	mplayer_args[17] = t.constData();
	mplayer_args[18] = NULL;

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::playVideoFullScreen(QString track, int start_time, bool write_output)
{
	QFile sk("/tmp/start_seek");

	sk.open(QFile::WriteOnly);
	sk.write(QString("seek %1 2\n").arg(start_time).toAscii());
	sk.close();

	const char *mplayer_args[20];

	QByteArray t = track.toLocal8Bit();
	// QByteArray size = QString("scale=%1:%2").arg(geometry.width()).arg(geometry.height()).toAscii();
	QByteArray width = QString::number(maxWidth()).toAscii();
	QByteArray height = QString::number(maxHeight()).toAscii();

	mplayer_args[0] = MPLAYER_FILENAME;
	mplayer_args[1] = "-screenw";
	mplayer_args[2] = width.constData();
	mplayer_args[3] = "-screenh";
	mplayer_args[4] = height.constData();
	mplayer_args[5] = "-ac";
	mplayer_args[6] = "mad,";
	mplayer_args[7] = "-af";
	mplayer_args[8] = "channels=2,resample=48000";
	mplayer_args[9] = "-ao";
	mplayer_args[10] = "oss:/dev/dsp1";
	mplayer_args[11] = "-input";
	mplayer_args[12] = "file=/tmp/start_seek";
	mplayer_args[13] = "-fs";
	mplayer_args[14] = t.constData();
	mplayer_args[15] = NULL;

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::play(QString track, bool write_output)
{
	const char *mplayer_args[] = {MPLAYER_FILENAME, NULL, NULL, NULL, NULL, NULL};

	QByteArray t = track.toLocal8Bit();
	if ((track.endsWith(".m3u", Qt::CaseInsensitive)) || (track.endsWith(".asx", Qt::CaseInsensitive)))
	{
		mplayer_args[1] = "-playlist";
		mplayer_args[2] = t.constData();
	}
	else
		mplayer_args[1] = t.constData();

	return runMPlayer(mplayer_args, write_output);
}

bool MediaPlayer::runMPlayer(const char *mplayer_args[], bool write_output)
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
		execCmd(" ");
		paused = true;
	}
}

void MediaPlayer::resume()
{
	if (paused)
	{
		execCmd(" ");
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

void MediaPlayer::execCmd(QString command)
{
	if (ctrlf)
	{
		QByteArray c = command.toLatin1();
		fprintf(ctrlf, c.constData());
		fflush(ctrlf);
	}
	else
		qDebug("[AUDIO] MediaPlayer::execCmd(): mplayer not running");
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

	/// READ ROW output from MPlayer
	QString row_data = readOutput();

	/// Create output Map
	QMap<QString, QString> info_data;

	/// Parse ROW data to get info
	QMap<QString, QString>::Iterator it;
	for (it = data_search.begin(); it != data_search.end(); ++it)
	{
		QRegExp rx(it.value());

		if (rx.indexIn(row_data) > -1)
			info_data[it.key()] = rx.cap(1);
	}

	return info_data;
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

	/// READ ROW output from MPlayer
	QString row_data = readOutput();

	/// Create output Map
	QMap<QString, QString> info_data;

	/// Parse ROW data to get info
	QMap<QString, QString>::Iterator it;
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
